#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Servo.h>
#define BUFFER_SIZE 255

//WiFi Connection
const char* STA_ssid = "ESPsoftAP";
const char* STA_password = "ESP8266Inmoov";


char buffer[BUFFER_SIZE];
char incomingPacket[BUFFER_SIZE];  // buffer for incoming packets
int len = 0;
int angle_Current; //
int angle_Next = 0;

Servo servo_T;
Servo servo_I;
Servo servo_M;
Servo servo_R;
Servo servo_P;

//Finger Structure
struct Doigt {
  String d_name;
  Servo servo;
  int pin_servo;
  int angle_min;
  int angle_Center;
  int angle_max;
};
// List of fingers
Doigt THUMB{"THUMB", servo_T, 16, 0, 0, 145};     //16 = Pin D0 de l'ESP8266
Doigt INDEX{"INDEX", servo_I, 5, 0, 0, 150};      //5 = Pin D1 de l'ESP8266
Doigt MID_FING{"MID_FING", servo_M, 4, 0, 0, 150};   //4 = Pin D2 de l'ESP8266
Doigt RING{"RING", servo_R, 0, 0, 0, 130};       //0 = Pin D3 de l'ESP8266
Doigt PINKY{"PINKY", servo_P, 2, 0, 0, 150};      //2 = Pin D4 de l'ESP8266


//Client Structure
struct Clt {
  String c_name;
  IPAddress ip;
  IPAddress gateway;
  int port;
  int angle_Center;
};
//List of clients
Clt client7{"HAND RIGHT", IPAddress(172, 20, 10, 127), IPAddress(172, 20, 10, 9), 4321, 0};          //HAND RIGHT
//Clt client8{"HAND LEFT", IPAddress(172, 20, 10, 128), IPAddress(172, 20, 10, 9), 4321, 0};            //HAND LEFT


IPAddress subnet(255, 255, 255, 0);
WiFiUDP udp;
Servo servoCurrent;


//See if the ESP is connected to the network
void isDisconnected()
{
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.print('.');
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
}

//Initialiser les doigts dans leur position neutre (tendue)
void Initialize()
{
  int angle_min = client7.angle_Center;
  THUMB.servo.write(angle_min);
  INDEX.servo.write(angle_min);
  MID_FING.servo.write(angle_min);
  RING.servo.write(angle_min);
  PINKY.servo.write(angle_min);
}


void setup() {
  Serial.begin(115200);
  THUMB.servo.attach(THUMB.pin_servo);
  INDEX.servo.attach(INDEX.pin_servo);
  MID_FING.servo.attach(MID_FING.pin_servo);
  RING.servo.attach(RING.pin_servo);
  PINKY.servo.attach(PINKY.pin_servo);
  Initialize();
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STA_ssid, STA_password);
  Serial.print("Connecting to EPS8266 to ");
  Serial.print(STA_ssid);
  isDisconnected();
  WiFi.config(client7.ip, client7.gateway, subnet);
  Serial.print(WiFi.localIP());
  udp.begin(client7.port);
}

//Send value to the right servo
bool SendValue(Doigt& doigt_current, int angle_target) {
  if ((doigt_current.angle_min <= angle_target) && (doigt_current.angle_max >= angle_target)) {

    return true;
  }
  //error
  return false;
}

int ReadPacket() {
  int packetSize = udp.parsePacket();
  int angle_Suivant = angle_Current ;
  if (packetSize)
  {
    // receive incoming UDP packets
    len = udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    String donnee = String(incomingPacket);
    angle_Suivant = AngletoInt(donnee);
    //Etudier les différents traitements de données pour la main et les doigts + les autres moteurs
    switch (donnee[0])
    {
      case 'T':
        if (SendValue(THUMB, angle_Suivant)) {
        servoCurrent = THUMB.servo;
      }
      break;
    case 'I':
        if (SendValue(INDEX, angle_Suivant)) {
          servoCurrent = INDEX.servo;
        }
      break;
    case 'M':
        if (SendValue(MID_FING, angle_Suivant)) {
          servoCurrent = MID_FING.servo;
        }
      break;
    case 'R':
        if (SendValue(RING, angle_Suivant)) {
          servoCurrent = RING.servo;
        }
      break;
    case 'P':
        if (SendValue(PINKY, angle_Suivant)) {
          servoCurrent = PINKY.servo;
        }
      break;
    default:
        break;
      }
  return angle_Suivant;
}
}

int AngletoInt(String donnee)
{
  int angle = donnee.substring(2).toInt();
  return angle;
}


void servoMove(int angle_Suivant, int angle_Current, Servo servo )
{
  if (angle_Suivant < angle_Current)
  {
    for (int index = angle_Current; index >= angle_Suivant; index--)
    {
      servo.write(client7.angle_Center + index);
      delay(15);
    }
  }
  else
  {
    for (int index = angle_Current; index <= angle_Suivant ; index++)
    {
      servo.write(client7.angle_Center + index);
      delay(15);
    }
  }
  angle_Current = angle_Suivant;
}


/*----------------------
      MAIN LOOP
  ---------------------*/
void loop() {
  isDisconnected();
  angle_Next = ReadPacket();
  if (angle_Next != angle_Current)      //CORRIGER LA COMMANDE QUAND ON ENTRE 0 = POSITION INITIALE
  {
    servoMove(angle_Next, angle_Current, servoCurrent);
  }
}
