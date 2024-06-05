#pragma once
#include <string>
#include <deque>
#include <set>
#include <unordered_map>
#include <vector>
#include "geo.h"


namespace transport_catalogue
{
    struct Stop {
        std::string name;
        geo::Coordinates coods;
    };

    struct Bus {
        std::string name;
        std::vector<const Stop*> stops;

    };

    struct BusInfo
    {
        std::string name;
        int numStops = 0;
        int numUniqueStops = 0;
        double routeLength = 0;
    };


    class TransportCatalogue {
    public:
        void AddStop(const std::string& stop_name, geo::Coordinates coordinate);
        void AddBus(const std::string& route_name, const std::vector<std::string_view>& stops);
        const BusInfo GetBusInfo(const std::string_view& bus_name) const;
        const Stop* FindStop(const std::string_view& stop_name) const noexcept;
        const Bus* FindBus(const std::string_view& bus_name) const noexcept;
        const std::set<std::string_view> GetBusesOnStop(const Stop& stop) const;

    private:
        void AddStop(Stop&& stop) noexcept;
        void AddBus(Bus&& bus) noexcept;
        int CalculateUniqueStops(const std::vector<const Stop*>& stops_) const;
        double CalculateRouteLength(const std::vector<const Stop*>& stops_) const;


    private:
        std::deque<Stop> stops_;
        std::deque<Bus> buses_;
        std::unordered_map<std::string_view, const Stop*> stops_by_name_;
        std::unordered_map<std::string_view, const Bus*> buses_by_names_;
        std::unordered_map<std::string_view, std::set<std::string_view>> buses_on_stops_;
    };

}

