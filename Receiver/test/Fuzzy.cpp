#include <Arduino.h>
#include <fuzzy.h>

// assign pin
int Relaypin = 27;
int Sensorpin1 = 26;
int Sensorpin2 = 25;

// declare fuzzy class
Fuzzy *fuzzy = new Fuzzy();

// set input fuzzy sensor 1
FuzzySet *Dry1 = new FuzzySet(0, 0, 40, 60);
FuzzySet *Mid1 = new FuzzySet(40, 60, 70, 90);
FuzzySet *Wet1 = new FuzzySet(70, 90, 100, 100);
// set input sensor 2
FuzzySet *Dry2 = new FuzzySet(0, 0, 40, 60);
FuzzySet *Mid2 = new FuzzySet(40, 60, 70, 90);
FuzzySet *Wet2 = new FuzzySet(70, 90, 100, 100);
// set input sensor 3
FuzzySet *Dry3 = new FuzzySet(0, 0, 40, 60);
FuzzySet *Mid3 = new FuzzySet(40, 60, 70, 90);
FuzzySet *Wet3 = new FuzzySet(70, 90, 100, 100);
// set output fuzzy
FuzzySet *RelayOff = new FuzzySet(0, 0, 0.2, 0.5);
FuzzySet *RelayOn = new FuzzySet(0.2, 0.5, 0.7, 1);

void setup()
{
    // decrale analog read to pin
    int Sensorval1 = analogRead(Sensorpin1);
    int Sensorval2 = analogRead(Sensorpin2);

    // convert sensor read data to percentage
    // use this if dry = high value
    int moisture1 = (100 - ((Sensorval1 / 4095.00) * 100));
    int moisture2 = (100 - ((Sensorval2 / 4095.00) * 100));
    // use this if wet = high value
    // int moisture1 = (Sensorval1 / 4095.00) * 100;
    // int moisture2 = (Sensorval2 / 4095.00) *100;

    // assign pin mode
    pinMode(Relaypin, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(Sensorpin1, INPUT);
    pinMode(Sensorpin2, INPUT);
    Serial.begin(115200);

    // fuzzyInput1
    FuzzyInput *sensor1 = new FuzzyInput(1);
    sensor1->addFuzzySet(Dry1);
    sensor1->addFuzzySet(Mid1);
    sensor1->addFuzzySet(Wet1);
    fuzzy->addFuzzyInput(sensor1);
    // fuzzyInput2
    FuzzyInput *sensor2 = new FuzzyInput(2);
    sensor2->addFuzzySet(Dry2);
    sensor2->addFuzzySet(Mid2);
    sensor2->addFuzzySet(Wet2);
    fuzzy->addFuzzyInput(sensor2);
    // fuzzyInput3 (unused?)
    FuzzyInput *sensor3 = new FuzzyInput(3);
    sensor3->addFuzzySet(Dry3);
    sensor3->addFuzzySet(Mid3);
    sensor3->addFuzzySet(Wet3);
    fuzzy->addFuzzyInput(sensor3);
    // Fuzzy Output
    FuzzyOutput *FRelay = new FuzzyOutput(1);
    FRelay->addFuzzySet(RelayOff);
    FRelay->addFuzzySet(RelayOn);
    fuzzy->addFuzzyOutput(FRelay);

    // Reminder = fuzzy set output is RelayOff, and RelayOn (21 rule total)(9 rule if only 2 input)
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
    Dry1_Wet2->joinWithAND(Dry1, Mid2);
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
}

void loop()
{

    // Read Sensor
    int Sensorval1 = analogRead(Sensorpin1);
    int Sensorval2 = analogRead(Sensorpin2);

    // convert input data to percentage (use based on sensor)
    // lower raw value = higher moisture level
    int moisture1 = (100 - ((Sensorval1 / 4095.00) * 100));
    int moisture2 = (100 - ((Sensorval2 / 4095.00) * 100));
    /*
    // Higher raw value = higher moisture level
    int moisture1 = 100.0 - ((Sensorval1 / 4095.0) * 100);
    int moisture2 = 100.0 - ((Sensorval2 / 4095.0) * 100);
    */

    // set fuzzy input
    fuzzy->setInput(1, moisture1);
    fuzzy->setInput(2, moisture2);

    // get fuzzy output
    float relayOutput = fuzzy->defuzzify(1);

    // Convert fuzzy output (HOW???)
    int relayTrueValue = (relayOutput >= 0.5) ? 1 : 0;

    // Set relay output(whichone is better?)
    // digitalWrite(Relaypin, relayTrueValue);
    // digitalWrite(LED_BUILTIN, HIGH);
    if (relayTrueValue == 1)
    {
        digitalWrite(Relaypin, HIGH);
        digitalWrite(LED_BUILTIN, HIGH);
    }
    else
    {
        digitalWrite(Relaypin, LOW);
        digitalWrite(LED_BUILTIN, LOW);
    }
    delay(1000);
}