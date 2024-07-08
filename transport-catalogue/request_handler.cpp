#include "request_handler.h"

#include <iostream>
#include <sstream>

namespace request_handler
{
    RequestHandler::RequestHandler(const transport_catalogue::TransportCatalogue& db, const renderer::MapRenderer& renderer, const json_reader::JsonReader reader)
        : db_(db), renderer_(renderer), reader_(reader)
        {
            PrintRequests();
        }


    std::optional<transport_catalogue::BusInfo> RequestHandler::GetBusStat(const std::string_view& bus_name) const
    {
        transport_catalogue::BusInfo result;
        const transport_catalogue::Bus* bus = db_.FindBus(bus_name);

        if (!bus)
        {
            throw std::invalid_argument("bus not found");
        }

        result.name = bus_name;

        if (bus->is_circle) 
        {
            result.numStops = bus->stops.size();
        } else 
        {
            result.numStops = bus->stops.size() * 2 - 1;
        }

        int routeLength = 0;
        double geographicLength = 0.0;

        for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
            auto from = bus->stops[i];
            auto to = bus->stops[i + 1];

            if (bus->is_circle) {
                routeLength += db_.GetDistance(from, to);
                geographicLength += geo::ComputeDistance(from->coods,
                                                         to->coods);
            }
            else {
                routeLength += db_.GetDistance(from, to) + db_.GetDistance(to, from);
                geographicLength += geo::ComputeDistance(from->coods,
                                                         to->coods) * 2;
            }
        }
        auto bus_temp = db_.FindBus(bus_name);
        result.numUniqueStops = db_.CalculateUniqueStops(bus_temp -> stops);
        result.routeLength = routeLength;
        result.curvature = routeLength / geographicLength;

        return result;
    }

    const std::set<std::string_view> RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
        auto stop = db_.FindStop(stop_name);
        return db_.GetBusesOnStop(*stop);
    }

    void RequestHandler::PrintRequests() const {
        json::Array requests;
        const json::Array& arr = reader_.GetStatRequests().AsArray();
        for (auto& request : arr) {
            const auto& request_map = request.AsMap();
            const auto& type = request_map.at("type").AsString();
            if (type == "Stop") {
                requests.emplace_back(PrintStop(request_map).AsMap());
            }
            if (type == "Bus") {
                requests.emplace_back(PrintBus(request_map).AsMap());
            }
            
            if (type == "Map") {
                requests.emplace_back(PrintMap(request_map).AsMap());
            }
        }
        json::Print(json::Document(requests), std::cout);
    }

    const json::Node RequestHandler::PrintBus(const json::Dict& bus_request) const {
        json::Dict result;
        const std::string& bus_name = bus_request.at("name").AsString();
        result["request_id"] = bus_request.at("id").AsInt();
        if (!db_.FindBus(bus_name)) {
            result["error_message"] = json::Node(std::string("not found"));
            return json::Node(result);
        } else {
            auto busInfo = GetBusStat(bus_name);
            result["stop_count"] = busInfo -> numStops;
            result["route_length"] = busInfo -> routeLength;
            result["unique_stop_count"] = busInfo -> numUniqueStops;
            result["curvature"] = busInfo -> curvature;
        }
        return json::Node(result);
    }

    const json::Node RequestHandler::PrintStop(const json::Dict& stop_request) const {
        json::Dict result;
        const std::string& stop_name = stop_request.at("name").AsString();
        result["request_id"] = stop_request.at("id").AsInt();

        const auto stop = db_.FindStop(stop_name);
        if (!stop) {
            result["error_message"] = json::Node(std::string("not found")); 
            return json::Node(result);
        }

        json::Array buses;
        for (auto& bus : GetBusesByStop(stop_name)) {
            buses.emplace_back((std::string)bus);
        }
        result["buses"] = buses;

        return json::Node(result);
    }
    
    svg::Document RequestHandler::RenderMap() const 
    {
        auto sorted_buses = db_.GetSortedBuses();
        return renderer_.GetSVGDocument(sorted_buses);
    }

    const json::Node RequestHandler::PrintMap(const json::Dict& map_request) const
    {
        json::Dict result;
        result["request_id"] = map_request.at("id").AsInt();
        std::ostringstream strm;
        auto mapRequest = RenderMap();
        mapRequest.Render(strm);
        result["map"] = strm.str();

        return json::Node(result);
    }

}