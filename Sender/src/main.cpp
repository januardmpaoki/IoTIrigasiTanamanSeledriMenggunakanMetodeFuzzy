#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// Declare pin Sensor
#define sensorPin1 35
#define sensorPin2 32

// Min and Max Value Sensor1
int dryValSensor1 = 2950;
int wetValSensor1 = 1340;
// Min and Max Value Sensor2
int dryValSensor2 = 2950;
int wetValSensor2 = 1320;

// percentage, persertase
int friendlyDryVal = 0;
int friendlyWetVal = 100;

// Receiver Mac   : A8:42:E3:C8:2D:18
// Transmiter Mac : C0:49:EF:F0:11:4C (this device)

// Macc Address Receiver
uint8_t broadcastAddress[] = {0xA8, 0x42, 0xE3, 0xC8, 0x2D, 0x18};

// Structure to send data, struktur untuk mengirim data
// Must match the receiver structure, harus sama dengan struktur penerima
typedef struct struct_message
{
  // sensor variable
  int Sensor1;
  int Sensor2;
  int Rawval1;
  int Rawval2;
} struct_message;

// Create a struct_message called myData, struct_message bernama myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent, callbackc ketika data terkirim
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup()
{
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  // terapkan alat sebagai Sasiun Wi-Fi
  WiFi.mode(WIFI_STA);

  // Inititate ESP-NOW, inisiasi ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  /*
    Once ESPNow is successfully Initialized
    it will register for Send CB to get the status of Trasmitted packet
    Setelah ESPNow telah di inisiasi, itu akan mendaftar ke Send CB Untuk mendapatkan status paket terkirim
  */
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop()
{
  // Read Sensor
  // Baca Sensor
  int sensorVal1 = analogRead(sensorPin1);
  int sensorVal2 = analogRead(sensorPin2);

  // map sensor value ke percentage format
  // batasi nilai sensor ke format persentasi
  int moisture1 = map(sensorVal1, dryValSensor1, wetValSensor1, friendlyDryVal, friendlyWetVal);
  int moisture2 = map(sensorVal2, dryValSensor2, wetValSensor2, friendlyDryVal, friendlyWetVal);

  // make variable unique for constrained value
  // buat variabel unik untuk nilai yang telah dibatasi
  int constMoist1;
  int constMoist2;

  // constrain value to not exceed between 0 and 100
  // batasi nilai untuk tidak melewati nilai 0 sampai 100
  constMoist1 = constrain(moisture1, 0, 100);
  constMoist2 = constrain(moisture2, 0, 100);

  // Set values to send
  // set nilai untuk di kirim
  myData.Sensor1 = constMoist1;
  myData.Sensor2 = constMoist2;
  myData.Rawval1 = sensorVal1;
  myData.Rawval2 = sensorVal2;

  // Send message via ESP-NOW
  // kirim data lewat ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

  if (result == ESP_OK)
  {
    Serial.println("Sent with success");
    // Serial.print("|");
    Serial.print(myData.Sensor1);
    Serial.print("|");
    Serial.print(moisture1);
    Serial.print("|");
    Serial.println(sensorVal1);
    Serial.print(myData.Sensor2);
    Serial.print("|");
    Serial.print(moisture2);
    Serial.print("|");
    Serial.print(sensorVal2);
  }
  else
  {
    Serial.println("Error sending the data");
  }
  delay(2000);
}