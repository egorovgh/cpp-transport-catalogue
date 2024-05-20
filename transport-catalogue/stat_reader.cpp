#include "stat_reader.h"
#include "iostream"
#include <iomanip>


namespace transport_catalogue
{
    namespace output_system
    {
        void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output)
        {
            auto request_temp = ParseRequest(request);
            if (request_temp.first == "Bus")
            {
                PrintBusInfo(transport_catalogue, request, output);
            }
            else if (request_temp.first == "Stop")
            {
                PrintStopInfo(transport_catalogue, request, output);
            }
        }

        std::pair<std::string, std::string> ParseRequest(std::string_view request) {
            std::size_t space_pos = request.find(' ');
            std::string type = std::string(request.substr(0, space_pos));
            std::string name = std::string(request.substr(space_pos + 1));

            return { type, name };
        }


        void PrintStopInfo(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output)
        {
            try
            {
                auto stop = transport_catalogue.FindStop(ParseRequest(request).second);
                auto busesOnStop = transport_catalogue.GetBusesOnStop(*stop);
                output << std::string(request) << ": ";
                if (busesOnStop.empty())
                {
                    output << "no buses" << std::endl;
                }
                else
                {
                    output << "buses ";
                    for (const auto& bus : busesOnStop)
                    {
                        auto bus_temp = transport_catalogue.FindBus(bus);
                        output << bus_temp->name << " ";
                    }
                    output << std::endl;
                }
            }
            catch (const std::exception& e) {
                output << std::string(request) << ": not found" << std::endl;
            }
        }

        void PrintBusInfo(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output)
        {
            try
            {
                auto bus = transport_catalogue.FindBus(ParseRequest(request).second);
                output << std::string(request) << ": "
                    << bus->numStops << " stops on route, "
                    << bus->numUniqueStops << " unique stops, "
                    //<< std::fixed << std::setprecision(6) 
                    << bus->routeLength << " route length"
                    << std::endl;
            }
            catch (const std::exception& e) {
                output << std::string(request) << ": not found" << std::endl;
            }
        }
    }  
}

