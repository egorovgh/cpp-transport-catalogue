#pragma once
#include <string>
#include <deque>
#include <set>
#include <unordered_map>
#include <vector>
#include <map>

#include "geo.h"
#include "domain.h"



namespace transport_catalogue
{
    

    struct StopPairHasher {
        size_t operator()(const std::pair<const Stop*, const Stop*>& pair) const {
            return std::hash<const void*>{}(pair.first) ^ std::hash<const void*>{}(pair.second);
        }
    };


    class TransportCatalogue {
    public:
        void AddStop(const std::string& stop_name, geo::Coordinates coordinate);
        void AddBus(const std::string& route_name, const std::vector<std::string_view>& stops, const bool& is_circle_);
        void AddStopDistance(const std::string& stop_from, const std::string& stop_to, const int& distance);
        const BusInfo GetBusInfo(const std::string_view& bus_name) const;
        const Stop* FindStop(const std::string_view& stop_name) const noexcept;
        const Bus* FindBus(const std::string_view& bus_name) const noexcept;
        const std::set<std::string_view> GetBusesOnStop(const Stop& stop) const;
        int GetDistance(const Stop* stop1, const Stop* stop2) const noexcept;
        int CalculateUniqueStops(const std::vector<const Stop*>& stops_) const;
        const std::map<std::string_view, const Bus*> GetSortedBuses() const;

    private:
        void AddStop(Stop&& stop) noexcept;
        void AddBus(Bus&& bus) noexcept;
        std::pair<double, double> CalculateGeoAndRealRouteLength(const std::vector<const Stop*>& stops_) const;


    private:
        std::deque<Stop> stops_;
        std::deque<Bus> buses_;
        std::unordered_map<std::string_view, const Stop*> stops_by_name_;
        std::unordered_map<std::string_view, const Bus*> buses_by_names_;
        std::unordered_map<std::string_view, std::set<std::string_view>> buses_on_stops_;
        std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopPairHasher> distances_;
    };

}

