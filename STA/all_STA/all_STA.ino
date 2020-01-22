#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Servo.h>
#define BUFFER_SIZE 255

//WiFi Connection
const char* STA_ssid = "ESPsoftAP";
const char* STA_password = "ESP8266Inmoov";

int angle_Current; //

char buffer[BUFFER_SIZE];
char incomingPacket[BUFFER_SIZE];  // buffer for incoming packets
int len = 0;
int angle_Suivant = 0;

//Servomoteur
int pin_servo = 16;//Pin D0 de l'ESP8266
//Client Structure
struct Clt {  
  String c_name;
  IPAddress ip;
  IPAddress gateway;
  int port;
  int angle_Center;
};


//List of clients

Clt client1{"SHOULDER RIGHT",IPAddress(172, 20, 10, 121), IPAddress(172, 20, 10, 9), 4321, 10};//CHECK
Clt client2{"OMOPLATE RIGHT",IPAddress(172, 20, 10, 122), IPAddress(172, 20, 10, 9), 4321, 30};//CHECK
Clt client3{"ROTATE RIGHT",IPAddress(172, 20, 10, 123), IPAddress(172, 20, 10, 9), 4321, 100};//CHECK
Clt client4{"BICEPS RIGHT",IPAddress(172, 20, 10, 124), IPAddress(172, 20, 10, 9), 4321, 120};    //VALEURS A VERIFIER
Clt client5{"HEAD Y",IPAddress(172, 20, 10, 125), IPAddress(172, 20, 10, 9), 4321, 130};//CHECK
Clt client6{"HEAD Z",IPAddress(172, 20, 10, 126), IPAddress(172, 20, 10, 9), 4321, 102};//CHECK
Clt client7{"HAND RIGHT", IPAddress(172, 20, 10, 127), IPAddress(172, 20, 10, 9), 4321, 0};        //HAND RIGHT   //VALEURS A VERIFIER
Clt client8{"HAND LEFT", IPAddress(172, 20, 10, 128), IPAddress(172, 20, 10, 9), 4321, 0};         //HAND LEFT    //VALEURS A VERIFIER
Clt client9{"WRIST RIGHT", IPAddress(172, 20, 10, 129), IPAddress(172, 20, 10, 9), 4321, 0};    //VALEURS A VERIFIER
Clt clientA{"WAIST", IPAddress(172, 20, 10, 130), IPAddress(172, 20, 10, 9), 4321, 85};//CHECK
Clt clientB{"SPINE", IPAddress(172, 20, 10, 131), IPAddress(172, 20, 10, 9), 4321, 80};//CHECK
Clt client_USED{clientA.c_name, clientA.ip, clientA.gateway, clientA.port, clientA.angle_Center}; //client actuel utilisé dans le programme
 

IPAddress subnet(255, 255, 255, 0);
WiFiUDP udp;
Servo servo;

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


void setup() {
  Serial.begin(115200);
  servo.attach(pin_servo);
  servo.write(client_USED.angle_Center); //Reset all servos to their center position when power is turned on
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STA_ssid, STA_password);
  Serial.print("Connecting to EPS8266 to ");
  Serial.print(STA_ssid);
  isDisconnected();
  WiFi.config(client_USED.ip, client_USED.gateway, subnet);
  udp.begin(client_USED.port);
}

int ReadPacket() {
  int packetSize = udp.parsePacket();
  int angle_Suivant = angle_Current ;
  if (packetSize)
  {
    // receive incoming UDP packets
    len = udp.read(incomingPacket, BUFFER_SIZE);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    String donnee = String(incomingPacket);
    //Etudier les différents traitements de données pour la main et les doigts + les autres moteurs
     
    angle_Suivant = donnee.toInt();
    
    return angle_Suivant;
  }
}

  void servoMove(int angle_Suivant, int angle_Current ) //Adjust the rotation speed by changing the delay
  {
    if (angle_Suivant < angle_Current)
    {
      for (int index = angle_Current; index >= angle_Suivant; index--)
      {
        servo.write(client_USED.angle_Center + index);
        delay(20);
      }
    }
    else
    {
      for (int index = angle_Current; index <= angle_Suivant ; index++)
      {
        servo.write(client_USED.angle_Center + index);
        delay(20);
      }
    }
    angle_Current = angle_Suivant;
  }


  /*----------------------
        MAIN LOOP
    ---------------------*/
  void loop() {
    isDisconnected();
    
    angle_Suivant = ReadPacket();
    if (angle_Suivant != angle_Current)      //CORRIGER LA COMMANDE QUAND ON ENTRE 0 = POSITION INITIALE  
    {
      servoMove(angle_Suivant, angle_Current);
    }
  }
