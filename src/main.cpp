#include <iostream>
#include <thread>
#include <chrono>
#include "logger.hpp"
#include "mqtt.hpp"

int main() {
    Logger::getInstance().log("Application started.");

    std::thread mqtt(mqtt_client, std::ref(Logger::getInstance()));

    mqtt.join();
    return 0;
}
