/*
 * NOTES DE VERSION
 * ----------------
 * 
 * 0.1  : Version de base avec la gestion des joystick
 * 
 * 0.2  : Mise en place de la transmission Wifi avec l'ancienne
 *        gestion de calibration
 * 
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

//Define widths
int pwm_width_2 = 0;
int pwm_width_3 = 0;
int pwm_width_4 = 0;
int pwm_width_5 = 0;

Servo PWM2;
Servo PWM3;
Servo PWM4;
Servo PWM5;

//Clé de connexion Wifi
const String KeyCode = "1E23F46AA00";
//const String KeyCode = "1E23F46AA00";

                    //We could use up to 32 channels
struct MyData
  {
    String keycode;
    byte throttle;      //We define each byte of data input, in this case just 6 channels
    byte yaw;
    byte pitch;
    byte roll;
    byte SWX1;
    byte SWX2;
    byte DrOn;
  };

MyData dataRX;


const uint64_t pipeIn = 0xE8E8F0F0E1LL;     //Remember that this code is the same as in the transmitter
RF24 radio(9, 10); 



void resetData()
  {
//We define the inicial value of each data input
//3 potenciometers will be in the middle position so 127 is the middle from 254
    //dataTX.keycode = "1E23F46AA00";
    dataRX.keycode = KeyCode;
    dataRX.throttle = 127;
    dataRX.yaw = 127;
    dataRX.pitch = 127;
    dataRX.roll = 127;
    dataRX.SWX1 = 0;
    dataRX.SWX2 = 0;
    dataRX.DrOn = 0;
  }

/**************************************************/

void setup()
{
  //Set the pins for each PWM signal
  PWM2.attach(2);
  PWM3.attach(3);
  PWM4.attach(4);
  PWM5.attach(5);

  //Configure the NRF24 module
  resetData();
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_2MBPS);  
  //radio.setDataRate(RF24_250KBPS);  
  radio.openReadingPipe(1,pipeIn);
  /*
  rf24_pa_dbm_e {RF24_PA_MIN = 0, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX, RF24_PA_ERROR}
  Power Amplifier level. More... For use with setPALevel()
  rf24_datarate_e { RF24_1MBPS = 0, RF24_2MBPS, RF24_250KBPS }
  Data rate. More... For use with setDataRate()
  rf24_crclength_e { RF24_CRC_DISABLED = 0, RF24_CRC_8, RF24_CRC_16 }
  CRC Length. More... For use with setCRCLength() 
  */
  
  Serial.begin(9600);
  
  //we start the radio comunication
  radio.startListening();
}

/**************************************************/

unsigned long lastRecvTime = 0;

void recvData()
  {
    while ( radio.available() )
      {
        radio.read(&dataRX, sizeof(MyData));
        lastRecvTime = millis(); //here we receive the data
      }
  }

void ShowTramWifi()
  {
    Serial.print("KeyCode : "); Serial.print(dataRX.keycode);  Serial.print("    ");
    Serial.print("Throttle: "); Serial.print(dataRX.throttle);  Serial.print("    ");
    Serial.print("Yaw: ");      Serial.print(dataRX.yaw);       Serial.print("    ");
    Serial.print("Pitch: ");    Serial.print(dataRX.pitch);     Serial.print("    ");
    Serial.print("Roll: ");     Serial.print(dataRX.roll);      Serial.print("    ");
    Serial.print("SWX1: ");     Serial.print(dataRX.SWX1);      Serial.print("    ");
    Serial.print("SWX2: ");     Serial.print(dataRX.SWX2);      Serial.print("    ");
    Serial.print("DrOn: ");     Serial.print(dataRX.DrOn);      Serial.print("\n");
  }

/**************************************************/

void loop()
{
recvData();
unsigned long now = millis();
//Here we check if we've lost signal, if we did we reset the values 
if ( now - lastRecvTime > 1000 ) {
// Signal lost?
resetData();
}

pwm_width_2 = map(dataRX.throttle, 0, 255, 0, 2000);     //PWM value on digital pin D2 1000 --> 2000
pwm_width_3 = map(dataRX.yaw,      0, 255, 0, 2000);     //PWM value on digital pin D3
pwm_width_4 = map(dataRX.pitch,    0, 255, 0, 2000);     //PWM value on digital pin D4
pwm_width_5 = map(dataRX.roll,     0, 255, 0, 2000);     //PWM value on digital pin D5

    Serial.print("Throttle: "); Serial.print(pwm_width_2);  Serial.print("    ");
    Serial.print("Yaw: ");      Serial.print(pwm_width_3);       Serial.print("    ");
    Serial.print("Pitch: ");    Serial.print(pwm_width_4);     Serial.print("    ");
    Serial.print("Roll: ");     Serial.print(pwm_width_5);      Serial.print("\n");


//Now we write the PWM signal using the servo function
PWM2.writeMicroseconds(pwm_width_2);
PWM3.writeMicroseconds(pwm_width_3);
PWM4.writeMicroseconds(pwm_width_4);
PWM5.writeMicroseconds(pwm_width_5);

ShowTramWifi();

delay(500);
}//Loop end
/**************************************************/

