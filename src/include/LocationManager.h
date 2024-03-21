#ifndef LOCATION_MANAGER_H
#define LOCATION_MANAGER_H

#include <string>
#include <vector>

// Struct to represent a location
struct Location {
    int id;
    std::string name;
    double latitude;
    double longitude;
    bool favorite;
    Location(int _id, const std::string& _name, double _latitude, double _longitude);
};

// Class to manage locations
class LocationManager {
private:
    std::vector<Location> locations;
    int nextId;
public:
    LocationManager();
    void addLocation(const std::string& name, double latitude, double longitude);
    void modifyLocation(int id, const std::string& newName, double newLatitude, double newLongitude);
    bool removeLocation(int id);
    void searchLocations(const std::string& query);
    std::vector<Location> getLocations() const;
    void setFavorite(int id, bool favorite);

private:
    std::vector<Location>::iterator findLocationById(int id);
};

#endif // LOCATION_MANAGER_H
