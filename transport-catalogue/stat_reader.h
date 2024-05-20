#pragma once

#include <iosfwd>
#include <string_view>
#include <string>
#include "transport_catalogue.h"


namespace transport_catalogue
{
    namespace output_system
    {
        void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output);
        std::pair<std::string, std::string> ParseRequest(std::string_view request);
        void PrintBusInfo(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output);
        void PrintStopInfo(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output);
    }
}
