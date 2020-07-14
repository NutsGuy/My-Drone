//Pin des 4 moteurs : Sorties PWM
const char PinARGMot = 6;
const char PinAVGMot = 8;
const char PinARDMot = 9;
const char PinAVDMot = 12;
//Vitesse de rotation pour stabilisation en vol
const byte StabARGMot = 0;
const byte StabAVGMot = 0;
const byte StabARDMot = 0;
const byte StabAVDMot = 0;
//Consignes de vitesse des moteurs
byte ConsARGMot = 0;
byte ConsAVGMot = 0;
byte ConsARDMot = 0;
byte ConsAVDMot = 0;
//Pin des LEDs de position
const char PinLEDPos = 5;
const char PinLEDAct = 7;
//Memoire MES du Drone
bool OnOffDrone;

void setup() {
  //Initialisation des sorties de commande des moteurs en OUTPUT
  pinMode(PinARGMot, OUTPUT);
  pinMode(PinAVGMot, OUTPUT);
  pinMode(PinARDMot, OUTPUT);
  pinMode(PinAVDMot, OUTPUT);
  pinMode(PinLEDPos, OUTPUT);
  OnOffDrone = 0;
  
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:

}
