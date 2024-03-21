#include "./include/LocationManager.h"
#include <iostream>
#include <algorithm>

using namespace std;



Location::Location(int _id, const string& _name, double _latitude, double _longitude)
    : id(_id), name(_name), latitude(_latitude), longitude(_longitude), favorite(false) {}

LocationManager::LocationManager() : nextId(1) {}

void LocationManager::addLocation(const string& name, double latitude, double longitude) {
    locations.push_back(Location(nextId++, name, latitude, longitude));
}

void LocationManager::modifyLocation(int id, const string& newName, double newLatitude, double newLongitude) {
    auto it = findLocationById(id);
    if (it != locations.end()) {
        it->name = newName;
        it->latitude = newLatitude;
        it->longitude = newLongitude;
    } else {
        cout << "Location with ID " << id << " not found." << endl;
    }
}

bool LocationManager::removeLocation(int id) {
    auto it = findLocationById(id);
    if (it != locations.end()) {
        locations.erase(it);
        return true;
    } else {
        cout << "Location with ID " << id << " not found." << endl;
        return false;
    }
}

void LocationManager::searchLocations(const string& query) {
    cout << "Search Results:" << endl;
    bool found = false;
    for (const auto& location : locations) {
        if (location.name.find(query) != string::npos) {
            cout << "ID: " << location.id << ", Name: " << location.name << ", Latitude: " << location.latitude
                << ", Longitude: " << location.longitude << ", Favorite: " << (location.favorite ? "Yes" : "No") << endl;
            found = true;
        }
    }
    if (!found) {
        cout << "No location found with name " << query << endl;
    }
}

vector<Location> LocationManager::getLocations() const {
    return locations;
}

void LocationManager::setFavorite(int id, bool favorite) {
    auto it = findLocationById(id);
    if (it != locations.end()) {
        it->favorite = favorite;
         cout << it->name << " has been set as favorite." << endl;
    } else {
        cout << "Location with ID " << id << " not found." << endl;
    }
}

vector<Location>::iterator LocationManager::findLocationById(int id) {
    return find_if(locations.begin(), locations.end(), [id](const Location& loc) { return loc.id == id; });
}
