#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include <iostream>
#include <vector>
#include <string>
#include "LocationManager.h"
#include "WeatherForecast.h"
#include "HistoricalWeather.h"
#include "AirQualityForecast.h"


using namespace std;

class UIController {
private:
    WeatherDataManager weatherDataManager;
    HistoricalWeatherDataManager historicalWeatherDataManager;
    AirQualityDataManager airQualityDataManager;


public:
    // Constructor
    UIController();

    // Destructor
    ~UIController();

    // Function to display main menu
    int displayMainMenu(LocationManager& locationManager);

private:

    // Function to display the menu and get user's choice
    int displayMenuAndGetChoice(const string& mode, LocationManager& locationManager); 

    // Function to handle user input and execute corresponding actions
    void handleUserInput(const string& mode, LocationManager& locationManager);

    // Function to display the weather forecast menu
    void weatherForecastMenu(const string& mode, LocationManager& locationManager);

    // Function to display the historical weather forecast menu
    void historicalWeatherForecastMenu(const string& mode, LocationManager& locationManager);

    // Function to display the air quality forecast menu
    void airQualityForecastMenu(const string& mode, LocationManager& locationManager);

    // Function to display the extra features menu
    void getSavedData();

    //Function to display the Location Management menu
    void locationManagementMenu(const string& mode, LocationManager& locationManager);

    // Function to display the settings menu
    void settingsMenu();
};

#endif // UICONTROLLER_H
