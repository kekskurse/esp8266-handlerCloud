#ifndef handlerCloud
#define handlerCloud

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <EEPROM.h>



#ifdef ESP8266
#include <functional>
#define MQTT_CALLBACK_SIGNATURE std::function<void(char*, uint8_t*, unsigned int)> callback
#else
#define MQTT_CALLBACK_SIGNATURE void (*callback)(char*, uint8_t*, unsigned int)
#endif


class HandlerCloud
{
  public:
    // Constructor
    HandlerCloud();

    // Methods
    void setupAll(bool debug = false, int baudRate=115200);
    void checkAll();

    void setupDebug(int baudRate=115200);
    void setupWifi();
    void checkWifi();
    void setupWebserver();
    void handleClient();
    void setupMQTT();
    void checkMQTT();
    bool addTopic(char* topic);
    void setCallback(MQTT_CALLBACK_SIGNATURE);
    void publishMessage(char* topic, char* message);
    void handleMQTT();

    long getRandomNumber();

  private:
    bool _debug;
    bool _internetConnection;
    String _topics;
    int _maxTopicCount;
    void debugMessage(char* message, bool newline = true);
    void debugMessage(String message, bool newline = true);
    String macToStr(const uint8_t* mac);
    String getClientID();
    String getConfig(char* configName);
    String redEERromString(int start, int lenght);

};


#endif
