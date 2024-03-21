#ifndef AIRQUALITYFORECAST_H
#define AIRQUALITYFORECAST_H

#include <string>
#include <vector>
#include <iostream>
#include <curl/curl.h>
#include <json/json.h>
#include <fstream>


using namespace std;

struct HourlyEuropeanAQI
{

    string defaultHourlyURL = "https://air-quality-api.open-meteo.com/v1/air-quality?latitude=52.52&longitude=13.41&hourly=pm10,pm2_5,carbon_monoxide,nitrogen_dioxide,sulphur_dioxide,ozone,aerosol_optical_depth,dust,uv_index,uv_index_clear_sky,ammonia,alder_pollen,birch_pollen,grass_pollen,mugwort_pollen,olive_pollen,ragweed_pollen";

    string baseHourlyURL = "https://air-quality-api.open-meteo.com/v1/air-quality?";

    string europeanAQIURL = "https://air-quality-api.open-meteo.com/v1/air-quality?latitude=52.52&longitude=13.41&hourly=european_aqi,european_aqi_pm2_5,european_aqi_pm10,european_aqi_nitrogen_dioxide,european_aqi_ozone,european_aqi_sulphur_dioxide";
};

class AirQualityDataManager
{

private:
    vector<HourlyEuropeanAQI> hourlyData;

public:
    // Private member functions
    static size_t curlWriteCallback(void *contents, size_t size, size_t nmemb, string *output)
    {
        size_t totalSize = size * nmemb;
        output->append((char *)contents, totalSize);
        return totalSize;
    }

    static size_t writeCallback(void *contents, size_t size, size_t nmemb, string *data)
    {
        data->append((char *)contents, size * nmemb);
        return size * nmemb;
    }
    void exportCSV(const string &filename, const string &data);
    void exportFunc(const string &data);

    // Constructor
    // AirQualityDataManager();

    // // Destructor
    // ~AirQualityDataManager();

    // Public methods
    void getHourlyAirQualityForecast(const std::string &mode);
    void getEuropeanAirQualityIndex(const std::string &mode);
    void displayDailyAirQualityForecastData(const std::string &mode);
};

#endif // AIRQUALITYFORECAST_H
