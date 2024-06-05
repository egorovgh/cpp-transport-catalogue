#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    transport_catalogue::TransportCatalogue catalogue;

    transport_catalogue::input_handler::InputReader reader;
    int base_request_count;
    cin >> base_request_count >> ws;
    reader.ReadInput(cin, base_request_count);
    reader.ApplyCommands(catalogue);

    int stat_request_count;
    cin >> stat_request_count >> ws;
    transport_catalogue::stat_reader::ReadAndProcessRequests(cin, stat_request_count, catalogue, cout);
}
