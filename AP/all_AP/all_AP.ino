#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define BUFFER_SIZE 255 //size of the buffer

//Client Structure
struct Clt {
  String c_name;
  IPAddress ip;
  int port;
  int angle_min;
  int angle_Center;
  int angle_max;
};
//List of clients
//Modifier les limites des angles pour chaque client

Clt client0{"WAIST", IPAddress(172, 20, 10, 120), 4321, 45, 90, 135};  //(-45) - neutre(0) - (+45)
Clt client1{"SHOULDER LEFT", IPAddress(172, 20, 10, 121), 4321, 65, 87, 160}; //VALEURS A VERIFIER //Arrière - neutre - Avant
Clt client2{"OMOPLATE LEFT", IPAddress(172, 20, 10, 122), 4321, 0, 5, 45}; //Collé au corps(-5) - neutre(0) - Levé côté(+40)
Clt client3{"ROTATE LEFT", IPAddress(172, 20, 10, 123), 4321, 80, 100, 140}; //VALEURS A VERIFIER
Clt client4{"BICEPS LEFT", IPAddress(172, 20, 10, 124), 4321, 20, 50, 102}; //VALEURS A VERIFIER
Clt client5{"HEAD Y", IPAddress(172, 20, 10, 125), 4321, 60, 130, 180};   //Menton en bas(-70) - neutre(0) - Lève la tête(+50)
Clt client6{"HEAD X", IPAddress(172, 20, 10, 126), 4321, 50, 102, 160};   //A droite(-52) - neutre(0) - A gauche(+58)
Clt client7{"HAND RIGHT", IPAddress(172, 20, 10, 127), 4321, 0, 0, 150};          //HAND RIGHT
Clt client8{"HAND LEFT", IPAddress(172, 20, 10, 128), 4321, 0, 0, 150};             //HAND LEFT
Clt client9{"WRIST LEFT", IPAddress(172, 20, 10, 129), 4321, 0, 0, 130};

//WiFi Connection
//const char* STA_ssid = "arduino1";
//const char* STA_password = "wim123456";
const char* STA_ssid = "Oppo Bastien";
const char* STA_password = "5T4N L33";

const char* AP_ssid = "ESPsoftAP";
const char* AP_password = "ESP8266Inmoov";

String envoie_angle = ""; // Initialised to nothing.
int angle = 0;           //Initialised to nothing.

char envoie_pkt[255]; // The char array who will be send

//Different IP Address for the Station (STA) and the Acces Point (AP)
IPAddress STA_ip(192, 168, 4, 22);
IPAddress STA_gateway(192, 168, 4, 9);
IPAddress AP_ip(172, 20, 10, 22);
IPAddress AP_gateway(172, 20, 10, 9);
IPAddress subnet(255, 255, 255, 0);


//Define the User Datagram Protocol
WiFiUDP udp;

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
  delay(5000);
  pinMode(LED_BUILTIN, OUTPUT);

  /*----------------------
    Config STA
    ---------------------*/
  WiFi.begin(STA_ssid, STA_password);
  Serial.print("Connecting to EPS8266 to ");
  Serial.print(STA_ssid);
  isDisconnected();

  WiFi.config(STA_ip, STA_gateway, subnet);
  Serial.println('\n');
  Serial.print("IP adress STA:\t");
  Serial.println(WiFi.localIP());
  delay(500);
  /*----------------------
    Config AP
    ---------------------*/
  WiFi.softAP(AP_ssid, AP_password);
  WiFi.softAPConfig(AP_ip, AP_gateway, subnet);
  Serial.println("\nSetting AP\t");
  Serial.print(AP_ssid);
  Serial.print('\n');
  Serial.println("IP adress AP:\t");
  Serial.println(WiFi.softAPIP());
  Serial.println('\n');
  WiFi.printDiag(Serial);
}

//send pkg to the corresponding client
void SendPacket(const char content[], IPAddress ip, int port)
{
  udp.beginPacket(ip, port);
  udp.write(content);
  udp.endPacket();
}


// send value to client
bool CheckValue(Clt& client_current, String angle_target_str) {
  String command = "";
  // pre treatment
  if (client_current.c_name == "HAND RIGHT" || client_current.c_name == "HAND LEFT")
  {
    command = angle_target_str.substring(3);
    angle_target_str = angle_target_str.substring(5);
  }
  else
  {
    angle_target_str = angle_target_str.substring(3);
    command = angle_target_str;
  }
  int angle_target = client_current.angle_Center + angle_target_str.toInt();
  // print
  Serial.print("client:");
  Serial.println(client_current.c_name);
  Serial.print(" target: ");
  Serial.println(angle_target);
  // check
  if ((client_current.angle_min <= angle_target) && (client_current.angle_max >= angle_target)) {
    command.toCharArray(envoie_pkt, BUFFER_SIZE);
    
    return true;
  }
  // error
  return false;
}

char* ConvertToPacket(int angle) {   //Convert the angle given into a char array packet
  char*  pck_char  = (char*) malloc(BUFFER_SIZE);
  String angle_str = String(angle);
  if ((0 <= angle && angle < 10) || (0 >= angle && angle > -10)) {   // 0 <= angle < 10  ; -10 < angle <= 0
    pck_char[0] = angle_str[0];
  }
  if ((10 <= angle && angle < 100) || (-10 >= angle && angle > -100)) {   // 10 <= angle < 100  ; -100 < angle <= -10
    pck_char[0] = angle_str[0];
    pck_char[1] = angle_str[1];
  }
  if ((100 <= angle && angle <= 180) || (-100 >= angle && angle >= -180)) {   // 100 <= angle <= 180  ; -180 <= angle <= -100
    pck_char[0] = angle_str[0];
    pck_char[1] = angle_str[1];
    pck_char[2] = angle_str[2];
  }
  return pck_char;
}

void Move1()//choreography  Move1
{
  char* pck_char;
  pck_char = ConvertToPacket(40);

  SendPacket(pck_char, client0.ip, client0.port);//client 0: the left shoulder should move by 20 degrees
  //SendPacket(pck_char, client6.ip, client6.port);//client 6: the left omoplate should move by 20 degrees
}



void loop() {
  isDisconnected();

  if (Serial.available() > 0)
  {
    envoie_angle = Serial.readString(); //should be 'C1_30' to indicate that the master ESP send to the slave/client1 30
    if (envoie_angle[0] == 'C')
    {
      bool success = false;
      switch (envoie_angle[1])
      {
        case '0':
          success = CheckValue(client0, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, client0.ip, client0.port);
          }
        case '1' :
          success = CheckValue(client1, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, client1.ip, client1.port);
          }
          break;
        case '2' :
          success = CheckValue(client2, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, client2.ip, client2.port);
          }
          break;
        case '3' :
          success = CheckValue(client3, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, client3.ip, client3.port);
          }
          break;
        case '4' :
          success = CheckValue(client4, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, client4.ip, client4.port);
          }
          break;
        case '5' :
          success = CheckValue(client5, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, client5.ip, client5.port);
          }
          break;
        case '6' :
          success = CheckValue(client6, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, client6.ip, client6.port);
          }
        case '7' :
          success = CheckValue(client7, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, client7.ip, client7.port);
          }
          break;
        case '8':
          success = CheckValue(client8, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, client8.ip, client8.port);
          }
          break;
        case '9':
          success = CheckValue(client9, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, client9.ip, client9.port);
          }
          break;
        default:
          break;
      }

      if (success) {
        Serial.println("success: value sent");
        Serial.println();
      } else {
        Serial.println("error: wrong value");
        Serial.println();
      }
    }
    else {
      Move1();
    }
  }
}
