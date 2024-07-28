#pragma once

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */
#include "geo.h"

#include <string>
#include <vector>
#include <set>
#include <unordered_map>

namespace transport_catalogue
{
    struct Stop {
        std::string name;
        geo::Coordinates coods;
    };

    struct Bus {
        std::string name;
        std::vector<const Stop*> stops;
        bool is_circle;
    };

    struct BusInfo {
        std::string name;
        int numStops = 0;
        int numUniqueStops = 0;
        double routeLength = 0;
        double curvature = 0;
    };
}