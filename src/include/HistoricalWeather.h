#ifndef HISTORICALWEATHER_H
#define HISTORICALWEATHER_H


#include <string>
#include <vector>
#include <iostream>
#include <curl/curl.h>
#include <json/json.h>
#include <fstream>

using namespace std;

struct HourlyHistoricalWeather {
    

    string defaultHourlyURL = "https://archive-api.open-meteo.com/v1/archive?latitude=52.52&longitude=13.41&start_date=2024-02-27&end_date=2024-03-12&hourly=temperature_2m,precipitation,surface_pressure,cloud_cover,wind_speed_10m";

    string baseHourlyURL = "https://archive-api.open-meteo.com/v1/archive?";
    
    string withOverFortyVarsURL = "https://archive-api.open-meteo.com/v1/archive?latitude=52.52&longitude=13.41&start_date=2024-02-27&end_date=2024-03-12&hourly=temperature_2m,relative_humidity_2m,dew_point_2m,apparent_temperature,precipitation,rain,snowfall,snow_depth,weather_code,pressure_msl,surface_pressure,cloud_cover,cloud_cover_low,cloud_cover_mid,cloud_cover_high,et0_fao_evapotranspiration,vapour_pressure_deficit,wind_speed_10m,wind_speed_100m,wind_direction_10m,wind_direction_100m,wind_gusts_10m,soil_temperature_0_to_7cm,soil_temperature_7_to_28cm,soil_temperature_28_to_100cm,soil_temperature_100_to_255cm,soil_moisture_0_to_7cm,soil_moisture_7_to_28cm,soil_moisture_28_to_100cm,soil_moisture_100_to_255cm";

};

struct DailyHistoricalWeather {

    string defaultDailyURL = "https://archive-api.open-meteo.com/v1/archive?latitude=52.52&longitude=13.41&start_date=2024-02-27&end_date=2024-03-12&daily=temperature_2m_mean,sunrise,sunset,daylight_duration,precipitation_sum,rain_sum,snowfall_sum,wind_speed_10m_max,shortwave_radiation_sum";

    string withSolarRadiationURL = "https://archive-api.open-meteo.com/v1/archive?latitude=52.52&longitude=13.41&start_date=2024-02-27&end_date=2024-03-12&hourly=direct_radiation,terrestrial_radiation&daily=temperature_2m_mean,sunrise,sunset,precipitation_sum,rain_sum";

    string withAdditionalVarsURL = "https://archive-api.open-meteo.com/v1/archive?latitude=52.52&longitude=13.41&start_date=2024-02-27&end_date=2024-03-12&hourly=is_day,sunshine_duration&daily=temperature_2m_mean,sunrise,sunset,rain_sum";

    string withReanalysisModelsURL = "https://archive-api.open-meteo.com/v1/archive?latitude=52.52&longitude=13.41&start_date=2024-02-27&end_date=2024-03-12&daily=temperature_2m_mean,sunrise,sunset,rain_sum&models=best_match,ecmwf_ifs";
    
};


    

class HistoricalWeatherDataManager {

private: 
    vector<HourlyHistoricalWeather> hourlyData;
    vector<DailyHistoricalWeather> dailyData;

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

    // Constructor
    // HistoricalWeatherDataManager();

    // // Destructor
    // ~HistoricalWeatherDataManager();

    // Public methods
    void getHourlyHistoricalWeather(const std::string& mode);
    void getHourlyHistoricalWeatherWithOverFortyVars(const std::string& mode);
    void getDailyHistoricalWeather(const std::string& mode);
    void getDailyHistoricalWeatherWithSolarRadiation(const std::string& mode);
    void getDailyHistoricalWeatherWithAdditionalVars(const std::string& mode);
    void getDailyHistoricalWeatherWithReanalysisModels(const std::string& mode);
    void displayHourlyHistoricalWeather(const std::string& mode);
    void displayDailyHistoricalWeather(const std::string& mode);
};

#endif // HISTORICALWEATHER_H
