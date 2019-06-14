#include <SPI.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>

#include "config.h"

WiFiClient net;
MQTTClient mqtt;

unsigned long lastMillis = 0;

#define LED_PIN 5

void setup()
{
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(115200);
    delay(10);
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(wifiSSID);

    WiFi.begin(wifiSSID, wifiPassword);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    delay(2000);

    mqtt.begin(mqttHost, mqttPort, net);
    mqtt.onMessage(messageReceived);
    connect();
}

void connect()
{
    Serial.println("connecting to MQTT broker");
    while (!mqtt.connect("nodemcu", mqttUser, mqttPassword))
    {
        Serial.print(".");
        delay(1000);
    }

    Serial.println("\nconnected!");
    mqtt.subscribe("/blinds/state");
}

void loop()
{
    mqtt.loop();
    delay(10); // <- fixes some issues with WiFi stability

    if (!mqtt.connected())
    {
        connect();
    }
}

void messageReceived(String &topic, String &payload)
{
    if (payload == "1")
    {
        digitalWrite(LED_PIN, HIGH);
    }
    else
    {
        digitalWrite(LED_PIN, LOW);
    }
}