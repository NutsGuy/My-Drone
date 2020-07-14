#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

/*Create a unique pipe out. The receiver has to 
  wear the same unique code*/
  
const uint64_t pipeOut = 0xE8E8F0F0E1LL;

RF24 radio(9, 10); // select  CSN  pin

// Valeurs des pin utilisées
const char PinXAxe1 = A0; //Pin Analogique A0
const char PinYAxe1 = A1; //Pin Analogique A1
const char PinSAxe1 = 7;  //Pin Digital D7

const char PinXAxe2 = A2; //Pin Analogique A2
const char PinYAxe2 = A3; //Pin Analogique A3
const char PinSAxe2 = 8;  //Pin Digital D8

// Valeurs de Calibration au démarrage
//int CalAxe1X, CalAxe1Y, CalAxe2X, CalAxe2Y;

// Valeurs état des axes et du btn
int ValXAxe1, ValYAxe1, ValXAxe2, ValYAxe2;
// Valeurs de décalage des Axes par rapport au milieu théorique qui est 512.
int CalXAxe1, CalYAxe1;
//int ValAxe1S, ValAxe2S;
bool EtaSAxe1, EtaSAxe2;

// The sizeof this struct should not exceed 32 bytes
// This gives us up to 32 8 bits channals
struct MyData {
  byte throttle;
  byte yaw;
  byte pitch;
  byte roll;
  byte AUX1;
  byte AUX2;
};

MyData data;

void resetData() 
{
  //This are the start values of each channal
  // Throttle is 0 in order to stop the motors
  //127 is the middle value of the 10ADC.
    
  data.throttle = 0;
  data.yaw = 127;
  data.pitch = 127;
  data.roll = 127;
  data.AUX1 = 0;
  data.AUX2 = 0;
}

void setup()
  {
  pinMode(PinSAxe1, INPUT);
  digitalWrite(PinSAxe1, HIGH); //Mis à 1 par défaut, si appui sur btn il passe à 0. Evite des surprises.

  pinMode(PinSAxe2, INPUT);
  digitalWrite(PinSAxe2, HIGH); //Mis à 1 par défaut, si appui sur btn il passe à 0. Evite des surprises.

  //Start everything up
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipeOut);
  resetData();

  CalXAxe1 = 512 - analogRead(PinXAxe1);
  CalYAxe1 = 512 - analogRead(PinYAxe1);

  //initialisation du port série
  Serial.begin(9600);

  }

// Returns a corrected value for a joystick position that takes into account
// the values of the outer extents and the middle of the joystick range.
int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse)
{
  val = constrain(val, lower, upper);
  if ( val < middle )
    val = map(val, lower, middle, 0, 128);
  else
    val = map(val, middle, upper, 128, 255);
  return ( reverse ? 255 - val : val );
}

void loop()
  {
    //Récupération des valeurs Axe 1
    ValXAxe1 = analogRead(PinXAxe1);// + CalAxe1X;
    ValYAxe1 = analogRead(PinYAxe1);// + CalAxe1Y;

    //ValAxe1X = map(ValAxe1X, 0, 1023, -100, 100);
    //ValAxe1Y = map(ValAxe1Y, 0, 1023, -100, 100);

    //Récupération des valeurs Axe 2
    ValXAxe2 = analogRead(PinXAxe2);
    //ValAxe2X = map(ValAxe2X, 0, 1023, 0, 100);
    ValYAxe2 = analogRead(PinYAxe2);
    //ValAxe2Y = map(ValAxe2Y, 0, 1023, -100, 100);

    //Récupération des boutons des Axes 1 & 2
    EtaSAxe1 = digitalRead(PinSAxe1);
    EtaSAxe2 = digitalRead(PinSAxe2);

    /*FONCTION MAP
     * MAP (Valeur_à_mapper,Valeur_min_Joy, Valeur_max_Joy, Valeur_min_Mappée, Val_max_mappée)
     * MAP (AxeX, 0, 1023, Min_Nouvelle_Plage, Max_Nouvelle_Plage)
     * YOUTUBE : https://www.youtube.com/watch?v=S4l4zddOmp4
     * YOUTUBE : https://www.youtube.com/watch?v=HONaw04L8js
     */
    
    //Affichage des valeurs
    Serial.println("********* LECTURE *********");
    //Affichage Valeurs PAD 1
    Serial.print("PAD 1 (Gauche) : AV/AR=");
    Serial.print(ValXAxe1);
    Serial.print(" | G/D=");
    Serial.print(ValYAxe1);
    if(EtaSAxe1 == true)
    {
      Serial.println("% | Btn = OFF");
    }
    else
    {
      Serial.println("% | Btn = ON");
    }
    
    //Affichage Valeurs PAD 2
    Serial.print("PAD 2 (Droite) : Elev=");
    Serial.print(ValXAxe2);
    Serial.print(" | Rot G/D=");
    Serial.print(ValYAxe2);
    if(EtaSAxe2 == true)
    {
      Serial.println("% | Btn = OFF");
    }
    else
    {
      Serial.println("% | Btn = ON");
    }
    delay(500);
  // The calibration numbers used here should be measured 
  // for your joysticks till they send the correct values.
  data.throttle = mapJoystickValues( analogRead(A0), 13, 524, 1015, true );
  data.yaw      = mapJoystickValues( analogRead(A1),  1, 505, 1020, true );
  data.pitch    = mapJoystickValues( analogRead(A2), 12, 544, 1021, true );
  data.roll     = mapJoystickValues( analogRead(A3), 34, 522, 1020, true );
  data.AUX1     = digitalRead(7);
  data.AUX2     = digitalRead(8);

  radio.write(&data, sizeof(MyData));

  }

