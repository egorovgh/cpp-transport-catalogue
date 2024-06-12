#include "stat_reader.h"
#include <iostream>
#include <iomanip>

namespace transport_catalogue {
    namespace stat_reader {
        void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output) {
            auto request_temp = ParseRequest(request);
            if (request_temp.type == "Bus") {
                PrintBusInfo(transport_catalogue, request, output);
            }
            else if (request_temp.type == "Stop") {
                PrintStopInfo(transport_catalogue, request, output);
            }
        }

        Request ParseRequest(std::string_view request) {
            std::size_t space_pos = request.find(' ');
            Request result;
            result.type = std::string(request.substr(0, space_pos));
            result.name = std::string(request.substr(space_pos + 1));
            return result;
        }

        void PrintStopInfo(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output) {
            auto stop = transport_catalogue.FindStop(ParseRequest(request).name);
            if (!stop) {
                output << std::string(request) << ": not found" << std::endl;
                return;
            }
            auto busesOnStop = transport_catalogue.GetBusesOnStop(*stop);
            output << std::string(request) << ": ";
            if (busesOnStop.empty()) {
                output << "no buses" << std::endl;
            }
            else {
                output << "buses ";
                for (const auto& bus : busesOnStop) {
                    output << bus << " ";
                }
                output << std::endl;
            }
        }

        void ReadAndProcessRequests(std::istream& input, const TransportCatalogue& catalogue, std::ostream& output) {
            int stat_request_count;
            std::cin >> stat_request_count >> std::ws;
            for (int i = 0; i < stat_request_count; ++i) {
                std::string line;
                std::getline(input, line);
                ParseAndPrintStat(catalogue, line, output);
            }
        }

        void PrintBusInfo(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output) {
            auto bus = transport_catalogue.FindBus(ParseRequest(request).name);
            if (!bus) {
                output << std::string(request) << ": not found" << std::endl;
                return;
            }
            auto busInfo = transport_catalogue.GetBusInfo(bus->name);
            output << std::string(request) << ": "
                << busInfo.numStops << " stops on route, "
                << busInfo.numUniqueStops << " unique stops, "
                << busInfo.routeLength << " route length, "
                << busInfo.curvature << " curvature"
                << std::endl;
        }
    }
}
