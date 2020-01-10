#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Servo.h>
#define BUFFER_SIZE 255

//WiFi Connection
const char* STA_ssid = "ESPsoftAP";
const char* STA_password = "ESP8266Inmoov";

int angle_Current; //

char buffer[BUFFER_SIZE];
char incomingPacket[255];  // buffer for incoming packets
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
Clt client1{"SHOULDER LEFT",IPAddress(172, 20, 10, 121), IPAddress(172, 20, 10, 9), 4321, 87};
Clt client2{"OMOPLATE LEFT",IPAddress(172, 20, 10, 122), IPAddress(172, 20, 10, 9), 4321, 5};
Clt client3{"ROTATE LEFT",IPAddress(172, 20, 10, 123), IPAddress(172, 20, 10, 9), 4321, 100};
Clt client4{"BICEPS LEFT",IPAddress(172, 20, 10, 124), IPAddress(172, 20, 10, 9), 4321, 120};
Clt client5{"HEAD Y",IPAddress(172, 20, 10, 125), IPAddress(172, 20, 10, 9), 4321, 130};
Clt client6{"HEAD X",IPAddress(172, 20, 10, 126), IPAddress(172, 20, 10, 9), 4321, 102};
Clt client7{"HAND RIGHT", IPAddress(172, 20, 10, 127), 4321, 0};            //HAND RIGHT
Clt client8{"HAND LEFT", IPAddress(172, 20, 10, 128), 4321, 0};             //HAND LEFT

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
  servo.write(client6.angle_Center);
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STA_ssid, STA_password);
  Serial.print("Connecting to EPS8266 to ");
  Serial.print(STA_ssid);
  isDisconnected();
  WiFi.config(client6.ip, client6.gateway, subnet);
  udp.begin(client6.port);
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
    //Etudier les différents traitements de données pour la main et les doigts + les autres moteurs
     
    angle_Suivant = donnee.toInt();
    
    return angle_Suivant;
  }
}

  void servoMove(int angle_Suivant, int angle_Current )
  {
    if (angle_Suivant < angle_Current)
    {
      for (int index = angle_Current; index >= angle_Suivant; index--)
      {
        servo.write(client6.angle_Center + index);
        delay(15);
      }
    }
    else
    {
      for (int index = angle_Current; index <= angle_Suivant ; index++)
      {
        servo.write(client6.angle_Center + index);
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
    angle_Suivant = ReadPacket();
    if (angle_Suivant != angle_Current)      //CORRIGER LA COMMANDE QUAND ON ENTRE 0 = POSITION INITIALE  
    {
      servoMove(angle_Suivant, angle_Current);
    }
  }
