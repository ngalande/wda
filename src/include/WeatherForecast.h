#ifndef WEATHER_FORECAST_H
#define WEATHER_FORECAST_H

#include <string>
#include <vector>
#include <iostream>
#include <curl/curl.h>
#include <json/json.h>
#include <fstream>

using namespace std;

struct HourlyWeather {
    

    string defaultHourlyURL = "https://api.open-meteo.com/v1/forecast?latitude=40.7143&longitude=-74.006&current=relative_humidity_2m&hourly=temperature_2m,precipitation,cloud_cover,wind_speed_10m,temperature_1000hPa";

    string baseHourlyURL = "https://api.open-meteo.com/v1/forecast?";
    
    string withOverFortyVarsURL = "https://api.open-meteo.com/v1/forecast?latitude=40.7143&longitude=-74.006&hourly=temperature_2m,relative_humidity_2m,dew_point_2m,apparent_temperature,precipitation_probability,precipitation,rain,showers,snowfall,snow_depth,weather_code,pressure_msl,surface_pressure,cloud_cover,cloud_cover_low,cloud_cover_mid,cloud_cover_high,visibility,evapotranspiration,et0_fao_evapotranspiration,vapour_pressure_deficit,wind_speed_10m,wind_speed_80m,wind_speed_120m,wind_speed_180m,wind_direction_10m,wind_direction_80m,wind_direction_120m,wind_direction_180m,wind_gusts_10m,temperature_80m,temperature_120m,temperature_180m,soil_temperature_0cm,soil_temperature_6cm,soil_temperature_18cm,soil_temperature_54cm,soil_moisture_0_to_1cm,soil_moisture_1_to_3cm,soil_moisture_3_to_9cm,temperature_1000hPa";

};

struct DailyWeather {

    string defaultDailyURL = "https://api.open-meteo.com/v1/forecast?latitude=40.7143&longitude=-74.006&daily=temperature_2m_max,temperature_2m_min,sunrise,sunset,rain_sum,wind_speed_10m_max";

    string withSolarRadiationURL = "https://api.open-meteo.com/v1/forecast?latitude=40.7143&longitude=-74.006&hourly=direct_radiation&daily=temperature_2m_max,temperature_2m_min,sunrise,sunset,rain_sum,wind_speed_10m_max";

    string withpresureURL = "https://api.open-meteo.com/v1/forecast?latitude=40.7143&longitude=-74.006&hourly=temperature_1000hPa&daily=temperature_2m_max,temperature_2m_min,sunrise,sunset,rain_sum,wind_speed_10m_max";

    string withWeatherModelsURL = "https://api.open-meteo.com/v1/forecast?latitude=40.7143&longitude=-74.006&daily=temperature_2m_max,temperature_2m_min,sunrise,sunset,rain_sum,wind_speed_10m_max&models=best_match,icon_seamless";
    
};


    

class WeatherDataManager {

private: 
    vector<HourlyWeather> hourlyData;
    vector<DailyWeather> dailyData;

public: 
    // Private member functions
    static size_t curlWriteCallback(void* contents, size_t size, size_t nmemb, 	string* output) {
        size_t totalSize = size * nmemb;
        output->append((char*)contents, totalSize);
        return totalSize;
    }
    
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, string* data) {
        data->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
    void exportCSV(const string& filename, const string& data);
    void exportFunc(const string& data);

    // // Constructor
    // WeatherDataManager();

    // // Destructor
    // ~WeatherDataManager();

    // Public methods
    void getHourlyWeather(const std::string& mode);
    void getHourlyWeatherWithOverFortyVars(const std::string& mode);
    void getDailyWeather(const std::string& mode);
    void getDailyWeatherWithSolarRadiation(const std::string& mode);
    void getDailyWeatherWithPressure(const std::string& mode);
    void getDailyWeatherWithWeatherModels(const std::string& mode);
    void displayHourlyWeather(const std::string& mode);
    void displayDailyWeather(const std::string& mode);
};

#endif // WEATHER_FORECAST_H
