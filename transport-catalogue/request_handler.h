#pragma once

#include <set>
#include <optional>
#include <string_view>


#include "transport_catalogue.h"
#include "domain.h"
#include "map_renderer.h"
#include "json_reader.h"
#include "svg.h"


namespace request_handler
{
    class RequestHandler {
        public:
        RequestHandler(const transport_catalogue::TransportCatalogue& db, const renderer::MapRenderer& renderer, const json_reader::JsonReader reader);

        // Возвращает информацию о маршруте (запрос Bus)
        std::optional<transport_catalogue::BusInfo> GetBusStat(const std::string_view& bus_name) const;

        // Возвращает маршруты, проходящие через
        const std::set<std::string_view> GetBusesByStop(const std::string_view& stop_name) const;
        
        void PrintRequests() const;

        // Этот метод будет нужен в следующей части итогового проекта
        svg::Document RenderMap() const;

        private:
        const json::Node PrintBus(const json::Dict& bus_request) const;
        const json::Node PrintStop(const json::Dict& stop_request) const;
        const json::Node PrintMap(const json::Dict& map_request) const;
        
        private:
        const transport_catalogue::TransportCatalogue& db_;
        const renderer::MapRenderer& renderer_;
        const json_reader::JsonReader reader_;
    };
}

