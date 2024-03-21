#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include "./include/WeatherForecast.h"
#include "./include/Utility.h"
// #include "./include/MongoInstance.h"
#include <iostream>
#include <vector>
#include <curl/curl.h>
#include <json/json.h>
#include <fstream>

using namespace std;

//Global URls
string baseWeatherForecastURL = "https://api.open-meteo.com/v1/forecast?";
// string csvDownloadParam = "&format=csv";
// string xlsxDownloadParam = "&format=xlsx";

void insertWeatherDataWF(const std::string& description, const std::string& weatherData) {
    
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

    void WeatherDataManager::exportCSV(const string& filename, const string& data) {
        ofstream outfile(filename);
        if (!outfile.is_open()) {
            cerr << "Error: Unable to open file " << filename << " for writing." << endl;
            return;
        }
        outfile << data; // Assuming data is already formatted as CSV
        outfile.close();
        cout << "Data exported to " << filename << " as CSV successfully." << endl;
    }

    void WeatherDataManager::exportFunc(const string& data) {
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


    void WeatherDataManager::getHourlyWeather(const string& mode) {
        string url = "https://api.open-meteo.com/v1/forecast?latitude=40.7143&longitude=-74.006&daily=temperature_2m_max,temperature_2m_min,sunrise,sunset,rain_sum,wind_speed_10m_max";

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
                                    
                            insertWeatherDataWF(description, responseBuffer);
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

    void WeatherDataManager::getHourlyWeatherWithOverFortyVars(const string& mode) {
        string url = "https://api.open-meteo.com/v1/forecast?latitude=40.7143&longitude=-74.006&hourly=temperature_2m,relative_humidity_2m,dew_point_2m,apparent_temperature,precipitation_probability,precipitation,rain,showers,snowfall,snow_depth,weather_code,pressure_msl,surface_pressure,cloud_cover,cloud_cover_low,cloud_cover_mid,cloud_cover_high,visibility,evapotranspiration,et0_fao_evapotranspiration,vapour_pressure_deficit,wind_speed_10m,wind_speed_80m,wind_speed_120m,wind_speed_180m,wind_direction_10m,wind_direction_80m,wind_direction_120m,wind_direction_180m,wind_gusts_10m,temperature_80m,temperature_120m,temperature_180m,soil_temperature_0cm,soil_temperature_6cm,soil_temperature_18cm,soil_temperature_54cm,soil_moisture_0_to_1cm,soil_moisture_1_to_3cm,soil_moisture_3_to_9cm,temperature_1000hPa";

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
                                    
                            insertWeatherDataWF(description, readBuffer);
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

    void WeatherDataManager::getDailyWeather(const string& mode) {
        string url = "https://api.open-meteo.com/v1/forecast?latitude=40.7143&longitude=-74.006&daily=temperature_2m_max,temperature_2m_min,sunrise,sunset,rain_sum,wind_speed_10m_max";

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
                                    
                            insertWeatherDataWF(description, readBuffer);
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

    void WeatherDataManager::getDailyWeatherWithSolarRadiation(const string& mode) {
        string url = "https://api.open-meteo.com/v1/forecast?latitude=40.7143&longitude=-74.006&hourly=direct_radiation&daily=temperature_2m_max,temperature_2m_min,sunrise,sunset,rain_sum,wind_speed_10m_max";

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
                                    
                            insertWeatherDataWF(description, response);
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

    void WeatherDataManager::getDailyWeatherWithPressure(const string& mode) {
        string url = "https://api.open-meteo.com/v1/forecast?latitude=40.7143&longitude=-74.006&hourly=temperature_1000hPa&daily=temperature_2m_max,temperature_2m_min,sunrise,sunset,rain_sum,wind_speed_10m_max";

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
                                    
                            insertWeatherDataWF(description, response);
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

    void WeatherDataManager::getDailyWeatherWithWeatherModels(const string& mode) {
        string url = "https://api.open-meteo.com/v1/forecast?latitude=40.7143&longitude=-74.006&daily=temperature_2m_max,temperature_2m_min,sunrise,sunset,rain_sum,wind_speed_10m_max&models=best_match,icon_seamless";

        CURL* curl;
        CURLcode res;
        string response;

        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WeatherDataManager::writeCallback); // Use the static member function
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
                                    
                            insertWeatherDataWF(description, response);
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

    void WeatherDataManager::displayHourlyWeather(const string& mode) {
        while (true) {
        cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+ Hourly Weather Data +-+-+-+-+-+-+-+-+-+-+-+-+-+-" << endl;
        cout << "Select action: " << endl;
        cout << "1. Get Hourly Weather Forecast." << endl;
        cout << "2. Get Hourly Weather Forecast with more than 40 vars." << endl;
        cout << "3. Go to Previous Menu" << endl;
        cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
        
        int choice;
        cout << "Enter your choice: " << endl;
        cin >> choice ;

        
            switch (choice){
                case 1:
                    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
                    cout << "Fetching hourly temperature, percipitation, pressure, wind, humidity and cloudiness..." << endl;
                    getHourlyWeather(mode);
                    break;
                case 2:
                    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
                    cout << "Fetching hourly weather data with over 40 variables..." << endl;
                    getHourlyWeatherWithOverFortyVars(mode);
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

    void WeatherDataManager::displayDailyWeather(const string& mode) {
        while (true) {
        cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+ Daily Weather Data +-+-+-+-+-+-+-+-+-+-+-+-+-+-" << endl;
        cout << "1. Get Daily Weather Forecast." << endl;
        cout << "2. Get forecast with solar radiation variables." << endl;
        cout << "3. Get forecast with pressure variables." << endl;
        cout << "4. Get Daily Weather Forecast with the Best Match and DWD ICON SEAMLESS weather models." << endl;
        cout << "5. Go to Previous Menu" << endl;
        cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;

        int choice;
        cout << "Enter your choice: " << endl;
        cin >> choice;

        
            switch (choice){
                case 1:
                    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
                    getDailyWeather(mode);
                    break;
                case 2:
                    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
                    getDailyWeatherWithSolarRadiation(mode);
                    break;
                case 3:
                    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
                    getDailyWeatherWithPressure(mode);
                    break;
                case 4:
                    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
                    getDailyWeatherWithWeatherModels(mode);
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

