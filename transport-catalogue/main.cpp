#include "json_reader.h"
#include "request_handler.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include <iostream>

int main() {
    /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массива "stat_requests", построив JSON-массив
     * с ответами Вывести в stdout ответы в виде JSON
     */
    transport_catalogue::TransportCatalogue catalogue;
    json_reader::JsonReader reader(catalogue, std::cin);

    reader.LoadDataToCatalogue();


    const auto& render_settings = reader.GetRenderSettings();
    const auto& renderer = reader.LoadRenderSettings(render_settings);

    const auto& routing_settings = reader.GetRoutingSettings();
    const auto& router_settings = reader.LoadRoutingSettings(routing_settings);
    const transport_catalogue::Router router = { router_settings, catalogue };

    request_handler::RequestHandler request_handler(catalogue, renderer, reader, router);

}