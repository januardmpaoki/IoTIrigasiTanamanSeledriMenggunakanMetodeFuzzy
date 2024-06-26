#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Fuzzy.h>

// Declare Pin Relay
int relayPin = 32;

// Declare Fuzzy Class
Fuzzy *fuzzy = new Fuzzy();

// Receiver Mac   : A8:42:E3:C8:2D:18 (this device)
// Transmiter Mac : C0:49:EF:F0:11:4C

// Structure to receive data, Struktur untuk menerima data
// Must match the sender structure, harus sama dengan struktur Pengirim
typedef struct struct_message
{
  // Variabel Sensor
  int Sensor1;
  int Sensor2;
  int Rawval1;
  int Rawval2;
} struct_message;

// Create a struct_message called myData, struct_message bernama myData
struct_message myData;

// callback function that will be executed when data is received
// Fungsi callback yang akan di mulai ketika data di terima
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.println("Sent with success");
  Serial.print(myData.Sensor1);
  Serial.print("% | Raw1 :");
  Serial.println(myData.Rawval1);
  Serial.print(myData.Sensor2);
  Serial.print("% | Raw2 :");
  Serial.println(myData.Rawval2);
}

void setup()
{
  // Serial Monitor
  Serial.begin(115200);

  // Set input fuzzy sensor 1
  FuzzySet *Dry1 = new FuzzySet(0, 0, 40, 50);
  FuzzySet *Mid1 = new FuzzySet(30, 50, 70, 90);
  FuzzySet *Wet1 = new FuzzySet(60, 80, 100, 100);
  // set input sensor 2
  FuzzySet *Dry2 = new FuzzySet(0, 0, 40, 50);
  FuzzySet *Mid2 = new FuzzySet(30, 50, 70, 90);
  FuzzySet *Wet2 = new FuzzySet(60, 80, 100, 100);

  // set output fuzzy
  FuzzySet *RelayOff = new FuzzySet(0, 0, 0.6, 0.8);
  FuzzySet *RelayOn = new FuzzySet(0.5, 0.8, 1, 1);

  // Fuzzy Input 1
  FuzzyInput *sensor1 = new FuzzyInput(1);
  sensor1->addFuzzySet(Dry1);
  sensor1->addFuzzySet(Mid1);
  sensor1->addFuzzySet(Wet1);
  fuzzy->addFuzzyInput(sensor1);

  // Fuzzy Input 2
  FuzzyInput *sensor2 = new FuzzyInput(2);
  sensor2->addFuzzySet(Dry2);
  sensor2->addFuzzySet(Mid2);
  sensor2->addFuzzySet(Wet2);
  fuzzy->addFuzzyInput(sensor2);

  // Fuzzy Output
  FuzzyOutput *FRelay = new FuzzyOutput(1);
  FRelay->addFuzzySet(RelayOff);
  FRelay->addFuzzySet(RelayOn);
  fuzzy->addFuzzyOutput(FRelay);

  // Fuzzy Rule 1
  FuzzyRuleAntecedent *Dry1_Dry2 = new FuzzyRuleAntecedent();
  Dry1_Dry2->joinWithAND(Dry1, Dry2);
  FuzzyRuleConsequent *RelayOn1 = new FuzzyRuleConsequent();
  RelayOn1->addOutput(RelayOn);
  FuzzyRule *FuzzyRule1 = new FuzzyRule(1, Dry1_Dry2, RelayOn1);
  fuzzy->addFuzzyRule(FuzzyRule1);

  // Rule 2
  FuzzyRuleAntecedent *Dry1_Mid2 = new FuzzyRuleAntecedent();
  Dry1_Mid2->joinWithAND(Dry1, Mid2);
  FuzzyRuleConsequent *RelayOn2 = new FuzzyRuleConsequent();
  RelayOn2->addOutput(RelayOn);
  FuzzyRule *FuzzyRule2 = new FuzzyRule(2, Dry1_Mid2, RelayOn2);
  fuzzy->addFuzzyRule(FuzzyRule2);

  // Rule 3
  FuzzyRuleAntecedent *Dry1_Wet2 = new FuzzyRuleAntecedent();
  Dry1_Wet2->joinWithAND(Dry1, Wet2);
  FuzzyRuleConsequent *RelayOn3 = new FuzzyRuleConsequent();
  RelayOn3->addOutput(RelayOn);
  FuzzyRule *FuzzyRule3 = new FuzzyRule(3, Dry1_Wet2, RelayOn3);
  fuzzy->addFuzzyRule(FuzzyRule3);

  // Rule 4
  FuzzyRuleAntecedent *Mid1_Mid2 = new FuzzyRuleAntecedent();
  Mid1_Mid2->joinWithAND(Mid1, Mid2);
  FuzzyRuleConsequent *RelayOff1 = new FuzzyRuleConsequent();
  RelayOff1->addOutput(RelayOff);
  FuzzyRule *FuzzyRule4 = new FuzzyRule(4, Mid1_Mid2, RelayOff1);
  fuzzy->addFuzzyRule(FuzzyRule4);

  // Rule 5
  FuzzyRuleAntecedent *Mid1_Dry2 = new FuzzyRuleAntecedent();
  Mid1_Dry2->joinWithAND(Mid1, Dry2);
  FuzzyRuleConsequent *RelayOn4 = new FuzzyRuleConsequent();
  RelayOn4->addOutput(RelayOn);
  FuzzyRule *FuzzyRule5 = new FuzzyRule(5, Mid1_Dry2, RelayOn4);
  fuzzy->addFuzzyRule(FuzzyRule5);

  // Rule 6
  FuzzyRuleAntecedent *Mid1_Wet2 = new FuzzyRuleAntecedent();
  Mid1_Wet2->joinWithAND(Mid1, Wet2);
  FuzzyRuleConsequent *RelayOff2 = new FuzzyRuleConsequent();
  RelayOff2->addOutput(RelayOff);
  FuzzyRule *FuzzyRule6 = new FuzzyRule(6, Mid1_Wet2, RelayOff2);
  fuzzy->addFuzzyRule(FuzzyRule6);

  // Rule 7
  FuzzyRuleAntecedent *Wet1_Wet2 = new FuzzyRuleAntecedent();
  Wet1_Wet2->joinWithAND(Wet1, Wet2);
  FuzzyRuleConsequent *RelayOff3 = new FuzzyRuleConsequent();
  RelayOff3->addOutput(RelayOff);
  FuzzyRule *FuzzyRule7 = new FuzzyRule(7, Wet1_Wet2, RelayOff3);
  fuzzy->addFuzzyRule(FuzzyRule7);

  // Rule 8
  FuzzyRuleAntecedent *Wet1_Dry2 = new FuzzyRuleAntecedent();
  Wet1_Dry2->joinWithAND(Wet1, Dry2);
  FuzzyRuleConsequent *RelayOn5 = new FuzzyRuleConsequent();
  RelayOn5->addOutput(RelayOn);
  FuzzyRule *FuzzyRule8 = new FuzzyRule(8, Wet1_Dry2, RelayOn5);
  fuzzy->addFuzzyRule(FuzzyRule8);

  // Rule 9
  FuzzyRuleAntecedent *Wet1_Mid2 = new FuzzyRuleAntecedent();
  Wet1_Mid2->joinWithAND(Wet1, Mid2);
  FuzzyRuleConsequent *RelayOff4 = new FuzzyRuleConsequent();
  RelayOff4->addOutput(RelayOff);
  FuzzyRule *FuzzyRule9 = new FuzzyRule(9, Wet1_Mid2, RelayOff4);
  fuzzy->addFuzzyRule(FuzzyRule9);

  pinMode(relayPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // Set device as a Wi-Fi Station
  // terapkan alat sebagai Sasiun Wi-Fi
  WiFi.mode(WIFI_STA);

  // Inititate ESP-NOW, inisiasi ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, register for recv CB to get recv packer info
  // Setelah ESPNow telah di inisiasi, daftarkan Recovery CallBack untuk mengambil info Packer
  esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{
  // simplify data received variable from ESPNOW, simpelkan data yang di terima dari ESPNOW
  int Moisture1 = myData.Sensor1;
  int Moisture2 = myData.Sensor2;
  int SensorVal1 = myData.Rawval1;
  int SensorVal2 = myData.Rawval2;

  // variabel input set fuzzy
  fuzzy->setInput(1, Moisture1);
  fuzzy->setInput(2, Moisture2);

  // fuzzyfication
  fuzzy->fuzzify();

  // get fuzzy output
  float relayOutput = fuzzy->defuzzify(1);

  Serial.print("relay :");
  Serial.println(relayOutput);
  if (relayOutput >= 0.36)
  {
    digitalWrite(relayPin, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
  }
  else
  {
    digitalWrite(relayPin, LOW);
    digitalWrite(LED_BUILTIN, LOW);
  }

  delay(1000);
}