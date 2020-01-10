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
bool SendValue(Clt& client_current, String angle_target_str) {
  String command = "";
  // pre treatment
  if (client_current.c_name == "HAND RIGHT" )
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
    //SendPacket(envoie_pkt, client_current.ip, client_current.port);
    return true;
  }
  // error
  return false;
}


void Move1()//choreography  Move1
{
  char pck_char[255];
  pck_char[0] = '4';
  pck_char[1] = '0';

  SendPacket(pck_char, client5.ip, client5.port);//client 5: the left shoulder should move by 20 degrees
  SendPacket(pck_char, client6.ip, client6.port);//client 6: the left omoplate should move by 20 degrees
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
        case '1' :
          success = SendValue(client1, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, client1.ip, client1.port);
          }
          break;
        case '2' :
          success = SendValue(client2, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, client2.ip, client2.port);
          }
          break;
        case '3' :
          success = SendValue(client3, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, client3.ip, client3.port);
          }
          break;
        case '4' :
          success = SendValue(client4, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, client4.ip, client4.port);
          }
          break;
        case '5' :
          success = SendValue(client5, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, client5.ip, client5.port);
          }
          break;
        case '6' :
          success = SendValue(client6, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, client6.ip, client6.port);
          }
        case '7' :
          success = SendValue(client7, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, client7.ip, client7.port);
          }
          break;
        case '8':
          success = SendValue(client8, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, client8.ip, client8.port);
          }
          break;
        case '9':
          success = SendValue(client9, envoie_angle);
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
    else{
      Move1();
    }
  }
}
