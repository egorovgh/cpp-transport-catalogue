#include "transport_catalogue.h"
#include "geo.h"
#include <cassert>
#include <stdexcept>
#include <unordered_set>

namespace transport_catalogue
{
    void TransportCatalogue::AddBus(const std::string& route_name, const std::vector<std::string_view>& stops_)
    {
        Bus bus;
        bus.name = route_name;
        bus.numStops = stops_.size();
        bus.routeLength = CalculateRouteLenght(stops_);
        bus.numUniqueStops = CalculateUniqueStops(stops_);
        for (const auto& stop_name : stops_) {
            bus.stops.push_back(FindStop(stop_name));
        }
        AddBus(std::move(bus));
    }

    void TransportCatalogue::AddBus(Bus bus) noexcept
    {
        buses_.push_back(std::move(bus));
        std::string_view route_name = buses_.back().name;
        buses_by_names_.insert({ route_name, &buses_.back() });

        for (auto stop : buses_.back().stops) {
            buses_on_stops_[stop->name].insert(route_name);
        }
    }

    int TransportCatalogue::CalculateUniqueStops(const std::vector<std::string_view>& stops_)
    {
        std::unordered_set<std::string_view> unique_stops;
        for (const auto& stop : stops_)
        {
            unique_stops.insert(stop);
        }

        return static_cast<int> (unique_stops.size());
    }

    double TransportCatalogue::CalculateRouteLenght(const std::vector<std::string_view>& stops_)
    {
        double result = 0.0;
        if (stops_.size() < 2) {
            return result;
        }

        for (size_t i = 0; i < stops_.size() - 1; ++i) {
            auto stop1 = FindStop(stops_[i]);
            auto stop2 = FindStop(stops_[i + 1]);
            result += geo::ComputeDistance(stop1->coods, stop2->coods);
        }
        return result;
    }

    void TransportCatalogue::AddStop(const std::string& stop_name, geo::Coordinates coordinate)
    {
        Stop stop;
        stop.name = stop_name;
        stop.coods = coordinate;
        AddStop(stop);
    }

    void TransportCatalogue::AddStop(Stop stop) noexcept {
        stops_.push_back(std::move(stop));
        stops_by_name_.insert({ stops_.back().name, &stops_.back() });
    }


    const Bus* TransportCatalogue::GetBusInfo(const std::string_view& bus_name) const
    {
        auto result = FindBus(bus_name);
        return result;
    }

    const Stop* TransportCatalogue::FindStop(const std::string_view& stop_name) const
    {
        if (stops_by_name_.count(stop_name) == 0) {
            throw std::out_of_range("Stop " + std::string(stop_name) + " does not exist in catalogue");
        }
        return stops_by_name_.at(stop_name);
    }

    const Bus* TransportCatalogue::FindBus(const std::string_view& bus_name) const
    {
        if (buses_by_names_.count(bus_name) == 0) {
            throw std::out_of_range("Bus " + std::string(bus_name) + " does not exist in catalogue");
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
}

