//ESP32 Monitoring Unit  (FINAL CLEAN VERSION)
//STM32 → UART → ESP32 → WiFi → Firebase RTDB

#include <WiFi.h>
#include <Firebase_ESP_Client.h>

//UART 
#define RXD2 16
#define TXD2 17

//WIFI 
#define WIFI_SSID     "wifi"
#define WIFI_PASSWORD "12345678"

// FIREBASE 
#define API_KEY      "AIzaSyBJWKKQkyOJFT4PjntJ9R_ehr_AHdmiqHE"
#define DATABASE_URL "unit-89d9b-default-rtdb.asia-southeast1.firebasedatabase.app"

// FIREBASE OBJECTS 
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup()
{
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  Serial.println("\n===== Monitoring Unit Booting =====");

  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected ✅");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  /******** Firebase Init ********/
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase Ready ✅");
}
void loop()
{
  if (Serial2.available())
  {
    String data = Serial2.readStringUntil('\n');
    data.trim();

    Serial.print("Received: ");
    Serial.println(data);

    float temp, hum, pressure;

    // Expect: temp,hum,pressure
    if (sscanf(data.c_str(), "%f,%f,%f", &temp, &hum, &pressure) == 3)
    {
      bool ok = true;

      ok &= Firebase.RTDB.setFloat(&fbdo, "/monitoring_unit/temperature", temp);
      ok &= Firebase.RTDB.setFloat(&fbdo, "/monitoring_unit/humidity", hum);
      ok &= Firebase.RTDB.setFloat(&fbdo, "/monitoring_unit/pressure", pressure);

      if (ok)
      {
        Serial.println("Upload Success ✅\n");
      }
      else
      {
        Serial.print("Firebase Error ❌ : ");
        Serial.println(fbdo.errorReason());
      }
    }
    else
    {
      Serial.println("Invalid format!\n");
    }
  }
}
