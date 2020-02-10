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

/*----------------------
     List of clients
  -----------------------*/
Clt client1{"SHOULDER RIGHT", IPAddress(172, 20, 10, 121), 4321, 5, 10, 50}; //Arrière(-5) - neutre(0) - Avant(+40)   //CHECK
Clt client2{"OMOPLATE RIGHT", IPAddress(172, 20, 10, 122), 4321, 30, 30, 70}; //Collé au corps(0) - neutre(0) - Levé côté(+40)    //CHECK
Clt client3{"ROTATE RIGHT", IPAddress(172, 20, 10, 123), 4321, 80, 100, 140}; //Vers l'intérieur(-20) - neutre(0) - vers l'extérieur(+40)  //CHECK
Clt client4{"BICEPS RIGHT", IPAddress(172, 20, 10, 124), 4321, 20, 50, 102}; //VALEURS A VERIFIER
Clt client5{"HEAD Y", IPAddress(172, 20, 10, 125), 4321, 60, 130, 180};   //Menton en bas(-70) - neutre(0) - Lève la tête(+50)    //CHECK
Clt client6{"HEAD Z", IPAddress(172, 20, 10, 126), 4321, 50, 102, 160};   //A droite(-52) - neutre(0) - A gauche(+58)   //CHECK
Clt client7{"HAND RIGHT", IPAddress(172, 20, 10, 127), 4321, 0, 0, 150};          //HAND RIGHT    //VALEURS A VERIFIER
Clt client8{"HAND LEFT", IPAddress(172, 20, 10, 128), 4321, 0, 0, 150};             //HAND LEFT   //VALEURS A VERIFIER
Clt client9{"WRIST RIGHT", IPAddress(172, 20, 10, 129), 4321, 0, 0, 130};   //VALEURS A VERIFIER
Clt clientA{"WAIST", IPAddress(172, 20, 10, 130), 4321, 40, 85, 130};  //vers la droite(-45) - neutre(0) - vers la gauche(+45)  //CHECK
Clt clientB{"SPINE", IPAddress(172, 20, 10, 131), 4321, 55, 80, 105}; //vers la droite(+25) - neutre(0) - vers la gauche(-25)   //CHECK


Clt clientC{"SHOULDER LEFT", IPAddress(172, 20, 10, 132), 4321, 80, 85, 125}; //Arrière(-5) - neutre(0) - Avant(+40)   //CHECK
Clt clientD{"OMOPLATE LEFT", IPAddress(172, 20, 10, 133), 4321, 0, 0, 40}; //Collé au corps(0) - neutre(0) - Levé côté(+40)    //CHECK
Clt clientE{"ROTATE LEFT", IPAddress(172, 20, 10, 134), 4321, 80, 100, 140}; //Vers l'intérieur(-20) - neutre(0) - vers l'extérieur(+40)  //CHECK




//WiFi Connection
//const char* STA_ssid = "arduino1";
//const char* STA_password = "wim123456"; //wim
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
    -----------------------*/
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
    -----------------------*/
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


// Check if the value sent to client is in the client angles'range
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
  Serial.print("target: ");
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


/*-------------------------
        Mouvement Methods
  --------------------------*/

//******************
// All Head moves
//******************
void Move_HEAD(char move_direction) { // U = UP ; D = Down ; R = Right ; L = Left ; 9 = Up Right ; 7 = Up Left ; 3 = Down Right ; 1 = Down Left ; 0 = Reset Position (1~=0)
  char* pck_char_HEAD_Y;                                                            //see arrows on num keyboard (from InMoov's view)
  char* pck_char_HEAD_Z;
  int HEAD_Y_U = 45;
  int HEAD_Y_D = -60;
  int HEAD_Z_R = -50;
  int HEAD_Z_L = 55;

  switch (move_direction) {
    case 'U': //UP
      pck_char_HEAD_Y = ConvertToPacket(HEAD_Y_U);
      pck_char_HEAD_Z = ConvertToPacket(1);
      break;
    case 'D': //DOWN
      pck_char_HEAD_Y = ConvertToPacket(HEAD_Y_D);
      pck_char_HEAD_Z = ConvertToPacket(1);
      break;
    case 'R': //RIGHT
      pck_char_HEAD_Y = ConvertToPacket(1);
      pck_char_HEAD_Z = ConvertToPacket(HEAD_Z_R);
      break;
    case 'L': //LEFT
      pck_char_HEAD_Y = ConvertToPacket(1);
      pck_char_HEAD_Z = ConvertToPacket(HEAD_Z_L);
      break;
    case '9': //UP RIGHT
      pck_char_HEAD_Y = ConvertToPacket(HEAD_Y_U);
      pck_char_HEAD_Z = ConvertToPacket(HEAD_Z_R);
      break;
    case '7': //UP LEFT
      pck_char_HEAD_Y = ConvertToPacket(HEAD_Y_U);
      pck_char_HEAD_Z = ConvertToPacket(HEAD_Z_L);
      break;
    case '3': //DOWN RIGHT
      pck_char_HEAD_Y = ConvertToPacket(HEAD_Y_D);
      pck_char_HEAD_Z = ConvertToPacket(HEAD_Z_R);
      break;
    case '1': //DOWN LEFT
      pck_char_HEAD_Y = ConvertToPacket(HEAD_Y_D);
      pck_char_HEAD_Z = ConvertToPacket(HEAD_Z_L);
      break;
    case '0': //RESET
      pck_char_HEAD_Y = ConvertToPacket(1);
      pck_char_HEAD_Z = ConvertToPacket(1);
      break;
  }
  SendPacket(pck_char_HEAD_Y, client5.ip, client5.port);
  SendPacket(pck_char_HEAD_Z, client6.ip, client6.port);
}


void SHOULDER_MOVE(char side, int angle) {
  if (side == 'R') {
    char* pck_SHOULDER_RIGHT = ConvertToPacket(angle);
    SendPacket(pck_SHOULDER_RIGHT, client1.ip, client1.port);
  }
  if (side == 'L') {
    char* pck_SHOULDER_LEFT = ConvertToPacket(angle);
    SendPacket(pck_SHOULDER_LEFT, clientC.ip, clientC.port);
  }
}

void OMOPLATE_MOVE(char side, int angle) {
  if (side == 'R') {
    char* pck_OMOPLATE_RIGHT = ConvertToPacket(angle);
    SendPacket(pck_OMOPLATE_RIGHT, client2.ip, client2.port);
  }
  if (side == 'L') {
    char* pck_OMOPLATE_LEFT = ConvertToPacket(angle);
    SendPacket(pck_OMOPLATE_LEFT, clientD.ip, clientD.port);
  }
}

void ROTATE_MOVE(char side, int angle) {
  if (side == 'R') {
    char* pck_ROTATE_RIGHT = ConvertToPacket(angle);
    SendPacket(pck_ROTATE_RIGHT, client3.ip, client3.port);
  }
  if (side == 'L') {
    char* pck_ROTATE_LEFT = ConvertToPacket(angle);
    SendPacket(pck_ROTATE_LEFT, clientE.ip, clientE.port);
  }
}

void WAIST_MOVES(int angle) {
  char* pck_WAIST = ConvertToPacket(angle);
  SendPacket(pck_WAIST, clientA.ip, clientA.port);
}

void SPINE_MOVES(int angle) {
  char* pck_SPINE = ConvertToPacket(angle);
  SendPacket(pck_SPINE, clientB.ip, clientB.port);
}

void ARM_MOVES(char side) {
  SHOULDER_MOVE(side, 40);
  OMOPLATE_MOVE(side, 40);
  ROTATE_MOVE(side, 40);
}


void HEAD_MOVES() {
  /* Move_HEAD('L'); //Start Complete circle
    delay(3000);
    Move_HEAD('7');
    delay(3000);
    Move_HEAD('U');
    delay(3000);
    Move_HEAD('9');
    delay(3000);
    Move_HEAD('R');
    delay(3000);
    Move_HEAD('3');
    delay(3000);
    Move_HEAD('D');
    delay(3000);
    Move_HEAD('1');
    delay(3000);
    Move_HEAD('0');  //End Complete circle
  */
  /* delay(5000);
    Move_HEAD('9'); //Start 'X'-Move
    delay(2000);
    Move_HEAD('1');
    delay(2000);
    Move_HEAD('7');
    delay(2000);
    Move_HEAD('3');
    delay(2000);
    Move_HEAD('0');  //End 'X'-Move
  */
  delay(5000);
  Move_HEAD('L'); //Start '+'-Move
  delay(3000);
  Move_HEAD('U');
  delay(3000);
  Move_HEAD('R');
  delay(3000);
  Move_HEAD('D');
  delay(3000);
  Move_HEAD('0'); //End '+'-Move
}





/*----------------------
        MAIN LOOP
    ---------------------*/
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
          break;
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
        case 'A':
          success = CheckValue(clientA, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, clientA.ip, clientA.port);
          }
          break;
        case 'B':
          success = CheckValue(clientB, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, clientB.ip, clientB.port);
          }
          break;
        case 'C':
          success = CheckValue(clientC, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, clientC.ip, clientC.port);
          }
          break;
        case 'D':
          success = CheckValue(clientD, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, clientD.ip, clientD.port);
          }
          break;
        case 'E':
          success = CheckValue(clientE, envoie_angle);
          if (success) {
            SendPacket(envoie_pkt, clientE.ip, clientE.port);
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
      if (envoie_angle[0] == 'H') {
        HEAD_MOVES();
      }
      else {
        if (envoie_angle[0] == 'A') {
          ARM_MOVES('R');
        }
      }
    }
  }
}
