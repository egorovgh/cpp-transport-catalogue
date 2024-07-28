#include "request_handler.h"

#include "json_builder.h"

#include <iostream>
#include <sstream>

namespace request_handler
{
    std::optional<transport_catalogue::BusInfo> RequestHandler::GetBusStat(const std::string_view& bus_name) const
    {
        transport_catalogue::BusInfo result;
        auto bus = db_.FindBus(bus_name);

        if (!bus)
        {
            throw std::invalid_argument("bus not found");
        }

        result = db_.GetBusInfo(bus_name);

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

            if (type == "Route") {
                requests.emplace_back(PrintRoute(request_map).AsMap()); //TODO
            }
        }
        json::Print(json::Document(requests), std::cout);
    }

    svg::Document RequestHandler::RenderMap() const
    {
        auto sorted_buses = db_.GetSortedBuses();
        return renderer_.GetSVGDocument(sorted_buses);
    }

    const json::Node RequestHandler::PrintBus(const json::Dict& bus_request) const {
        auto result = json::Builder{};
        result.StartDict();
        const std::string& bus_name = bus_request.at("name").AsString();
        const int id = bus_request.at("id").AsInt();
        if (!db_.FindBus(bus_name)) {
            result.Key("request_id").Value(id)
                .Key("error_message").Value((std::string)"not found");
        }
        else {
            auto busInfo = GetBusStat(bus_name);
            result.Key("request_id").Value(id)
                .Key("stop_count").Value(busInfo->numStops)
                .Key("route_length").Value(busInfo->routeLength)
                .Key("unique_stop_count").Value(busInfo->numUniqueStops)
                .Key("curvature").Value(busInfo->curvature);
        }
        result.EndDict();
        return result.Build();
    }

    const json::Node RequestHandler::PrintStop(const json::Dict& stop_request) const {
        auto result = json::Builder{};
        result.StartDict();
        const std::string& stop_name = stop_request.at("name").AsString();
        const int id = stop_request.at("id").AsInt();

        const auto stop = db_.FindStop(stop_name);
        if (!stop) {
            result.Key("request_id").Value(id)
                .Key("error_message").Value((std::string)"not found");
        }
        else
        {
            json::Array buses;
            for (auto& bus : GetBusesByStop(stop_name)) {
                buses.emplace_back((std::string)bus);
            }
            result.Key("request_id").Value(id)
                .Key("buses").Value(buses);
        }
        result.EndDict();
        return result.Build();
    }


    const json::Node RequestHandler::PrintMap(const json::Dict& map_request) const
    {
        std::ostringstream strm;
        auto mapRequest = RenderMap();
        mapRequest.Render(strm);
        const int id = map_request.at("id").AsInt();

        auto result = json::Builder{};
        result.StartDict();
        result.Key("request_id").Value(id)
            .Key("map").Value(strm.str());

        result.EndDict();
        return result.Build();
    }

    const json::Node RequestHandler::PrintRoute(const json::Dict& route_request) const {
        json::Node result;
        const int id = route_request.at("id").AsInt();
        const std::string_view stop_from = route_request.at("from").AsString();
        const std::string_view stop_to = route_request.at("to").AsString();
        const auto& routing = router_.FindRoute(stop_from, stop_to);

        if (!routing) {
            result = json::Builder{}
                .StartDict()
                .Key("request_id").Value(id)
                .Key("error_message").Value((std::string)"not found")
                .EndDict()
                .Build();
        }
        else {
            json::Array items;
            double total_time = 0.0;
            items.reserve(routing.value().edges.size());
            for (auto& edge_id : routing.value().edges) {
                const graph::Edge<double> edge = router_.GetGraph().GetEdge(edge_id);
                if (edge.quality == 0) {
                    items.emplace_back(json::Node(json::Builder{}
                        .StartDict()
                        .Key("stop_name").Value(edge.name)
                        .Key("time").Value(edge.weight)
                        .Key("type").Value((std::string)"Wait")
                        .EndDict()
                        .Build()));

                    total_time += edge.weight;
                }
                else {
                    items.emplace_back(json::Node(json::Builder{}
                        .StartDict()
                        .Key("bus").Value(edge.name)
                        .Key("span_count").Value(static_cast<int>(edge.quality))
                        .Key("time").Value(edge.weight)
                        .Key("type").Value((std::string)"Bus")
                        .EndDict()
                        .Build()));

                    total_time += edge.weight;
                }
            }

            result = json::Builder{}
                .StartDict()
                .Key("request_id").Value(id)
                .Key("total_time").Value(total_time)
                .Key("items").Value(items)
                .EndDict()
                .Build();
        }

        return result;
    }

}