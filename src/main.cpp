#include <Arduino.h>

#include "SoftwareSerial.h"      // pour les communications series avec le DFplayer
#include "DFRobotDFPlayerMini.h" // bibliotheque pour le DFPlayer

// PIN qui serviront pour la communication série sur le WEMOS
SoftwareSerial mySoftwareSerial(14, 12); // RX, TX ( wemos D2,D8 ou 4,15 GPIO )  ou Tx,RX ( Dfplayer )
DFRobotDFPlayerMini myDFPlayer;        // init player

const int buttonPin = 16;

void setup()
{
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);

  if (!myDFPlayer.begin(mySoftwareSerial))
  { // Utilisation de  softwareSerial pour communiquer
    Serial.println(F("Pb communication:"));
    Serial.println(F("1.SVP verifier connexion serie!"));
    Serial.println(F("2.SVP verifier SDcard !"));
    while (true)
      ;
  }

  Serial.println(F("DFPlayer Mini En ligne !"));

  myDFPlayer.setTimeOut(500); // Définit un temps de time out sur la communication série à 500 ms

  //----Controle volume----
  myDFPlayer.volume(15); // ( valeur de 0 à 30 )
  // ---- indique d'utiliser le player de carte SD interne
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);

  // optionel , permet d'afficher quelques infos
  Serial.println(myDFPlayer.readFileCounts());        // Le nombre total de fichier mp3 sur la carte ( dossier inclus )
  Serial.println(myDFPlayer.readCurrentFileNumber()); // l'index courant
}

void loop()
{
  delay(100);

  if(digitalRead(buttonPin) == HIGH){
    int randomNumber = random(1,4);
    myDFPlayer.play(randomNumber);
  }
}