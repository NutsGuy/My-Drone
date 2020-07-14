/*
 * NOTES DE VERSION
 * ----------------
 * 
 * 0.1  : Version de base avec la gestion des joystick
 * 
 * 0.11 : Mise en place de la transmission Wifi avec l'ancienne
 *        gestion de calibration
 * 
 * 0.12 : Mise en place de la nouvelle gestion de la calibration
 * 
 * 0.13 : Mise en place de la gestion du LCD
 * 
 * 0.14 : Optimisation de la gestion du LCD, réglage de la calibration
 *        des joystick, ajout d'une clé de sécurité dans ma trame Wifi,
 *        correction beug des boutons des joystick en passant les entrées
 *        en PULL_UP puis inversion des infos dans la trame Wifi.
 */
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

/*Create a unique pipe out. The receiver has to 
  wear the same unique code*/
  
const uint64_t pipeOut = 0xE8E8F0F0E1LL;

const char X1 = A0; //Entrée ANA Joystick 1 axe X (HORIZONTAL)
const char Y1 = A1; //Entrée ANA Joystick 1 axe Y (VERTICAL)
const char S1 = 7;  //Entrée DIG Joystick 1 Switch
const char X2 = A2; //Entrée ANA Joystick 2 axe X (HORIZONTAL)
const char Y2 = A3; //Entrée ANA Joystick 2 axe Y (VERTICAL)
const char S2 = 8;  //Entrée DIG Joystick 2 Switch

const int ColLCD = 16; //Nombre de colonnes du LCD

// set the LCD address to 0x20(Cooperate with 3 short circuit caps) for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27,ColLCD,2);
RF24 radio(9, 10); // select  CSN  pin

// The sizeof this struct should not exceed 32 bytes
// This gives us up to 32 8 bits channals
struct MyData {
  String keycode;   //Code Clé pour dialoguer avec le bon drone
  byte throttle;  //Puissance Moteur : Elevation
  byte yaw;       //Déplacement LATERAL
  byte pitch;     //Avancer / Reculer
  byte roll;      //Rotation G / D
  byte SWX1;      //Switch PAD G
  byte SWX2;      //Switch PAD D
};

MyData data;

void resetData() 
{
  //This are the start values of each channal
  // Throttle is 0 in order to stop the motors
  //127 is the middle value of the 10ADC.
  data.keycode = "1E23F46AA00";
  data.throttle = 127;
  data.yaw = 127;
  data.pitch = 127;
  data.roll = 127;
  data.SWX1 = 0;
  data.SWX2 = 0;
}

void LCD_Init()
  {
  lcd.init();       //Init du LCD
  lcd.backlight();  //Rétroéclairage ON
  LCD_Display("Drone 0.14", "NutsGuy (c) 2018" ,0 ,0);
  delay(2000);
  lcd.clear();
  }

void setup()
  {
    //Utilisation de INPUT_PULLUP afin d'utiliser la résistance interne anti-parasite
    pinMode(S1, INPUT_PULLUP);
    pinMode(S2, INPUT_PULLUP);

    //Afficheur LCD
    LCD_Init();
  
    //Start everything up
    radio.begin();
    radio.setAutoAck(false);
    radio.setDataRate(RF24_2MBPS);
    //radio.setDataRate(RF24_250KBPS);
    radio.openWritingPipe(pipeOut);
    resetData();

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

// LCD_Display s'occupe de la gestion de l'affichage sur le LCD
// StrA & StrB : Les chaines de caractères à afficher
// LineNum : Numéro de ligne à utiliser
// Mode : Mode de gestion de l'affichage
//    Mode 0 : Centrage de StrA sur la 1ère ligne et StrB sur le 2e ligne
//    Mode 1 : StrA à gauche & StrB à droite sur la ligne indiquée avec LineNum

void LCD_Display(String StrA, String StrB, int LineNum, int Mode)
  {
    int LenLA, SpcLA, CptLA = 0;
    switch (Mode)
      {
        case 0: // Mode Centrage normal
          LenLA = StrA.length();
          SpcLA = (ColLCD - LenLA) / 2;
          lcd.setCursor(SpcLA, 0);
          lcd.print(StrA);
          LenLA = StrB.length();
          SpcLA = (ColLCD - LenLA) / 2;
          lcd.setCursor(SpcLA, 1);
          lcd.print(StrB);
          break;
        case 1:
          LenLA = StrA.length() + StrB.length();
          SpcLA = ColLCD - LenLA;
          CptLA = 0;
          lcd.setCursor(0, LineNum - 1);
          lcd.print(StrA);
          while(CptLA < SpcLA)
            {
              lcd.print(" ");
              CptLA += 1;
            }
          lcd.print(StrB);
          break;
      }
  }

void ShowAnalog()
  {
    Serial.print("X1 : "); Serial.print(analogRead(X1)); Serial.print(" | ");
    Serial.print("Y1 : "); Serial.print(analogRead(Y1)); Serial.print(" | ");
    Serial.print("X2 : "); Serial.print(analogRead(X2)); Serial.print(" | ");
    Serial.print("Y2 : "); Serial.print(analogRead(Y2)); Serial.print("\n");
  }

void ShowTramWifi()
  {
    Serial.print("KeyCode : "); Serial.print(data.keycode);  Serial.print("    ");
    Serial.print("Throttle: "); Serial.print(data.throttle);  Serial.print("    ");
    Serial.print("Yaw: ");      Serial.print(data.yaw);       Serial.print("    ");
    Serial.print("Pitch: ");    Serial.print(data.pitch);     Serial.print("    ");
    Serial.print("Roll: ");     Serial.print(data.roll);      Serial.print("    ");
    Serial.print("SWX1: ");     Serial.print(data.SWX1);      Serial.print("    ");
    Serial.print("SWX2: ");     Serial.print(data.SWX2);      Serial.print("\n");
  }

void loop()
  {
  // Préparation des données avant l'envoi en Wifi
  data.throttle = mapJoystickValues( analogRead(X1), 0, 499, 1022, true );
  data.yaw      = mapJoystickValues( analogRead(Y1), 0, 512, 1022, true );
  data.pitch    = mapJoystickValues( analogRead(X2), 0, 498, 1022, true );
  data.roll     = mapJoystickValues( analogRead(Y2), 0, 511, 1022, true );
  data.SWX1     = !digitalRead(S1);  //Réinversion du signal pour faciliter l'utilisation
  data.SWX2     = !digitalRead(S2);  //Réinversion du signal pour faciliter l'utilisation

  // Envoi des données par le Wifi
  radio.write(&data, sizeof(MyData));

  //ShowTramWifi();
  //ShowAnalog();

  //Affichage des infos sur le LCD
  LCD_Display("F/R " + String(data.throttle), String(data.yaw) + " L/R", 1, 1);
  LCD_Display("U/D " + String(data.pitch), String(data.roll) + " L/R", 2, 1);

  //Delay pour le rafraichissement des données envoyées
  delay(500);
  }

