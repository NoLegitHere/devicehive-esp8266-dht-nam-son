#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define DHTPIN D2      // Pin connected to DHT11
#define DHTTYPE DHT11  // DHT 11 

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Test";
const char* password = "<x029T43";

const char* serverName = "http://192.168.137.1:80/api/rest/device/pSQgUApakskYMogqmDngfLDZ3i2DtghajvY5/notification";

// Replace with your DeviceHive access token
const char* authToken = "Bearer eyJhbGciOiJIUzI1NiJ9.eyJwYXlsb2FkIjp7ImEiOlswXSwiZSI6MjA1MTExMDgwMDAwMCwidCI6MSwidSI6MSwibiI6WyIqIl0sImR0IjpbIioiXX19.N8XOcOX3SFnhgnJJ5VbTHmKbAipYeoe8chWnXP6eBCw";

WiFiClient wifiClient;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  dht.begin();
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  HTTPClient http;
  http.begin(wifiClient, serverName);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", authToken);

  StaticJsonDocument<200> doc;
  doc["notification"] = "temperature_humidity";
  JsonObject params = doc.createNestedObject("parameters");
  params["temperature"] = temperature;
  params["humidity"] = humidity;

  String requestBody;
  serializeJson(doc, requestBody);

  int httpResponseCode = http.POST(requestBody);
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println(httpResponseCode);
    Serial.println(response);
  } else {
    Serial.println("Error on sending POST: " + String(httpResponseCode));
  }

  http.end();

  delay(5000);  // Send data every 60 seconds
}
