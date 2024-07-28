#include "svg.h"

namespace svg {

    using namespace std::literals;

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();
        RenderObject(context);
        context.out << std::endl;
    }

    Circle& Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\" "sv;
        RenderAttrs(out);
        out << "/>"sv;
    }

    Polyline& Polyline::AddPoint(Point point) {
        points_.push_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<polyline points=\""sv;
        bool first = true;
        for (const auto& point : points_) {
            if (!first) {
                out << " ";
            }
            first = false;
            out << point.x << "," << point.y;
        }
        out << "\"";
        RenderAttrs(out);
        out << " />"sv;
    }

    Text& Text::SetPosition(Point pos) {
        pos_ = pos;
        return *this;
    }

    Text& Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size) {
        font_size_ = size;
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = font_family;
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_ = font_weight;
        return *this;
    }

    Text& Text::SetData(std::string data) {
        data_ = data;
        return *this;
    }

    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<text x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" "sv;
        out << "dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\" "sv;
        out << "font-size=\""sv << font_size_ << "\""sv;

        if (font_family_) {
            out << " font-family=\""sv << *font_family_ << "\""sv;
        }

        if (font_weight_) {
            out << " font-weight=\""sv << *font_weight_ << "\""sv;
        }

        RenderAttrs(out);

        out << ">"sv;
        out << data_;
        out << "</text>"sv;
    }

    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        objects_.emplace_back(std::move(obj));
    }

    void Document::Render(std::ostream& out) const {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
        for (const auto& obj : objects_) {
            obj->Render({ out, 2, 2 });
        }
        out << "</svg>"sv;
    }

    std::string TagStrokeLineCap(StrokeLineCap line_cap) {
        std::string result;
        switch (line_cap) {
        case StrokeLineCap::BUTT:
            result = "butt"s;
            break;
        case StrokeLineCap::ROUND:
            result = "round"s;
            break;
        case StrokeLineCap::SQUARE:
            result = "square"s;
            break;
        }
        return result;
    }

    std::string TagStrokeLineJoin(StrokeLineJoin line_join) {
        std::string result;
        switch (line_join) {
        case StrokeLineJoin::ARCS:
            result = "arcs"s;
            break;
        case StrokeLineJoin::BEVEL:
            result = "bevel"s;
            break;
        case StrokeLineJoin::MITER:
            result = "miter"s;
            break;
        case StrokeLineJoin::MITER_CLIP:
            result = "miter-clip"s;
            break;
        case StrokeLineJoin::ROUND:
            result = "round"s;
            break;
        }
        return result;
    }

    std::ostream& operator<<(std::ostream& out, Color color) {
        std::visit(OstreamColorPrinter{ out }, color);
        return out;
    }

    void OstreamColorPrinter::operator()(std::monostate) const {
        out << "none"sv;
    }
    void OstreamColorPrinter::operator()(std::string color) const {
        out << color;
    }
    void OstreamColorPrinter::operator()(Rgb color) const {
        out << "rgb("sv << int(color.red) << ","sv << int(color.green)
            << ","sv << int(color.blue) << ")"sv;
    }
    void OstreamColorPrinter::operator()(Rgba color) const {
        out << "rgba("sv << int(color.red) << ","sv << int(color.green)
            << ","sv << int(color.blue) << ","sv << color.opacity << ")"sv;
    }

}  // namespace svg
