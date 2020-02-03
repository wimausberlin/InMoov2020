



  int fsrPin = 0; // 
  int fsrReading; // 
  
  void setup(void)
  {
    // Envoyons des information de débogage sur le port série
    Serial.begin(9600);
  }
  void Move1(String envoie_angle)//choreography  Move1 
{
  if ( envoie_angle == "move1")
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
