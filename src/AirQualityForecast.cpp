#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include "./include/AirQualityForecast.h"
#include "./include/Utility.h"
#include <iostream>
#include <vector>
#include <curl/curl.h>
#include <json/json.h>
#include <fstream>
// #include "./include/MongoInstance.h"



using namespace std;

//Global URls
string baseAirQualityForecastURL = "https://air-quality-api.open-meteo.com/v1/air-quality?";

void insertWeatherData(const std::string& description, const std::string& weatherData) {
    
    
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


    void AirQualityDataManager::exportCSV(const string& filename, const string& data) {
        ofstream outfile(filename);
        if (!outfile.is_open()) {
            cerr << "Error: Unable to open file " << filename << " for writing." << endl;
            return;
        }
        outfile << data; // Assuming data is already formatted as CSV
        outfile.close();
        cout << "Data exported to " << filename << " as CSV successfully." << endl;
    }

    void AirQualityDataManager::exportFunc(const string& data) {
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

    void AirQualityDataManager::getHourlyAirQualityForecast(const string& mode) {
        string url = "https://air-quality-api.open-meteo.com/v1/air-quality?latitude=52.52&longitude=13.41&hourly=pm10,pm2_5,carbon_monoxide,nitrogen_dioxide,sulphur_dioxide,ozone,aerosol_optical_depth,dust,uv_index,uv_index_clear_sky,ammonia,alder_pollen,birch_pollen,grass_pollen,mugwort_pollen,olive_pollen,ragweed_pollen";

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
        cout << "Hourly Air Quality Data JSON Response:" << endl;
        cout << responseBuffer << endl;
        cout << "Hourly Air Quality Data Retrieved Successfully." << endl;

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
                            
                    insertWeatherData(description, responseBuffer);
                    break;
                }
                default:
                    cout << "Invalid Choice!" << endl;
                    break;        
        }
        }
    
        // Export chart as PNG
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

    void AirQualityDataManager::getEuropeanAirQualityIndex(const string& mode) {
        string url = "https://air-quality-api.open-meteo.com/v1/air-quality?latitude=52.52&longitude=13.41&hourly=european_aqi,european_aqi_pm2_5,european_aqi_pm10,european_aqi_nitrogen_dioxide,european_aqi_ozone,european_aqi_sulphur_dioxide";

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
                                    
                            insertWeatherData(description, readBuffer);
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

    void AirQualityDataManager::displayDailyAirQualityForecastData(const string& mode) {
        while (true) {
        cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+ Air Quality Forecast Data +-+-+-+-+-+-+-+-+-+-+-+-+-+-" << endl;
        cout << "1. Get Hourly Air Quality Weather Forecast." << endl;
        cout << "2. Get European Air Quality Index." << endl;
        cout << "3. Go to Previous Menu" << endl;
        cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;

        int choice;
        cout << "Enter your choice: " << endl;
        cin >> choice;

        
            switch (choice){
                case 1:
                    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
                    cout << "Getting Hourly Air Quality Weather Forecast..." << endl;
                    getHourlyAirQualityForecast(mode);
                    break;
                case 2:
                    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
                    cout << "Getting European Air Quality Index..." << endl;
                    getEuropeanAirQualityIndex(mode);
                    break;
                case 3:
                    cout << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
                    cout << "Returning to previous menu..." << endl;
                    return;
                default: 
                    cout << "Invalid choice! Please enter a valid choice" << endl;
                    break;
            }
        }
    }
