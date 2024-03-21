#include <matplot/matplot.h>
#include "./include/Utility.h"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <cmath>
#include <unordered_map>

using namespace std;

// Utility.cpp

bool Utility::isValidCoordinates(double latitude, double longitude) {
    // Implement code to validate latitude and longitude values
    // Example: Check if latitude is within the range [-90, 90] and longitude is within the range [-180, 180]
    return true; // Or false, depending on your validation logic
}

bool Utility::isValidTimestamp(const string& timestamp) {
    // Implement code to validate timestamp format
    // Example: Check if timestamp follows a specific format (e.g., YYYY-MM-DD HH:MM:SS)
    return true; // Or false, depending on your validation logic
}

string Utility::convertTimestamp(const string& timestamp, const string& format) {
    // Implement code to convert timestamp to a specific format
    // Example: Convert timestamp from one format to another using stringstream or strftime function
    return ""; // Return the converted timestamp or an empty string if conversion fails
}

// Implement splitString function
vector<string> Utility::splitString(const string& str, char delimiter) {
    // Implement code to split a string by a delimiter
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Implement additional utility functions as per requirements

void exportCSV(const string& filename, const string& data) {
    ofstream outfile(filename);
    if (!outfile.is_open()) {
        cerr << "Error: Unable to open file " << filename << " for writing." << endl;
        return;
    }
    outfile << data; // Assuming data is already formatted as CSV
    outfile.close();
    cout << "Data exported to " << filename << " successfully." << endl;
}

void Utility::exportToChart() {
    using namespace matplot;

    // Sample data
    std::vector<std::string> date = {"2024-03-16", "2024-03-17", "2024-03-18", "2024-03-19", "2024-03-20", "2024-03-21", "2024-03-22"};
    std::vector<double> max_temperature = {14.9, 13.4, 9.2, 6.9, 7.9, 7.0, 5.0};
    std::vector<double> min_temperature = {7.8, 7.1, 0.9, 0.9, 1.6, -1.6, -1.6};
    std::vector<double> rain_sum = {0.00, 0.00, 0.00, 0.00, 0.58, 0.00, 0.00};
    std::vector<double> wind_speed_max = {23.5, 35.3, 28.8, 28.1, 19.8, 32.2, 22.2};

    // Convert date strings to numerical values
    std::unordered_map<std::string, double> date_mapping;
    for (size_t i = 0; i < date.size(); ++i) {
        date_mapping[date[i]] = static_cast<double>(i + 1);
    }

    std::vector<double> x;
    for (const auto &d : date) {
        x.push_back(date_mapping[d]);
    }

    //Return Mongo Instance
    

    // Create a new figure
    auto f = figure();

    // Plot max temperature
    subplot(2, 2, 0);
    plot(x, max_temperature);
    title("Max Temperature");
    xlabel("Date");
    xticklabels(date);

    // Plot min temperature
    subplot(2, 2, 1);
    plot(x, min_temperature);
    title("Min Temperature");
    xlabel("Date");
    xticklabels(date);

    // Plot rain sum
    subplot(2, 2, 2);
    plot(x, rain_sum);
    title("Rain Sum");
    xlabel("Date");
    xticklabels(date);

    // Plot wind speed max
    subplot(2, 2, 3);
    plot(x, wind_speed_max);
    title("Wind Speed Max");
    xlabel("Date");
    xticklabels(date);

    // Show the plot
    show();

}

