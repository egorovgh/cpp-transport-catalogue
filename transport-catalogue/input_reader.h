#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <iostream>

#include "geo.h"
#include "transport_catalogue.h"

namespace transport_catalogue {
    namespace input_handler {
        struct CommandDescription {
            // Определяет, задана ли команда (поле command непустое)
            explicit operator bool() const {
                return !command.empty();
            }

            bool operator!() const {
                return !operator bool();
            }

            std::string command;      // Название команды
            std::string id;           // id маршрута или остановки
            std::string description;  // Параметры команды
        };
        
        struct StopCommandDescription
        {
            geo::Coordinates coods;
            std::unordered_map<std::string, int> distances;
        };

        class InputReader {
        public:
            /**
             * Читает количество базовых запросов и сами запросы из стандартного ввода
             */
            void ReadInput(std::istream& input);

            /**
             * Парсит строку в структуру CommandDescription и сохраняет результат в commands_
             */
            void ParseLine(std::string_view line);

            /**
             * Наполняет данными транспортный справочник, используя команды из commands_
             */
            void ApplyCommands(TransportCatalogue& catalogue) const;

            StopCommandDescription ParseCoordinatesAndDistances(const std::string& description) const;

        private:
            std::vector<CommandDescription> commands_;
        };
    }
}
