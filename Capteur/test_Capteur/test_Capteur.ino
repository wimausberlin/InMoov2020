



  int fsrPin = 0; // 
  int fsrReading; // 

  
Clt client1{"SHOULDER LEFT", IPAddress(172, 20, 10, 121), 4321, 65, 87, 160}; //VALEURS A VERIFIER //Arrière - neutre - Avant
Clt client2{"OMOPLATE LEFT", IPAddress(172, 20, 10, 122), 4321, 0, 5, 45}; //Collé au corps(-5) - neutre(0) - Levé côté(+40)
Clt client3{"ROTATE LEFT", IPAddress(172, 20, 10, 123), 4321, 80, 100, 140}; //VALEURS A VERIFIER
Clt client4{"BICEPS LEFT", IPAddress(172, 20, 10, 124), 4321, 20, 50, 102}; //VALEURS A VERIFIER
Clt client5{"HEAD Y", IPAddress(172, 20, 10, 125), 4321, 60, 130, 180};   //Menton en bas(-70) - neutre(0) - Lève la tête(+50)
Clt client6{"HEAD X", IPAddress(172, 20, 10, 126), 4321, 50, 102, 160};   //A droite(-52) - neutre(0) - A gauche(+58)
Clt client7{"HAND RIGHT", IPAddress(172, 20, 10, 127), 4321, 0, 0, 150};          //HAND RIGHT
  void setup(void)
  {
    // Envoyons des information de débogage sur le port série
    Serial.begin(9600);
  }
  void Handshake0(String envoie_angle)//choreography  Handshake0 with the open hand
{
  if ( envoie_angle == "Handshake0")
  {
    char [] pck_char = new char[255];
    pck_char[0] = '2';
    pck_char[1] = '0';
    
    SendPacket(pck_char, client1.ip, client1.port);//client 1: the left shoulder should move by 20 degrees
    SendPacket(pck_char, client2.ip, client2.port);//client 2: the left omoplate should move by 20 degrees
  }

}
  void loop(void) 
  
  {
    fsrReading = analogRead(fsrPin);
    Serial.print("Lecture Analogique = ");
    Serial.print(fsrReading); // La valeur brute (valeur analogique lue)

    
    //ideally if the client is shaking  inmoov's hand in a hard way it should be able to tell the person that's doing that 

    // Utilisons quelques seuils, pour faire une détermination qualitative
    if (fsrReading < 10) {
    //Serial.println(" - Pas de pression (No pressure)");  
    
    } else if (fsrReading < 200) {
    Serial.println(" - Touché léger (Light touch)");
    } else if (fsrReading < 500) {
    Serial.println(" - Pression légère (Light squeeze)");
    } else if (fsrReading < 800) {
    Serial.println(" - Pression moyenne (Medium squeeze)");
    } else {
    Serial.println(" - Forte pression (Big squeeze)");
    }
    delay(1000);
  } 
