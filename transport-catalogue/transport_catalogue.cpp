#include "transport_catalogue.h"
#include "geo.h"
#include <cassert>
#include <stdexcept>
#include <unordered_set>


namespace transport_catalogue
{
    void TransportCatalogue::AddBus(const std::string& route_name, const std::vector<std::string_view>& stops_, bool is_circle_)
    {
        Bus bus;
        bus.name = route_name;
        for (const auto& stop_name : stops_) {
            bus.stops.push_back(FindStop(stop_name));
        }
        bus.is_circle = is_circle_;
        AddBus(std::move(bus));
    }

    void TransportCatalogue::AddBus(Bus&& bus) noexcept
    {
        buses_.push_back(std::move(bus));
        std::string_view route_name = buses_.back().name;
        buses_by_names_.insert({ route_name, &buses_.back() });

        for (auto stop : buses_.back().stops) {
            buses_on_stops_[stop->name].insert(route_name);
        }
    }

    int TransportCatalogue::CalculateUniqueStops(const std::vector<const Stop*>& stops_) const
    {
        std::unordered_set<std::string_view> unique_stops;
        for (const auto& stop : stops_)
        {
            unique_stops.insert(stop->name);
        }

        return static_cast<int> (unique_stops.size());
    }

    void TransportCatalogue::AddStop(const std::string& stop_name, geo::Coordinates coordinate)
    {
        Stop stop;
        stop.name = stop_name;
        stop.coods = coordinate;
        AddStop(std::move(stop));
    }

    void TransportCatalogue::AddStop(Stop&& stop) noexcept {
        stops_.push_back(std::move(stop));
        stops_by_name_.insert({ stops_.back().name, &stops_.back() });
    }


    const BusInfo TransportCatalogue::GetBusInfo(const std::string_view& bus_name) const
    {
        BusInfo result;
        auto bus = FindBus(bus_name);
        if (bus->is_circle)
        {
            result.numStops = bus->stops.size();
        }
        else
        {
            result.numStops = bus->stops.size() * 2 - 1;
        }

        result.name = bus->name;
        result.numUniqueStops = CalculateUniqueStops(bus->stops);

        int routeLength = 0;
        double geographicLength = 0.0;

        for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
            auto from = bus->stops[i];
            auto to = bus->stops[i + 1];

            if (bus->is_circle) {
                routeLength += GetDistance(from, to);
                geographicLength += geo::ComputeDistance(from->coods,
                    to->coods);
            }
            else {
                routeLength += GetDistance(from, to) + GetDistance(to, from);
                geographicLength += geo::ComputeDistance(from->coods,
                    to->coods) * 2;
            }
        }


        result.routeLength = routeLength;
        result.curvature = routeLength / geographicLength;
        return result;
    }

    const Stop* TransportCatalogue::FindStop(const std::string_view& stop_name) const noexcept
    {
        if (stops_by_name_.count(stop_name) == 0) {
            return nullptr;
        }
        return stops_by_name_.at(stop_name);
    }

    const Bus* TransportCatalogue::FindBus(const std::string_view& bus_name) const noexcept
    {
        if (buses_by_names_.count(bus_name) == 0) {
            return nullptr;
        }
        auto result = buses_by_names_.at(bus_name);
        return result;
    }

    const std::set<std::string_view> TransportCatalogue::GetBusesOnStop(const Stop& stop) const
    {
        try
        {
            return buses_on_stops_.at(stop.name);
        }
        catch (const std::exception& e)
        {
            return {};
        }
    }

    int TransportCatalogue::GetDistance(const Stop* stop1, const Stop* stop2) const noexcept
    {
        int result = 0;
        if (distances_.count({ stop1, stop2 }) != 0) {
            result = distances_.at({ stop1, stop2 });
            return result;
        }
        if (distances_.count({ stop2, stop1 }) != 0) {
            result = distances_.at({ stop2, stop1 });
            return result;
        }

        return result;
    }

    void TransportCatalogue::AddStopDistance(const std::string& stop_from, const std::string& stop_to, const int& distance)
    {
        const Stop* stop_from_ = FindStop(stop_from);
        const Stop* stop_to_ = FindStop(stop_to);

        if (stop_from_ && stop_to_)
        {
            distances_[{stop_from_, stop_to_}] = distance;
        }
    }

    const std::map<std::string_view, const Bus*> TransportCatalogue::GetSortedBuses() const {
        std::map<std::string_view, const Bus*> result;
        for (const auto& bus : buses_by_names_)
        {
            result.insert(bus);
        }
        return result;
    }
    const std::map<std::string_view, const Stop*> TransportCatalogue::GetSortedStops() const
    {
        std::map<std::string_view, const Stop*> result;
        for (const auto& stop : stops_by_name_)
        {
            result.insert(stop);
        }
        return result;
    }
}

