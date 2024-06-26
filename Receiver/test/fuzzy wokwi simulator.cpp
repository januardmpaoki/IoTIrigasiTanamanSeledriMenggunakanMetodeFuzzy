#include <arduino.h>
#include <Fuzzy.h>

// set relay pin
#define relayPin 33
#define sensorPin1 35
#define sensorPin2 32

// Value Sensor1
int dryValSensor1 = 0;
int wetValSensor1 = 4095;
// Value Sensor2
int dryValSensor2 = 0;
int wetValSensor2 = 4095;

// percentage
int friendlyDryVal = 0;
int friendlyWetVal = 100;

// Variable to store the time when the relay is turned on
unsigned long relayStartTime = 0;

/*
  millisecond = 1000, 60*1000 = 1 minute,
  duration * second * 1000 millisecond = desired duration
*/
const unsigned long relayDuration = 30 * 60 * 1000;
const unsigned long waitDuration = 60 * 60 * 1000;
// Flag to check if the relay is currently on
bool relayStt = false;

// Declare Fuzzy Class
Fuzzy *fuzzy = new Fuzzy();

// Set input fuzzy sensor 1
FuzzySet *Dry1 = new FuzzySet(0, 0, 40, 60);
FuzzySet *Mid1 = new FuzzySet(40, 60, 70, 90);
FuzzySet *Wet1 = new FuzzySet(70, 90, 100, 100);
// set input sensor 2
FuzzySet *Dry2 = new FuzzySet(0, 0, 40, 60);
FuzzySet *Mid2 = new FuzzySet(40, 60, 70, 90);
FuzzySet *Wet2 = new FuzzySet(70, 90, 100, 100);

// set output fuzzy
FuzzySet *RelayOff = new FuzzySet(0, 0, 40, 60);
FuzzySet *RelayOn = new FuzzySet(0.2, 0.5, 0.7, 1);

void setup()
{
    Serial.begin(115200);

    // Set input fuzzy sensor 1
    FuzzySet *Dry1 = new FuzzySet(0, 0, 40, 60);
    FuzzySet *Mid1 = new FuzzySet(40, 60, 70, 90);
    FuzzySet *Wet1 = new FuzzySet(70, 90, 100, 100);
    // set input sensor 2
    FuzzySet *Dry2 = new FuzzySet(0, 0, 40, 60);
    FuzzySet *Mid2 = new FuzzySet(40, 60, 70, 90);
    FuzzySet *Wet2 = new FuzzySet(70, 90, 100, 100);

    // set output fuzzy
    FuzzySet *RelayOff = new FuzzySet(0, 0, 40, 60);
    FuzzySet *RelayOn = new FuzzySet(0.2, 0.5, 0.7, 1);

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

    // Fuzzy Rule 1, if put dry3 theres a error
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
}

void loop()
{
    // Read Sensor
    int Sensorval1 = analogRead(sensorPin1);
    int Sensorval2 = analogRead(sensorPin2);

    // convert input data to percentage (use based on sensor)
    int moisture1 = map(Sensorval1, dryValSensor1, wetValSensor1, friendlyDryVal, friendlyWetVal);
    int moisture2 = map(Sensorval2, dryValSensor2, wetValSensor2, friendlyDryVal, friendlyWetVal);

    // make variable unique for constrained value
    int constMoist1;
    int constMoist2;
    // constrain value to not exceed between 0 and 100
    constMoist1 = constrain(moisture1, 0, 100);
    constMoist2 = constrain(moisture2, 0, 100);

    // set fuzzy input
    fuzzy->setInput(1, constMoist1);
    fuzzy->setInput(2, constMoist2);

    // get fuzzy output
    float relayOutput = fuzzy->defuzzify(1);

    Serial.print("fuzzy : ");
    Serial.println(relayOutput);

    Serial.print("Sensor1 raw :");
    Serial.print(Sensorval1);
    Serial.print(" | % :");
    Serial.println(constMoist1);

    Serial.print("Sensor2 raw :");
    Serial.print(Sensorval2);
    Serial.print(" | % :");
    Serial.println(constMoist2);

    // Set relay output
    if (relayOutput >= 0.3) // Adjust the threshold as needed
    {
        digitalWrite(relayPin, HIGH);
        Serial.println("Relay : ON ");
    }
    else
    {
        digitalWrite(relayPin, LOW);
        Serial.println("Relay : OFF ");
    }

    delay(1000);
}