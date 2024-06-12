#pragma once

#include <iosfwd>
#include <string_view>
#include <string>
#include "transport_catalogue.h"

namespace transport_catalogue {
    namespace stat_reader {
        struct Request {
            std::string type;
            std::string name;
        };

        void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output);
        Request ParseRequest(std::string_view request);
        void PrintBusInfo(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output);
        void PrintStopInfo(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output);
        void ReadAndProcessRequests(std::istream& input, const TransportCatalogue& catalogue, std::ostream& output);
    }
}
