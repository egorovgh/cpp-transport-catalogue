#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    transport_catalogue::TransportCatalogue catalogue;

    transport_catalogue::input_handler::InputReader reader;
    
    reader.ReadInput(cin);
    reader.ApplyCommands(catalogue);

    
    transport_catalogue::stat_reader::ReadAndProcessRequests(cin, catalogue, cout);
}
