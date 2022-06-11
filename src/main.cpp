#include <Arduino.h>

#include "SoftwareSerial.h"      // pour les communications series avec le DFplayer
#include "DFRobotDFPlayerMini.h" // bibliotheque pour le DFPlayer
#include <ESP8266WiFi.h>         // Le wifi ou quoi

const char *ssid = "Wii-Fit";
const char *password = "Gnocking1234";

// PIN qui serviront pour la communication série sur le WEMOS
SoftwareSerial mySoftwareSerial(14, 12); // RX, TX ( wemos D2,D8 ou 4,15 GPIO )  ou Tx,RX ( Dfplayer )
DFRobotDFPlayerMini myDFPlayer;          // init player

const int buttonPin = 16;
const int ledPin = 5;

const int R_PIN = 4;
const int G_PIN = 0;
const int B_PIN = 2;

bool isPressed = false;
bool isOn = false;

WiFiServer server(80);

void toggleLed()
{
  if (isOn)
  {
    digitalWrite(ledPin, HIGH);
    isOn = false;
  }
  else
  {
    digitalWrite(ledPin, LOW);
    isOn = true;
  }
}

void setRGBLight(int red, int green, int blue){
  analogWrite(R_PIN, red);
  analogWrite(G_PIN, green);
  analogWrite(B_PIN, blue);
}

void setup()
{
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);

  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);

  // Verification du lecteur MP3
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

  // Connection au wifi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    toggleLed();
  }

  Serial.println("Connecté !");

  // Démmarage du serveur
  server.begin();
  Serial.println("Serveur ok !");

  Serial.print("Utilisez cette adresse pour la connexion :");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop()
{
  WiFiClient client;

  if (digitalRead(buttonPin) == HIGH && !isPressed)
  {
    int randomNumber = random(1, 5);
    myDFPlayer.play(randomNumber);

    isPressed = true;
  }
  else if (digitalRead(buttonPin) == LOW && isPressed)
  {
    isPressed = false;
  }

  // Vérification si le client est connecté.
  client = server.available();
  if (!client)
  {
    return;
  }

  // Attendre si le client envoie des données ...
  Serial.println("nouveau client");
  while (!client.available())
  {
    delay(1);
  }

  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  int value = LOW;
  if (request.indexOf("/BTN=ON") != -1)
  {
    value = HIGH;

    int randomNumber = random(1, 5);
    myDFPlayer.play(randomNumber);
  }
  if (request.indexOf("/BTN=OFF") != -1)
  {
    value = LOW;

    int randomNumber = random(1, 5);
    myDFPlayer.play(randomNumber);
  }

  // Réponse
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<h1>Arr ?</h1>");
  client.println("<br><br>");
  if (value == HIGH)
  {
    // client.print("On");
    client.println("<a href=\"/BTN=OFF\"\"><button>ARR </button></a>");
  }
  else
  {
    // client.print("Off");
    client.println("<a href=\"/BTN=ON\"\"><button>ARR </button></a>");
  }
  client.println("</html>");

  delay(1);
  Serial.println("Client deconnecté");
  Serial.println("");

  // digitalWrite(ledPin, HIGH);
}