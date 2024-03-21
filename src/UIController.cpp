#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <chrono>
#include <ctime>
#include "./include/UIController.h"
#include "./include/LocationManager.h"
#include "./include/WeatherForecast.h"
#include "./include/HistoricalWeather.h"
#include "./include/AirQualityForecast.h"
#include <iostream>
#include <string>

using namespace std;

// Initialize mongocxx instance globally
mongocxx::instance inst{};

// Constructor
UIController::UIController()
{
    // Initialize member variables if necessary
}

// Destructor
UIController::~UIController()
{
    // Cleanup resources if necessary
}

// Function to display main menu
int UIController::displayMainMenu(LocationManager &locationManager)
{

    while (true)
    {
        int choice;
        string selectedMode;

        cout << "o+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+o" << endl;
        cout << "=== Welcome to your Weather Data App! ===" << endl;
        cout << endl;
        cout << "To Begin select a mode." << endl;
        cout << "1. Basic" << endl;
        cout << "2. Intermediate" << endl;
        cout << "3. Advanced" << endl;
        cout << "4. Expert" << endl;
        cout << "5. Exit" << endl;
        cout << "o+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+o" << endl;

        cout << "Enter Your Choice: " << endl;
        cin >> choice;

        switch (choice)
        {
        case 1:
            selectedMode = "Basic";
            break;
        case 2:
            selectedMode = "Intermediate";
            break;
        case 3:
            selectedMode = "Advanced";
            break;
        case 4:
            selectedMode = "Expert";
            break;
        case 5:
            cout << "Exiting.... Goodbye!" << endl;
            exit(0);
        default:
            cout << "Invalid choice. Please Select a valid choice..." << endl;
        }

        // Call corresponding menu based on the selected mode
        handleUserInput(selectedMode, locationManager);
    }

    return 0;
}

// Function to handle user input and execute corresponding actions
void UIController::handleUserInput(const string &mode, LocationManager &locationManager)
{

    while (true)
    {
        int choice = displayMenuAndGetChoice(mode, locationManager);

        switch (choice)
        {
        case 1:
            locationManagementMenu(mode, locationManager);
            break;
        case 2:
            weatherForecastMenu(mode, locationManager);
            break;
        case 3:
            if (mode != "Basic")
            {
                historicalWeatherForecastMenu(mode, locationManager);
            }
            break;
        case 4:
            if (mode != "Basic" && mode != "Intermediate")
            {
                airQualityForecastMenu(mode, locationManager);
            }
            break;
        case 5:
            if (mode != "Basic" && mode != "Intermediate" && mode != "Advanced")
            {
                getSavedData();
            }
            break;
        case 6:
            cout << "Returning to Main Menu..." << endl;
            displayMainMenu(locationManager);
            break;
        default:
            cout << "Invalid choice. Please enter a number between 1 and 6." << endl;
            break;
        }
    }
}

// Function to display the menu and get user's choice
int UIController::displayMenuAndGetChoice(const string &mode, LocationManager &locationManager)
{
    // Display menu based on the selected mode
    cout << "o+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+o" << endl;
    cout << "You have selected " << mode << " mode." << endl;
    cout << "What would you like to do?" << endl;
    cout << "1. Location Management" << endl;
    cout << "2. Weather Forecast" << endl;

    if (mode != "Basic")
    {
        cout << "3. Historical Weather Forecast" << endl;
        if (mode != "Intermediate")
        {
            cout << "4. Air Quality Forecast" << endl;

            if (mode != "Advanced")
            {
                cout << "5. See data saved in cloud database (MongoDB)" << endl;
            }
        }
    }
    cout << "6. Go back to main menu" << endl;

    int choice;
    cout << "Enter your choice: ";
    cin >> choice;
    cout << "o+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+o" << endl;
    cout << endl;

    return choice;
}

// Function to display the weather forecast menu
void UIController::weatherForecastMenu(const string &mode, LocationManager &locationManager)
{
    // Implement code to display the weather forecast menu
    WeatherDataManager weatherForecastManager;

    int choice;

    cout << "o+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+o" << endl;
    cout << "Weather Forecast Menu:" << endl;
    cout << "1. Hourly Weather forecast" << endl;
    cout << "2. Display Daily Weather Forecast" << endl;
    cout << "3. Return to Previous Menu" << endl;
    cout << "Enter your choice: ";
    cin >> choice;
    cout << "o+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+o" << endl;

    switch (choice)
    {
    case 1:
        weatherForecastManager.displayHourlyWeather(mode);
        break;
    case 2:
        weatherForecastManager.displayDailyWeather(mode);
        break;
    case 3:
        cout << "Returning to Previous Menu..." << endl;
        break;
        // Return from the function to go back to the main menu
    default:
        cout << "Invalid choice. Please enter a number between 1 and 3." << endl;
        break;
    }
}

// Function to display the historical weather forecast menu
void UIController::historicalWeatherForecastMenu(const string &mode, LocationManager &locationManager)
{
    // Implement code to display the historical weather forecast menu
    HistoricalWeatherDataManager historicalWeatherManager;

    cout << "o+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+o" << endl;
    cout << "Historical Weather Menu:" << endl;
    cout << "1. Display Hourly historical weather" << endl;
    cout << "2. Display Daily historical weather" << endl;
    cout << "3. Return to previous menu" << endl;

    cout << "o+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+o" << endl;
    cout << "Enter your choice: ";
    int choice;
    cin >> choice;
    cout << endl;

    switch (choice)
    {
    case 1:
        historicalWeatherManager.displayHourlyHistoricalWeather(mode);
        break;
    case 2:
        historicalWeatherManager.displayDailyHistoricalWeather(mode);
        break;
    case 3:
        // Return to previous menu
        cout << "Returning to previous menu..." << endl;
        break;
    default:
        cout << "Invalid choice. Please enter a valid option." << endl;
        break;
    }
}

// Function to display the air quality forecast menu
void UIController::airQualityForecastMenu(const string &mode, LocationManager &locationManager)
{
    // Implement code to display the air quality forecast menu
    AirQualityDataManager airQualityDataManager;

    int choice;

    cout << "o+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+o" << endl;
    cout << "Air Quality Forecast Menu:" << endl;
    cout << "1. Display Air Quality Data" << endl;
    cout << "2. Return to previous menu" << endl;
    cout << "Enter your choice: ";
    cin >> choice;
    cout << "o+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+o" << endl;

    switch (choice)
    {
    case 1:
        airQualityDataManager.displayDailyAirQualityForecastData(mode);
        break;
    case 2:
        cout << "Returning to previous menu..." << endl;
        break;
    default:
        cout << "Invalid choice. Please enter a number between 1 and 3." << endl;
    }
}

// Function to display the location management menu
void UIController::locationManagementMenu(const string &mode, LocationManager &locationManager)
{

    int choice;
    cout << "o+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+o" << endl;
    cout << "Location Management Menu:" << endl;
    cout << "1. View Locations" << endl;
    cout << "2. Add Location" << endl;
    cout << "3. Search for a Location" << endl;
    cout << "4. Remove Location" << endl;
    if (mode != "Basic"){
        cout << "5. Set favorite Locations" << endl;
    }
    cout << "6. Back to previous menu" << endl;
    cout << "Enter your choice: ";
    cin >> choice;
    cout << "o+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+o" << endl;

    switch (choice)
    {
    case 1:
    {
        cout << "Locations:" << endl;
        const vector<Location> &locations = locationManager.getLocations();
        for (int i = 0; i < locations.size(); ++i)
        {
            cout << i + 1 << ". " << locations[i].name << ", Latitude: " << locations[i].latitude << ", Longitude: " << locations[i].longitude << ", Fvaorite: " << (locations[i].favorite ? "Yes" : "No") << endl;
        }
        break;
    }
    case 2:
    {
        string locationName;
        double latitude, longitude;
        cout << "Enter location name: ";
        cin.ignore(); // Clear input buffer
        getline(cin, locationName);
        cout << "Enter latitude: ";
        cin >> latitude;
        cout << "Enter longitude: ";
        cin >> longitude;
        locationManager.addLocation(locationName, latitude, longitude);
        cout << "Location added successfully." << endl;
        break;
    }
    case 3:
    {
        string query;
        cout << "Enter a Location name to search for: " << endl;
        cin.ignore(); // Clear input buffer
        std::getline(std::cin, query);
        locationManager.searchLocations(query);
        break;
    }

    case 4:
    {
        int index;
        cout << "Enter index of location to remove: ";
        cin >> index;
        if (locationManager.removeLocation(index))
        {
            cout << "Location removed successfully." << endl;
        }
        else
        {
            cout << "Invalid index. Please try again." << endl;
        }
        break;
    }
    case 5:
    {
        int ID;
        bool favorite;
        int choice;
        cout << "Enter location ID you want to set as favorite: "<< endl;
        cin >> ID;
        cout << endl;
        cout << "Set location with ID " << ID << " as favorite?" << "Enter 1 for YES and 0 for No:" << endl;
        cin >> choice;
        
        switch (choice ){
            case 1: 
                favorite = true;
                locationManager.setFavorite(ID, favorite);
                break;
            case 0:
                cout <<"Operation canceled. Return...." << endl;
                break;
            default:
                cout << "Invalid choice. Returning..." << endl;
                break;

        }
    }
    case 6:
        cout << "Returning to previous menu..." << endl;
        break;
    default:
        cout << "Invalid choice. Please try again." << endl;
    }
}

// Function to get saved weather data from DB
void UIController::getSavedData()
{
    cout << endl;
    cout << "Fetching your saved data. Please wait..." << endl;
    cout << "---------------------------------" << endl;

    try
    {
        // Connect to MongoDB
        mongocxx::client client{mongocxx::uri{"mongodb+srv://root:Chimwemwe@cluster0.bhxezut.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0"}};

        // Access WDA database and weather-collection collection
        auto collection = client["WDA"]["weather-collection"];

        // Perform a query to retrieve IDs and descriptions
        auto cursor = collection.find({});

        // Iterate over the cursor and print IDs and descriptions
        for (auto &&doc : cursor)
        {
            std::cout << "ID: " << doc["_id"].get_oid().value.to_string() << std::endl;

            std::string description_str = std::string(doc["description"].get_utf8().value);
            std::cout << "Description: " << description_str << std::endl;

            auto timestamp = doc["timestamp"].get_date();
            auto timestamp_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::time_point(timestamp));
            std::cout << "Timestamp: " << std::ctime(&timestamp_time_t); // Convert time_t to string

            std::cout << "---------------------------------" << std::endl;
        }

        int choice;
        cout << endl;
        cout << "Would you like to see the data in detail? Enter 1 for YES and 0 for NO." << endl;
        cin >> choice;

        switch (choice)
        {
        case 0:
            cout << endl;
            cout << "You have choosen to NOT view the data in detail. Returning..." << endl;
            break;
        case 1:
        {
            string ID;
            cout << endl;
            cout << "Enter the corresponding ID for the data you want." << endl;
            cin >> ID;

            cout << endl;
            cout << "Getting detailed data for " << ID << ". Please wait..." << endl;
            // Find document by ID
            auto filter = bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("_id", bsoncxx::oid{ID})); // Correcting the usage of make_document
            auto result = collection.find_one(filter.view());

            if (result)
            {
                // Convert weather data to JSON string and display
                std::string weather_data_str = bsoncxx::to_json(result->view()["weatherData"].get_document().view());
                std::cout << "Weather Data: " << weather_data_str << std::endl;
            }
            else
            {
                std::cout << "No weather data found for the provided ID." << std::endl;
            }
            break;
        }
        default:
            cout << "Invalid choice returning..." << endl;
            break;
        }
    }
    catch (const std::exception &e)
    {
        // Catch other standard exceptions
        cerr << "Exception: " << e.what() << endl;
    }
    catch (...)
    {
        // Catch any other unhandled exceptions
        cerr << "Unknown Exception occurred." << endl;
    }
}
// Function to display the settings menu
void UIController::settingsMenu()
{
    // Implement code to display the settings menu
}
