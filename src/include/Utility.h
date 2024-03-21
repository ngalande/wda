#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <vector>

class Utility {
public:
    // Function to validate latitude and longitude values
    static bool isValidCoordinates(double latitude, double longitude);

    // Function to validate timestamp format
    static bool isValidTimestamp(const std::string& timestamp);

    // Function to convert timestamp to a specific format
    static std::string convertTimestamp(const std::string& timestamp, const std::string& format);

    // Function to split a string by a delimiter
    static std::vector<std::string> splitString(const std::string& str, char delimiter);

    // Function to export weather data to chart
    void exportToChart();
};

#endif // UTILITY_H
