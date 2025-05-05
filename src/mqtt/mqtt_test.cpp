#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <thread>
#include "mqtt.hpp"
#include "logger.hpp"

TEST(MQTTClientTest, LogMessageOnce) {
    Logger *Log;
    // Run mqtt_client in a thread so it doesn't block
    std::thread t([&]() {
        mqtt_client(Log->getInstance());
    });

    // Give some time for the log to happen
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // Detach or terminate (since mqtt_client never ends by itself)
    t.detach();
}