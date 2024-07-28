#pragma once

#include <iostream>
#include "transport_catalogue.h"
#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_router.h"

namespace json_reader {

    class JsonReader {
    public:
        JsonReader(transport_catalogue::TransportCatalogue& tc, std::istream& input) : transport_catalogue_(tc), document_(json::Load(input)) {}

        void LoadDataToCatalogue();
        const json::Node& GetBaseRequests() const;
        const json::Node& GetStatRequests() const;
        const json::Node& GetRenderSettings() const;
        const json::Node& GetRoutingSettings() const;
        renderer::MapRenderer LoadRenderSettings(const json::Node& request_map) const;
        transport_catalogue::Router LoadRoutingSettings(const json::Node& routing_settings) const;

    private:
        void LoadStop(const json::Dict& request_map);
        void LoadBus(const json::Dict& request_map);
        void LoadDistances();


    private:
        transport_catalogue::TransportCatalogue& transport_catalogue_;
        json::Document document_;
    };

}  // namespace json_reader