#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <mqtt/async_client.h>
#include "mqtt.hpp"

class MyMqttClient : public virtual mqtt::callback {
private:
    mqtt::async_client client;
    mqtt::connect_options connOpts;
    const std::string topic;
    const int qos;

public:
    MyMqttClient(const std::string& server, const std::string& clientId, const std::string& topic_, int qos_ = 1)
        : client(server, clientId), topic(topic_), qos(qos_) {
        client.set_callback(*this);

        connOpts.set_clean_session(true);
    }

    void connect() {
        std::cout << "Connecting..." << std::endl;
        client.connect(connOpts)->wait();
        std::cout << "Connected." << std::endl;
    }

    void subscribe() {
        std::cout << "Subscribing to topic: " << topic << std::endl;
        client.subscribe(topic, qos)->wait();
    }

    void publish(const std::string& payload) {
        mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
        pubmsg->set_qos(qos);
        std::cout << "Publishing: " << payload << std::endl;
        client.publish(pubmsg)->wait();
    }

    void disconnect() {
        client.disconnect()->wait();
        std::cout << "Disconnected." << std::endl;
    }

    // Callbacks
    void connection_lost(const std::string& cause) override {
        std::cerr << "Connection lost: " << cause << std::endl;
    }

    void message_arrived(mqtt::const_message_ptr msg) override {
        std::cout << "Message received:\n"
                    << "  Topic: " << msg->get_topic() << "\n"
                    << "  Payload: " << msg->to_string() << std::endl;
    }

    void delivery_complete(mqtt::delivery_token_ptr token) override {
        std::cout << "Delivery complete (msg ID: "
                    << (token ? token->get_message_id() : -1) << ")\n";
    }
};

    
void mqtt_client(Logger &LOG) {
    LOG.log("MQTT thread started.");
    MyMqttClient mqtt("tcp://localhost:1883", "client123", "test/topic");

    try {
        mqtt.connect();
        mqtt.subscribe();
        mqtt.publish("Hello from MyMqttClient!");

        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }
    catch (const mqtt::exception& e) {
        mqtt.disconnect();
        std::cerr << "MQTT error: " << e.what() << std::endl;
    }
}