cmake_minimum_required(VERSION 3.10)

project(WDA)

find_package(CURL REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(bsoncxx CONFIG REQUIRED)
find_package(mongocxx CONFIG REQUIRED)
find_package(Matplot++ CONFIG REQUIRED)

add_executable(WDA 
    src/main.cpp
    src/UIController.cpp
    src/LocationManager.cpp  
    src/WeatherForecast.cpp
    src/HistoricalWeather.cpp
    src/AirQualityForecast.cpp
    src/Utility.cpp
)

target_link_libraries(WDA PRIVATE JsonCpp::JsonCpp)
target_link_libraries(WDA PRIVATE CURL::libcurl)
target_link_libraries(WDA PRIVATE $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>)
target_link_libraries(WDA PRIVATE $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>)
target_link_libraries(WDA PRIVATE Matplot++::cimg  Matplot++::matplot)
