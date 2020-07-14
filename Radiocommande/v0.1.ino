//#include <Wire.h>

// Valeurs des pin utilisées
const char PinAxe1X = A0;
const char PinAxe1Y = A1;
const char PinAxe1S = 11;

const char PinAxe2X = A2;
const char PinAxe2Y = A3;
const char PinAxe2S = 10;

// Valeurs de Calibration au démarrage
//int CalAxe1X, CalAxe1Y, CalAxe2X, CalAxe2Y;

// Valeurs état des axes et du btn
int ValAxe1X, ValAxe1Y, ValAxe2X, ValAxe2Y;
// Valeurs de décalage des Axes par rapport au milieu théorique qui est 512.
int CalAxe1X, CalAxe1Y;
//int ValAxe1S, ValAxe2S;
bool ValAxe1S, ValAxe2S;

void setup()
  {
  pinMode(PinAxe1S, INPUT);
  digitalWrite(PinAxe1S, HIGH); //Mis à 1 par défaut, si appui sur btn il passe à 0. Evite des surprises.

  pinMode(PinAxe2S, INPUT);
  digitalWrite(PinAxe2S, HIGH); //Mis à 1 par défaut, si appui sur btn il passe à 0. Evite des surprises.

  CalAxe1X = 512 - analogRead(PinAxe1X);
  CalAxe1Y = 512 - analogRead(PinAxe1Y);

  //initialisation du port série
  Serial.begin(9600);

  }

void loop()
  {
    //Récupération des valeurs Axe 1
    ValAxe1X = analogRead(PinAxe1X);// + CalAxe1X;
    ValAxe1Y = analogRead(PinAxe1Y);// + CalAxe1Y;

    //ValAxe1X = map(ValAxe1X, 0, 1023, -100, 100);
    //ValAxe1Y = map(ValAxe1Y, 0, 1023, -100, 100);

    //Récupération des valeurs Axe 2
    ValAxe2X = analogRead(PinAxe2X);
    //ValAxe2X = map(ValAxe2X, 0, 1023, 0, 100);
    ValAxe2Y = analogRead(PinAxe2Y);
    //ValAxe2Y = map(ValAxe2Y, 0, 1023, -100, 100);

    //Récupération des boutons des Axes 1 & 2
    ValAxe1S = digitalRead(PinAxe1S);
    ValAxe2S = digitalRead(PinAxe2S);

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
    Serial.print(ValAxe1X);
    Serial.print(" | G/D=");
    Serial.print(ValAxe1Y);
    Serial.print(" | Btn=");
    Serial.print(ValAxe1S);
    
    if(ValAxe1S == true)
    {
      Serial.println("% | Btn = OFF");
    }
    else
    {
      Serial.println("% | Btn = ON");
    }
    
    //Affichage Valeurs PAD 2
    Serial.print("PAD 2 (Droite) : Elev=");
    Serial.print(ValAxe2X);
    Serial.print(" | Rot G/D=");
    Serial.print(ValAxe2Y);
    if(ValAxe2S == true)
    {
      Serial.println("% | Btn = OFF");
    }
    else
    {
      Serial.println("% | Btn = ON");
    }
    delay(500);
  }

