#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include "./include/main.h"
#include "./include/LocationManager.h"

int main() {
    UIController uiController;
    LocationManager locationManager;

    // // Initialize mongocxx instance globally
    // mongocxx::instance inst{};


    //Add base loction
    locationManager.addLocation("Lusaka", 52.52, 13.41);

    // Main application loop
    bool running = true;
    while (running) {
        // Display main menu and handle user input
        uiController.displayMainMenu(locationManager);
    }

    return 0;
}
