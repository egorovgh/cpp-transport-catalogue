#include "map_renderer.h"

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */

namespace renderer {
    bool IsZero(double value) {
        return std::abs(value) < EPSILON;
    }

    std::vector<svg::Polyline> MapRenderer::GetBusPolylines(const std::map<std::string_view, const transport_catalogue::Bus*>& buses, const SphereProjector& sphereProjector) const {
        std::vector<svg::Polyline> result;
        size_t colorNumber = 0;
        for (const auto& bus : buses) {
            if (bus.second->stops.empty())
            {
                continue;
            }

            std::vector<const transport_catalogue::Stop*> bus_stops = bus.second->stops;
            if (!bus.second->is_circle)
            {
                bus_stops.insert(bus_stops.end(), std::next(bus.second->stops.rbegin()), bus.second->stops.rend());
            }

            svg::Polyline line;
            for (const auto& stop : bus_stops) {
                line.AddPoint(sphereProjector(stop->coods));
            }
            line.SetStrokeColor(render_settings_.color_palette[colorNumber]);
            line.SetFillColor("none");
            line.SetStrokeWidth(render_settings_.line_width);
            line.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

            if (colorNumber < (render_settings_.color_palette.size() - 1))
            {
                colorNumber++;
            }
            else {
                colorNumber = 0;
            }

            result.push_back(line);
        }

        return result;
    }

    std::vector<svg::Text> MapRenderer::GetBusNameText(const std::map<std::string_view, const transport_catalogue::Bus*>& buses,
        const SphereProjector& sphereProjector) const {
        std::vector<svg::Text> result;
        size_t colorNumber = 0;

        for (const auto& bus : buses)
        {
            if (bus.second->stops.empty())
            {
                continue;
            }

            svg::Text nameText;
            nameText.SetFillColor(render_settings_.color_palette[colorNumber]);
            nameText.SetPosition(sphereProjector(bus.second->stops[0]->coods));
            nameText.SetData((std::string)bus.first);
            nameText.SetOffset(render_settings_.bus_label_offset);
            nameText.SetFontFamily("Verdana");
            nameText.SetFontSize(render_settings_.bus_label_font_size);
            nameText.SetFontWeight("bold");


            if (colorNumber < (render_settings_.color_palette.size() - 1)) {
                colorNumber++;
            }
            else {
                colorNumber = 0;
            }


            svg::Text underlayerText;
            underlayerText.SetFontFamily("Verdana");
            underlayerText.SetFontSize(render_settings_.bus_label_font_size);
            underlayerText.SetFontWeight("bold");
            underlayerText.SetData((std::string)bus.first);
            underlayerText.SetPosition(sphereProjector(bus.second->stops[0]->coods));
            underlayerText.SetOffset(render_settings_.bus_label_offset);
            underlayerText.SetFillColor(render_settings_.underlayer_color);
            underlayerText.SetStrokeColor(render_settings_.underlayer_color);
            underlayerText.SetStrokeWidth(render_settings_.underlayer_width);
            underlayerText.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            underlayerText.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

            result.push_back(underlayerText);
            result.push_back(nameText);

            if (!bus.second->is_circle && bus.second->stops[0] != bus.second->stops.back())
            {
                svg::Text nameText_copy(nameText);
                svg::Text underlayerText_copy(underlayerText);

                nameText_copy.SetPosition(sphereProjector(bus.second->stops.back()->coods));
                underlayerText_copy.SetPosition(sphereProjector(bus.second->stops.back()->coods));
                result.push_back(underlayerText_copy);
                result.push_back(nameText_copy);
            }
        }

        return result;
    }


    std::vector<svg::Circle> MapRenderer::GetStopsCircle(const std::map<std::string_view, const transport_catalogue::Stop*>& stops,
        const SphereProjector& sphereProjector) const {
        std::vector<svg::Circle> result;

        for (const auto& stop : stops)
        {
            svg::Circle circle;
            circle.SetCenter(sphereProjector(stop.second->coods));
            circle.SetRadius(render_settings_.stop_radius);
            circle.SetFillColor("white");
            result.push_back(circle);
        }

        return result;
    }


    std::vector<svg::Text> MapRenderer::GetStopNamesText(const std::map<std::string_view, const transport_catalogue::Stop*>& stops,
        const SphereProjector& sphereProjector) const {
        std::vector<svg::Text> result;

        for (const auto& stop : stops)
        {
            svg::Text nameText;
            nameText.SetPosition(sphereProjector(stop.second->coods));
            nameText.SetData((std::string)stop.first);
            nameText.SetOffset(render_settings_.stop_label_offset);
            nameText.SetFontFamily("Verdana");
            nameText.SetFontSize(render_settings_.stop_label_font_size);
            //nameText.SetFontWeight("bold");
            nameText.SetFillColor("black");



            svg::Text underlayerText;
            underlayerText.SetFontFamily("Verdana");
            underlayerText.SetFontSize(render_settings_.stop_label_font_size);
            //underlayerText.SetFontWeight("bold");
            underlayerText.SetData((std::string)stop.first);
            underlayerText.SetPosition(sphereProjector(stop.second->coods));
            underlayerText.SetOffset(render_settings_.stop_label_offset);
            underlayerText.SetFillColor(render_settings_.underlayer_color);
            underlayerText.SetStrokeColor(render_settings_.underlayer_color);
            underlayerText.SetStrokeWidth(render_settings_.underlayer_width);
            underlayerText.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
            underlayerText.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

            result.push_back(underlayerText);
            result.push_back(nameText);
        }

        return result;
    }

    svg::Document MapRenderer::GetSVGDocument(const std::map<std::string_view,
        const transport_catalogue::Bus*>& buses) const {
        svg::Document result;
        std::vector<geo::Coordinates> bus_stopsCoods;
        std::map<std::string_view, const transport_catalogue::Stop*> stops_;
        for (const auto& bus : buses) {
            for (const auto& stop : bus.second->stops) {
                bus_stopsCoods.push_back(stop->coods);
                stops_[stop->name] = stop;
            }
        }
        SphereProjector sphereProjector(bus_stopsCoods.begin(), bus_stopsCoods.end(), render_settings_.width, render_settings_.height, render_settings_.padding);
        for (const auto& line : GetBusPolylines(buses, sphereProjector)) {
            result.Add(line);
        }

        for (const auto& busText : GetBusNameText(buses, sphereProjector))
        {
            result.Add(busText);
        }

        for (const auto& stopCircle : GetStopsCircle(stops_, sphereProjector))
        {
            result.Add(stopCircle);
        }

        for (const auto& stopText : GetStopNamesText(stops_, sphereProjector))
        {
            result.Add(stopText);
        }

        return result;
    }
}