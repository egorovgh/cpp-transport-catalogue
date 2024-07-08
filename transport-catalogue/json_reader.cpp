#include "json_reader.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

namespace json_reader {
    void JsonReader::LoadDataToCatalogue() { 
        const json::Array& arr = GetBaseRequests().AsArray();
        for (auto& request_stops : arr) {
            const auto& request_stops_map = request_stops.AsMap();
            const auto& type = request_stops_map.at("type").AsString();
            if (type == "Stop") {
                LoadStop(request_stops_map);
            }
        }

        LoadDistances();

        for (auto& request_bus : arr) {
            const auto& request_bus_map = request_bus.AsMap();
            const auto& type = request_bus_map.at("type").AsString();
            if (type == "Bus") {
                LoadBus(request_bus_map);
            }
        }
    }

    const json::Node& JsonReader::GetBaseRequests() const
    {
        if (!document_.GetRoot().AsMap().count("base_requests"))
        {
            static json::Node nullNode(nullptr);  
            return nullNode;
        }
        return document_.GetRoot().AsMap().at("base_requests");
    }

    const json::Node& JsonReader::GetStatRequests() const
    {
        if (!document_.GetRoot().AsMap().count("stat_requests"))
        {
            static json::Node nullNode(nullptr);  
            return nullNode;
        }
        return document_.GetRoot().AsMap().at("stat_requests");
    }

    const json::Node& JsonReader::GetRenderSettings() const {
        if (!document_.GetRoot().AsMap().count("render_settings")){
            static json::Node nullNode(nullptr);  
            return nullNode;
        }
        return document_.GetRoot().AsMap().at("render_settings");
    }

    void JsonReader::LoadStop(const json::Dict& request_map)
    {
        std::string_view stop_name = request_map.at("name").AsString();
        geo::Coordinates coordinates = { request_map.at("latitude").AsDouble(), request_map.at("longitude").AsDouble() };
        transport_catalogue_.AddStop((std::string)stop_name, coordinates);
    }

    void JsonReader::LoadBus(const json::Dict& request_map)
    {
        std::string_view bus_number = request_map.at("name").AsString();
        std::vector<std::string_view> stops;
        for (auto& stop : request_map.at("stops").AsArray()) {
            stops.push_back(transport_catalogue_.FindStop(stop.AsString()) -> name);
        }
        bool is_circle = request_map.at("is_roundtrip").AsBool();

        transport_catalogue_.AddBus((std::string)bus_number, stops, is_circle);
    }

    void JsonReader::LoadDistances()
    {
        const json::Array& arr = GetBaseRequests().AsArray();
        for (auto& request_stops : arr) {
            const auto& request_stops_map = request_stops.AsMap();
            const auto& type = request_stops_map.at("type").AsString();
            if (type == "Stop") {
                std::string_view stop_name = request_stops_map.at("name").AsString();
                auto stop_temp = transport_catalogue_.FindStop(stop_name);
                auto& distances = request_stops_map.at("road_distances").AsMap();
                std::map<std::string_view, int> stop_distances;
                for (auto& [stop_name, dist] : distances) {
                    stop_distances.emplace(stop_name, dist.AsInt());
                }
                for (auto& [to_name, dist] : stop_distances) {
                    auto to = transport_catalogue_.FindStop(to_name);
                    transport_catalogue_.AddStopDistance(stop_temp -> name, to -> name, dist);
                }
            }
        }
    }


    renderer::MapRenderer JsonReader::LoadRenderSettings(const json::Dict& request_map) const {
        /*struct RenderSettings {
        double width = 0.0;
        double height = 0.0;
        double padding = 0.0;
        double line_width = 0.0;
        double stop_radius = 0.0;
        int bus_label_font_size = 0;
        svg::Point bus_label_offset = { 0.0, 0.0 };
        int stop_label_font_size = 0;
        svg::Point stop_label_offset = { 0.0, 0.0 };
        svg::Color underlayer_color = { svg::NoneColor };
        double underlayer_width = 0.0;
        std::vector<svg::Color> color_palette {};
        */

        renderer::RenderSettings render_settings;
        render_settings.width = request_map.at("width").AsDouble();
        render_settings.height = request_map.at("height").AsDouble();
        render_settings.padding = request_map.at("padding").AsDouble();
        render_settings.stop_radius = request_map.at("stop_radius").AsDouble();
        render_settings.line_width = request_map.at("line_width").AsDouble();
        render_settings.bus_label_font_size = request_map.at("bus_label_font_size").AsInt();
        const json::Array& bus_label_offset = request_map.at("bus_label_offset").AsArray();
        render_settings.bus_label_offset = { bus_label_offset[0].AsDouble(), bus_label_offset[1].AsDouble() };
        render_settings.stop_label_font_size = request_map.at("stop_label_font_size").AsInt();
        const json::Array& stop_label_offset = request_map.at("stop_label_offset").AsArray();
        render_settings.stop_label_offset = { stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble() };

        if (request_map.at("underlayer_color").IsString()) render_settings.underlayer_color = request_map.at("underlayer_color").AsString();
        else if (request_map.at("underlayer_color").IsArray()) {
            const json::Array& underlayer_color = request_map.at("underlayer_color").AsArray();
            if (underlayer_color.size() == 3) {
                render_settings.underlayer_color = svg::Rgb(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt());
            }
            else if (underlayer_color.size() == 4) {
                render_settings.underlayer_color = svg::Rgba(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt(), underlayer_color[3].AsDouble());
            } else throw std::logic_error("wrong underlayer colortype");
        } else throw std::logic_error("wrong underlayer color");

        render_settings.underlayer_width = request_map.at("underlayer_width").AsDouble();

        const json::Array& color_palette = request_map.at("color_palette").AsArray();
        for (const auto& color_element : color_palette) {
            if (color_element.IsString()) render_settings.color_palette.emplace_back(color_element.AsString());
            else if (color_element.IsArray()) {
                const json::Array& color_type = color_element.AsArray();
                if (color_type.size() == 3) {
                    render_settings.color_palette.emplace_back(svg::Rgb(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt()));
                }
                else if (color_type.size() == 4) {
                    render_settings.color_palette.emplace_back(svg::Rgba(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt(), color_type[3].AsDouble()));
                } else throw std::logic_error("wrong color_palette type");
            } else throw std::logic_error("wrong color_palette");
        }

        return render_settings;
    }
}