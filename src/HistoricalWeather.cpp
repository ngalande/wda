#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include "./include/HistoricalWeather.h"
// #include "./include/MongoInstance.h"
#include "./include/Utility.h"
#include <iostream>
#include <vector>
#include <curl/curl.h>
#include <json/json.h>
#include <fstream>

using namespace std;

//Global URls
string baseHistoricalWeatherForecastURL = "https://archive-api.open-meteo.com/v1/archive?";
// string csvDownloadParam = "&format=csv";
// string xlsxDownloadParam = "&format=xlsx";

void insertWeatherDataHW(const std::string& description, const std::string& weatherData) {
    
    // Connect to MongoDB
    mongocxx::client client{mongocxx::uri{"mongodb+srv://root:Chimwemwe@cluster0.bhxezut.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0"}};
    
    // Access WDA database and weather-collection collection
    auto collection = client["WDA"]["weather-collection"];
    
    // Create a BSON document for insertion
    bsoncxx::builder::stream::document doc{};
    doc << "description" << description
        << "weatherData" << bsoncxx::from_json(weatherData)
        << "timestamp" << bsoncxx::types::b_date(std::chrono::system_clock::now());
    
    cout << "Saving data to Mongo DB...."<< endl;
    
    // Insert the document into the collection
    auto result = collection.insert_one(doc.view());
    
    // Check if insertion was successful
    if (result) {
        std::cout << "Weather data stored to MongoDB successfully! You can now view it in saved data section." << std::endl;
    } else {
        std::cerr << "Error: Failed to save Weather Data." << std::endl;
    }
}


// Helper function to handle libcurl response
    static size_t curlWriteCallback(void* contents, size_t size, size_t nmemb, 	string* output) {
        size_t totalSize = size * nmemb;
        output->append((char*)contents, totalSize);
        return totalSize;
    }

    // Static member function to handle libcurl response
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, string* data) {
        data->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    void HistoricalWeatherDataManager::exportCSV(const string& filename, const string& data) {
        ofstream outfile(filename);
        if (!outfile.is_open()) {
            cerr << "Error: Unable to open file " << filename << " for writing." << endl;
            return;
        }
        outfile << data; // Assuming data is already formatted as CSV
        outfile.close();
        cout << "Data exported to " << filename << " as CSV successfully." << endl;
    }

    void HistoricalWeatherDataManager::exportFunc(const string& data) {
         string filename;
                    cout << "Enter a file name for your files: " << endl;;
                    cin >> filename;
                    string filename2 = filename;
                    filename2 = filename2.append(".csv");
                    ofstream jsonFile(filename.append(".json"));
                    jsonFile << data;
                    jsonFile.close();
                    cout << "Hourly weather Data exported to '"<< filename <<"'successfully!" << endl;

                    //Export to CSV
                    exportCSV(filename2, data);
    }


    void HistoricalWeatherDataManager::getHourlyHistoricalWeather(const string& mode) {
        string url = "https://archive-api.open-meteo.com/v1/archive?latitude=52.52&longitude=13.41&start_date=2024-02-27&end_date=2024-03-12&hourly=temperature_2m,precipitation,surface_pressure,cloud_cover,wind_speed_10m";

        // Initialize libcurl
        CURL* curl = curl_easy_init();
        if (!curl) {
            cerr << "Error initializing libcurl." << endl;
            return;
        }

        // Set the URL to fetch
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set up a buffer to store the response data
        string responseBuffer;

        // Set the write callback function to store the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);

        // Perform the HTTP GET request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "Failed to perform HTTP request: " << curl_easy_strerror(res) << endl;
            curl_easy_cleanup(curl);
            return;
        }

        // Cleanup libcurl
        curl_easy_cleanup(curl);

        // Print the JSON response
        cout << "Hourly Weather Data JSON Response:" << endl;
        cout << responseBuffer << endl;
        cout << "Hourly Weather Data Retrieved Successfully." << endl;

        if (mode != "Basic") {
            int choice;
            cout << "Would you like to export the data as a JSON file and CSV?" << endl;
            cout << "Enter 1 for YES and 0 for No:  ";
            cin >> choice; 
            cout << endl;

            switch (choice){
                case 0: 
                    cout << "You have choosen not to export the data. Returning..." << endl;
                    break;
                case 1: {
                    //Export to JSON
                    exportFunc(responseBuffer);
                    break;
                    }
                default:
                    cout << "Invalid Choice!" << endl;
                    break;
            }
        }

        //Save Data to DB menu
        if (mode == "Expert"){
                int choice;
            cout << endl;
            cout << "Would you like to save this data your Database?" << endl;
            cout << "Enter 1 for YES and 0 for No: ";
            cin >> choice;
            cout << endl;

            switch(choice){
                case 0: 
                    cout << "You have choosen not to save the data. Returning..." << endl;
                    break;
                case 1:{
                    string description;
                    cout << "Enter a decription for the data: " << endl;
                    cin >> description ;
                    cout << endl;
                            
                    insertWeatherDataHW(description, responseBuffer);
                    break;
                }
                default:
                    cout << "Invalid Choice!" << endl;
                    break;        
            }
        }

        if (mode == "Expert"){
                int choice;
                cout << endl;
                cout << "Would you like to save the data as a chart?" << endl;
                cout << "Enter 1 for YES and 0 for NO: " << endl;
                cin >> choice;

                switch (choice)
                {
                case 0:
                    cout << endl;
                    cout << "You have choosen to to export data as a PNG. Returning..." << endl;
                    break;
                case 1:
                {
                    cout << endl;
                    cout << "Exporting weather data as chart. Please wait..." << endl;
                    try
                    {
                        Utility util;
                        util.exportToChart();
                        cout << "Chart Created successfully! You can now save it with a name of your choice." << endl;
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
                default:
                    cout << endl;
                    cout << "Invalid choice. Returning..." << endl;
                    break;
                }
            }
        
    }

    void HistoricalWeatherDataManager::getHourlyHistoricalWeatherWithOverFortyVars(const string& mode) {
        string url = "https://archive-api.open-meteo.com/v1/archive?latitude=52.52&longitude=13.41&start_date=2024-02-27&end_date=2024-03-12&hourly=temperature_2m,relative_humidity_2m,dew_point_2m,apparent_temperature,precipitation,rain,snowfall,snow_depth,weather_code,pressure_msl,surface_pressure,cloud_cover,cloud_cover_low,cloud_cover_mid,cloud_cover_high,et0_fao_evapotranspiration,vapour_pressure_deficit,wind_speed_10m,wind_speed_100m,wind_direction_10m,wind_direction_100m,wind_gusts_10m,soil_temperature_0_to_7cm,soil_temperature_7_to_28cm,soil_temperature_28_to_100cm,soil_temperature_100_to_255cm,soil_moisture_0_to_7cm,soil_moisture_7_to_28cm,soil_moisture_28_to_100cm,soil_moisture_100_to_255cm";

        CURL *curl;
        CURLcode res;
        string readBuffer;

        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);

            if (res == CURLE_OK) {
                // Print out returned JSON
                cout << "JSON Response:" << endl;
                cout << readBuffer << endl;

                if (mode != "Basic") {
                    int choice;
                    cout << "Would you like to export the data as a JSON file and CSV?" << endl;
                    cout << "Enter 1 for YES and 0 for No:";
                    cin >> choice;
                    cout << endl;

                    switch (choice){
                        case 0: 
                            cout << "You have choosen not to export the data. Returning..." << endl;
                            break;
                        case 1: {
                            exportFunc(readBuffer);
                            break;
                            }
                        default:
                            cout << "Invalid Choice!" << endl;
                            break;
                    }
                }
                //Save Data to DB menu
                if (mode == "Expert"){
                        int choice;
                    cout << endl;
                    cout << "Would you like to save this data your Database?" << endl;
                    cout << "Enter 1 for YES and 0 for No: ";
                    cin >> choice;
                    cout << endl;

                    switch(choice){
                        case 0: 
                            cout << "You have choosen not to save the data. Returning..." << endl;
                            break;
                        case 1:{
                            string description;
                            cout << "Enter a decription for the data: " << endl;
                            cin >> description ;
                            cout << endl;
                                    
                            insertWeatherDataHW(description, readBuffer);
                            break;
                        }
                        default:
                            cout << "Invalid Choice!" << endl;
                            break;        
                    }
                }

                if (mode == "Expert"){
                int choice;
                cout << endl;
                cout << "Would you like to save the data as a chart?" << endl;
                cout << "Enter 1 for YES and 0 for NO: " << endl;
                cin >> choice;

                switch (choice)
                {
                case 0:
                    cout << endl;
                    cout << "You have choosen to to export data as a PNG. Returning..." << endl;
                    break;
                case 1:
                {
                    cout << endl;
                    cout << "Exporting weather data as chart. Please wait..." << endl;
                    try
                    {
                        Utility util;
                        util.exportToChart();
                        cout << "Chart Created successfully! You can now save it with a name of your choice." << endl;
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
                default:
                    cout << endl;
                    cout << "Invalid choice. Returning..." << endl;
                    break;
                }
            }
        

            } else {
                cout << "Failed to fetch data from the server." << endl;
            }
        } else {
            cout << "Failed to initialize libcurl." << endl;
        }
    }

    void HistoricalWeatherDataManager::getDailyHistoricalWeather(const string& mode) {
        string url = "https://archive-api.open-meteo.com/v1/archive?latitude=52.52&longitude=13.41&start_date=2024-02-27&end_date=2024-03-12&daily=temperature_2m_mean,sunrise,sunset,daylight_duration,precipitation_sum,rain_sum,snowfall_sum,wind_speed_10m_max,shortwave_radiation_sum";

        CURL *curl;
        CURLcode res;
        string readBuffer;

        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);

            if (res == CURLE_OK) {
                // Print out returned JSON
                cout << "JSON Response:" << endl;
                cout << readBuffer << endl;

                if (mode != "Basic") {
                    int choice;
                    cout << "Would you like to export the data as a JSON file and CSV?" << endl;
                    cout << "Enter 1 for YES and 0 for No:";
                    cin >> choice;
                    cout << endl;

                    switch (choice){
                        case 0: 
                            cout << "You have choosen not to export the data. Returning..." << endl;
                            break;
                        case 1: {
                            //Export to JSON
                            exportFunc(readBuffer);
                            break;
                            }
                        default:
                            cout << "Invalid Choice!" << endl;
                            break;
                    }
                }
                //Save Data to DB menu
                if (mode == "Expert"){
                        int choice;
                    cout << endl;
                    cout << "Would you like to save this data your Database?" << endl;
                    cout << "Enter 1 for YES and 0 for No: ";
                    cin >> choice;
                    cout << endl;

                    switch(choice){
                        case 0: 
                            cout << "You have choosen not to save the data. Returning..." << endl;
                            break;
                        case 1:{
                            string description;
                            cout << "Enter a decription for the data: " << endl;
                            cin >> description ;
                            cout << endl;
                                    
                            insertWeatherDataHW(description, readBuffer);
                            break;
                        }
                        default:
                            cout << "Invalid Choice!" << endl;
                            break;        
                }
                }

                if (mode == "Expert"){
                int choice;
                cout << endl;
                cout << "Would you like to save the data as a chart?" << endl;
                cout << "Enter 1 for YES and 0 for NO: " << endl;
                cin >> choice;

                switch (choice)
                {
                case 0:
                    cout << endl;
                    cout << "You have choosen to to export data as a PNG. Returning..." << endl;
                    break;
                case 1:
                {
                    cout << endl;
                    cout << "Exporting weather data as chart. Please wait..." << endl;
                    try
                    {
                        Utility util;
                        util.exportToChart();
                        cout << "Chart Created successfully! You can now save it with a name of your choice." << endl;
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
                default:
                    cout << endl;
                    cout << "Invalid choice. Returning..." << endl;
                    break;
                }
            }
        

            } else {
                cout << "Failed to fetch data from the server." << endl;
            }
        } else {
            cout << "Failed to initialize libcurl." << endl;
        }
    }

    void HistoricalWeatherDataManager::getDailyHistoricalWeatherWithSolarRadiation(const string& mode) {
        string url = "https://archive-api.open-meteo.com/v1/archive?latitude=52.52&longitude=13.41&start_date=2024-02-27&end_date=2024-03-12&hourly=direct_radiation,terrestrial_radiation&daily=temperature_2m_mean,sunrise,sunset,precipitation_sum,rain_sum";

        CURL* curl = curl_easy_init();
        string response;

        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            CURLcode res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);

            if (res == CURLE_OK) {
                // Print json response to user
                cout << "JSON Response:" << endl;
                cout << response << endl;

                if (mode != "Basic") {
                    int choice;
                    cout << "Would you like to export the data as a JSON file and CSV?" << endl;
                    cout << "Enter 1 for YES and 0 for No:";
                    cin >> choice;
                    cout << endl;

                    switch (choice){
                        case 0: 
                            cout << "You have choosen not to export the data. Returning..." << endl;
                            break;
                        case 1: {
                            exportFunc(response);
                            break;
                            }
                        default:
                            cout << "Invalid Choice!" << endl;
                            break;
                    }
                }
                //Save Data to DB menu
                if (mode == "Expert"){
                        int choice;
                    cout << endl;
                    cout << "Would you like to save this data your Database?" << endl;
                    cout << "Enter 1 for YES and 0 for No: ";
                    cin >> choice;
                    cout << endl;

                    switch(choice){
                        case 0: 
                            cout << "You have choosen not to save the data. Returning..." << endl;
                            break;
                        case 1:{
                            string description;
                            cout << "Enter a decription for the data: " << endl;
                            cin >> description ;
                            cout << endl;
                                    
                            insertWeatherDataHW(description, response);
                            break;
                        }
                        default:
                            cout << "Invalid Choice!" << endl;
                            break;        
                }
                }

                if (mode == "Expert"){
                int choice;
                cout << endl;
                cout << "Would you like to save the data as a chart?" << endl;
                cout << "Enter 1 for YES and 0 for NO: " << endl;
                cin >> choice;

                switch (choice)
                {
                case 0:
                    cout << endl;
                    cout << "You have choosen to to export data as a PNG. Returning..." << endl;
                    break;
                case 1:
                {
                    cout << endl;
                    cout << "Exporting weather data as chart. Please wait..." << endl;
                    try
                    {
                        Utility util;
                        util.exportToChart();
                        cout << "Chart Created successfully! You can now save it with a name of your choice." << endl;
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
                default:
                    cout << endl;
                    cout << "Invalid choice. Returning..." << endl;
                    break;
                }
            }
        

            } else {
                cerr << "Failed to make HTTP request. Error code: " << res << endl;
            }
        } else {
            cerr << "Failed to initialize libcurl." << endl;
        }
    }

    void HistoricalWeatherDataManager::getDailyHistoricalWeatherWithAdditionalVars(const string& mode) {
        string url = "https://archive-api.open-meteo.com/v1/archive?latitude=52.52&longitude=13.41&start_date=2024-02-27&end_date=2024-03-12&hourly=is_day,sunshine_duration&daily=temperature_2m_mean,sunrise,sunset,rain_sum";

        CURL* curl = curl_easy_init();
        string response;

        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            CURLcode res = curl_easy_perform(curl);

            if (res == CURLE_OK) {
                // Print json response to user
                cout << "JSON Response:\n" << response << endl;
                
                if (mode != "Basic") {
                    int choice;
                    cout << "Would you like to export the data as a JSON file and CSV?" << endl;
                    cout << "Enter 1 for YES and 0 for No:";
                    cin >> choice;
                    cout << endl;

                    switch (choice){
                        case 0: 
                            cout << "You have choosen not to export the data. Returning..." << endl;
                            break;
                        case 1: {
                            //Export to JSON
                            exportFunc(response);
                            break;
                            }
                        default:
                            cout << "Invalid Choice!" << endl;
                            break;
                    }
                }
                //Save Data to DB menu
                if (mode == "Expert"){
                        int choice;
                    cout << endl;
                    cout << "Would you like to save this data your Database?" << endl;
                    cout << "Enter 1 for YES and 0 for No: ";
                    cin >> choice;
                    cout << endl;

                    switch(choice){
                        case 0: 
                            cout << "You have choosen not to save the data. Returning..." << endl;
                            break;
                        case 1:{
                            string description;
                            cout << "Enter a decription for the data: " << endl;
                            cin >> description ;
                            cout << endl;
                                    
                            insertWeatherDataHW(description, response);
                            break;
                        }
                        default:
                            cout << "Invalid Choice!" << endl;
                            break;        
                }
                }

                if (mode == "Expert"){
                int choice;
                cout << endl;
                cout << "Would you like to save the data as a chart?" << endl;
                cout << "Enter 1 for YES and 0 for NO: " << endl;
                cin >> choice;

                switch (choice)
                {
                case 0:
                    cout << endl;
                    cout << "You have choosen to to export data as a PNG. Returning..." << endl;
                    break;
                case 1:
                {
                    cout << endl;
                    cout << "Exporting weather data as chart. Please wait..." << endl;
                    try
                    {
                        Utility util;
                        util.exportToChart();
                        cout << "Chart Created successfully! You can now save it with a name of your choice." << endl;
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
                default:
                    cout << endl;
                    cout << "Invalid choice. Returning..." << endl;
                    break;
                }
            }
        

                // Close libcurl
                curl_easy_cleanup(curl);
            } else {
                cerr << "Failed to fetch data: " << curl_easy_strerror(res) << endl;
            }
        }
    }

    void HistoricalWeatherDataManager::getDailyHistoricalWeatherWithReanalysisModels(const string& mode) {
        string url = "https://archive-api.open-meteo.com/v1/archive?latitude=52.52&longitude=13.41&start_date=2024-02-27&end_date=2024-03-12&daily=temperature_2m_mean,sunrise,sunset,rain_sum&models=best_match,ecmwf_ifs";

        CURL* curl;
        CURLcode res;
        string response;

        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HistoricalWeatherDataManager::writeCallback); // Use the static member function
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            res = curl_easy_perform(curl);
            if (res == CURLE_OK) {
                // Print JSON response
                cout << "JSON Response:" << endl;
                cout << response << endl; // Output the entire JSON response

                if (mode != "Basic") {
                    int choice;
                    cout << "Would you like to export the data as a JSON file and CSV?" << endl;
                    cout << "Enter 1 for YES and 0 for No:";
                    cin >> choice;
                    cout << endl;

                    switch (choice){
                        case 0: 
                            cout << "You have choosen not to export the data. Returning..." << endl;
                            break;
                        case 1: {
                            exportFunc(response);
                            break;
                            }
                        default:
                            cout << "Invalid Choice!" << endl;
                            break;
                    }
                }
                
                //Save Data to DB menu
                if (mode == "Expert"){
                        int choice;
                    cout << endl;
                    cout << "Would you like to save this data your Database?" << endl;
                    cout << "Enter 1 for YES and 0 for No: ";
                    cin >> choice;
                    cout << endl;

                    switch(choice){
                        case 0: 
                            cout << "You have choosen not to save the data. Returning..." << endl;
                            break;
                        case 1:{
                            string description;
                            cout << "Enter a decription for the data: " << endl;
                            cin >> description ;
                            cout << endl;
                                    
                            insertWeatherDataHW(description, response);
                            break;
                        }
                        default:
                            cout << "Invalid Choice!" << endl;
                            break;        
                }
                }

                if (mode == "Expert"){
                int choice;
                cout << endl;
                cout << "Would you like to save the data as a chart?" << endl;
                cout << "Enter 1 for YES and 0 for NO: " << endl;
                cin >> choice;

                switch (choice)
                {
                case 0:
                    cout << endl;
                    cout << "You have choosen to to export data as a PNG. Returning..." << endl;
                    break;
                case 1:
                {
                    cout << endl;
                    cout << "Exporting weather data as chart. Please wait..." << endl;
                    try
                    {
                        Utility util;
                        util.exportToChart();
                        cout << "Chart Created successfully! You can now save it with a name of your choice." << endl;
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
                default:
                    cout << endl;
                    cout << "Invalid choice. Returning..." << endl;
                    break;
                }
            }
        

            } else {
                cerr << "Failed to perform HTTP request: " << curl_easy_strerror(res) << endl;
            }

            curl_easy_cleanup(curl);
        } else {
            cerr << "Failed to initialize libcurl" << endl;
        }

        curl_global_cleanup();
    }

    void HistoricalWeatherDataManager::displayHourlyHistoricalWeather(const string& mode) {
        while (true) {
        cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+ Hourly Historical Weather Data +-+-+-+-+-+-+-+-+-+-+-+-+-+-" << endl;
        cout << "Select action: " << endl;
        cout << "1. Get Hourly Historical Weather Forecast." << endl;
        cout << "2. Get Hourly Historical Weather Forecast with more than 40 vars." << endl;
        cout << "3. Go to Previous Menu" << endl;
        cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
        
        int choice;
        cout << "Enter your choice: " << endl;
        cin >> choice ;

        
            switch (choice){
                case 1:
                    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
                    cout << "Fetching hourly Historical temperature, percipitation, pressure, wind, humidity and cloudiness..." << endl;
                    getHourlyHistoricalWeather(mode);
                    break;
                case 2:
                    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
                    cout << "Fetching hourly Historical weather data with over 40 variables..." << endl;
                    getHourlyHistoricalWeatherWithOverFortyVars(mode);
                    break;
                case 3:
                    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
                    cout << "Returning to previous menu..." << endl;
                    return;

                default: 
                    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
                    cout << "Invalid choice! Please enter a valid choice" << endl;
                break;
            }
        }
        
    }

    void HistoricalWeatherDataManager::displayDailyHistoricalWeather(const string& mode) {
        while (true) {
        cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+ Daily Historical Weather Data +-+-+-+-+-+-+-+-+-+-+-+-+-+-" << endl;
        cout << "1. Get Daily Historical Weather Forecast." << endl;
        cout << "2. Get Historical forecast with solar radiation variables." << endl;
        cout << "3. Get Historical forecast with additional variables (day/night)." << endl;
        cout << "4. Get Daily Historical Weather Forecast with the Best Match and ECMWF IFS Reanalysis models." << endl;
        cout << "5. Go to Previous Menu" << endl;
        cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;

        int choice;
        cout << "Enter your choice: " << endl;
        cin >> choice;

        
            switch (choice){
                case 1:
                    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
                    getDailyHistoricalWeather(mode);
                    break;
                case 2:
                    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
                    getDailyHistoricalWeatherWithSolarRadiation(mode);
                    break;
                case 3:
                    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
                    getDailyHistoricalWeatherWithAdditionalVars(mode);
                    break;
                case 4:
                    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
                    getDailyHistoricalWeatherWithReanalysisModels(mode);
                    break;
                case 5:
                    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
                    cout << "Returning to previous menu..." << endl;
                    return;
                default: 
                    cout << "Invalid choice! Please enter a valid choice" << endl;
                    break;
            }
        }
    }
