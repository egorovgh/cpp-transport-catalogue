#include "input_reader.h" 

#include <algorithm> 
#include <cassert> 
#include <iterator> 
#include <iostream>
#include <cctype>  


namespace transport_catalogue
{
    namespace input_handler
    {
        /**
         * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
        */
        geo::Coordinates ParseCoordinates(std::string_view str) {
            static const double nan = std::nan("");

            auto not_space = str.find_first_not_of(' ');
            auto comma = str.find(',');

            if (comma == str.npos) {
                return { nan, nan };
            }

            auto not_space2 = str.find_first_not_of(' ', comma + 1);

            double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
            double lng = std::stod(std::string(str.substr(not_space2)));

            return { lat, lng };
        }

        /**
         * Удаляет пробелы в начале и конце строки
         */
        std::string_view Trim(std::string_view string) {
            const auto start = string.find_first_not_of(' ');
            if (start == string.npos) {
                return {};
            }
            return string.substr(start, string.find_last_not_of(' ') + 1 - start);
        }

        /**
         * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
         */
        std::vector<std::string_view> Split(std::string_view string, std::string_view delim) {
            std::vector<std::string_view> result;

            size_t pos = 0;
            while (pos < string.size()) {
                auto delim_pos = string.find(delim, pos);
                if (delim_pos == string.npos) {
                    delim_pos = string.size();
                }
                auto substr = string.substr(pos, delim_pos - pos);
                substr = Trim(substr);
                if (!substr.empty()) {
                    result.push_back(substr);
                }
                pos = delim_pos + delim.size();
            }

            return result;
        }

        /**
         * Парсит маршрут.
         * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
         * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
         */
        std::vector<std::string_view> ParseRoute(std::string_view route) {
            if (route.find('>') != route.npos) {
                return Split(route, ">");
            }

            auto stops = Split(route, "-");
            std::vector<std::string_view> results(stops.begin(), stops.end());
            results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

            return results;
        }

        CommandDescription ParseCommandDescription(std::string_view line) {
            auto colon_pos = line.find(':');
            if (colon_pos == line.npos) {
                return {};
            }

            auto space_pos = line.find(' ');
            if (space_pos >= colon_pos) {
                return {};
            }

            auto not_space = line.find_first_not_of(' ', space_pos);
            if (not_space >= colon_pos) {
                return {};
            }

            return { std::string(line.substr(0, space_pos)),
                    std::string(line.substr(not_space, colon_pos - not_space)),
                    std::string(line.substr(colon_pos + 1)) };
        }

        void InputReader::ReadInput(std::istream& input) {
            int base_request_count;
            std::cin >> base_request_count >> std::ws;

            for (int i = 0; i < base_request_count; ++i) {
                std::string line;
                std::getline(input, line);
                ParseLine(line);
            }
        }

        void InputReader::ParseLine(std::string_view line) {
            auto command_description = ParseCommandDescription(line);
            if (command_description) {
                commands_.push_back(std::move(command_description));
            }
        }

        void InputReader::ApplyCommands([[maybe_unused]] TransportCatalogue& catalogue) const {
            // Реализуйте метод самостоятельно 
            for (const CommandDescription& command : commands_)
            {
                if (!command)
                {
                    return;
                }

                if (command.command == "Stop") {
                    auto stop_info = ParseCoordinatesAndDistances(command.description);
                    catalogue.AddStop(command.id, stop_info.first);
                }           
            }

            for (const CommandDescription& command : commands_)
            {
                if (!command)
                {
                    return;
                }

                if (command.command == "Stop") {
                    auto stop_info = ParseCoordinatesAndDistances(command.description);
                    catalogue.AddStopDistances(command.id, stop_info.second);
                }
            }

            for (const CommandDescription& command : commands_)
            {
                if (!command)
                {
                    return;
                }

                if (command.command == "Bus") {
                    catalogue.AddBus(command.id, ParseRoute(command.description));
                }
            }
        }
        std::pair<geo::Coordinates, std::unordered_map<std::string, int>> InputReader::ParseCoordinatesAndDistances(const std::string& description) const
        {
            geo::Coordinates coordinates;
            std::unordered_map<std::string, int> distances;

            auto splitted_description = Split(description, ",");
            coordinates.lat = std::stod((std::string)splitted_description[0]);
            coordinates.lng = std::stod((std::string)splitted_description[1]);

            if (splitted_description.size() == 2)
            {
                return { coordinates, distances };
            }

            for (size_t i = 2; i < splitted_description.size(); i++)
            {
                auto splitted_distance = Split(splitted_description[i], "m to ");
                std::string stop = (std::string)splitted_distance[1];
                int distance = std::stoi((std::string)splitted_distance[0]);
                
                distances.insert(std::make_pair(stop, distance));
            }

            return { coordinates, distances };
        }
    }
}
