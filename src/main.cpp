#include <Arduino.h>

#include "SoftwareSerial.h"      // pour les communications series avec le DFplayer
#include "DFRobotDFPlayerMini.h" // bibliotheque pour le DFPlayer
#include <ESP8266WiFi.h>         // Le wifi ou quoi

const char *ssid = "CC C T-O";
const char *password = "gnocchis";

// PIN qui serviront pour la communication série sur le WEMOS
SoftwareSerial mySoftwareSerial(14, 12); // RX, TX ( wemos D2,D8 ou 4,15 GPIO )  ou Tx,RX ( Dfplayer )
DFRobotDFPlayerMini myDFPlayer;          // init player

const int buttonPin = 16;

const int B_PIN = 4;
const int G_PIN = 0;
const int R_PIN = 2;

bool isPressed = false;
bool isOn = false;

int retries = 0;
bool wifiConnected = false;

int maxIndex = 0;

WiFiServer server(80);

void setRGBLight(int red, int green, int blue)
{
  analogWrite(R_PIN, red);
  analogWrite(G_PIN, green);
  analogWrite(B_PIN, blue);
}

void toggleLed(int red, int green, int blue)
{
  if (isOn)
  {
    setRGBLight(red, green, blue);
    isOn = false;
  }
  else
  {
    setRGBLight(0, 0, 0);
    isOn = true;
  }
}

void setup()
{
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);

  pinMode(buttonPin, INPUT);

  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);

  setRGBLight(127, 127, 127);

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

  maxIndex = myDFPlayer.readFileCounts() + 1;

  myDFPlayer.play(1);

  // Connection au wifi
  setRGBLight(0, 0, 255);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED && retries < 30)
  {
    delay(500);
    toggleLed(0, 0, 255);
    Serial.print(".");

    retries++;
  }

  if (retries < 30)
  {
    wifiConnected = true;

    Serial.println("Connecté !");

    // Démmarage du serveur
    server.begin();
    Serial.println("Serveur ok !");

    Serial.print("Utilisez cette adresse pour la connexion :");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");

    setRGBLight(0, 255, 0);
  }
  else
  {
    Serial.println("Impossible de se connecter au Wifi :(");
    setRGBLight(255, 0, 0);
  }
}

void loop()
{
  if (digitalRead(buttonPin) == HIGH && !isPressed)
  {
    int randomNumber = random(2, maxIndex);
    myDFPlayer.play(randomNumber);

    isPressed = true;
  }
  else if (digitalRead(buttonPin) == LOW && isPressed)
  {
    isPressed = false;
  }

  if (wifiConnected)
  {
    WiFiClient client;

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

      int randomNumber = random(2, maxIndex);
      myDFPlayer.play(randomNumber);
    }
    if (request.indexOf("/BTN=OFF") != -1)
    {
      value = LOW;

      int randomNumber = random(2, maxIndex);
      myDFPlayer.play(randomNumber);
    }

    // Réponse
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<head>");
    client.println("<meta charset=\"UTF-8\" />");
    client.println("<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\" />");
    client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />");
    client.println("<link rel=\"preconnect\" href=\"https://fonts.googleapis.com\" />");
    client.println("<link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin />");
    client.println("<link href=\"https://fonts.googleapis.com/css2?family=Fredericka+the+Great&display=swap\" rel=\"stylesheet\"/>");
    client.println("<title>ARR</title>");
    client.println("<style>");
    client.println("body {");
    client.println("font-family: \"Fredericka the Great\", cursive;");
    client.println("margin: 0;");
    client.println("width: 100%;");
    client.println("height: 100vh;");
    client.println("display: flex;");
    client.println("flex-direction: column;");
    client.println("justify-content: center;");
    client.println("align-items: center;");
    client.println("align-content: center;");
    client.println("background-image: url(https://cdn.discordapp.com/attachments/455791465734602782/985571028841431151/motif.png);");
    client.println("background-repeat: repeat;");
    client.println("}");
    client.println("h1 {");
    client.println("font-size: 120px;");
    client.println("font-weight: normal;");
    client.println("margin-top: 0;");
    client.println("color: rgb(78, 67, 56);");
    client.println("}");
    client.println("button {");
    client.println("font-family: \"Fredericka the Great\", cursive;");
    client.println("font-size: 40px;");
    client.println("padding: 10px 40px;");
    client.println("border: solid rgb(80, 69, 58) 3px;");
    client.println("background: none;");
    client.println("color: rgb(58, 50, 41);");
    client.println("font-weight: bold;");
    client.println("transform: rotate(-4deg);");
    client.println("transition: 0.3s;");
    client.println("}");
    client.println("button:hover {");
    client.println("cursor: pointer;");
    client.println("transform: scale(0.99) rotate(-5deg);");
    client.println("}");
    client.println("button:active {");
    client.println("transform: scale(0.97) rotate(-2deg);");
    client.println("}");
    client.println("div {");
    client.println("position: fixed;");
    client.println("width: 100%;");
    client.println("height: 100%;");
    client.println("display: flex;");
    client.println("flex-direction: column;");
    client.println("justify-content: center;");
    client.println("align-items: center;");
    client.println("align-content: center;");
    client.println("background-image: url(https://cdn.discordapp.com/attachments/455791465734602782/985571029038534716/ARR.png);");
    client.println("background-repeat: no-repeat;");
    client.println("background-size: contain;");
    client.println("background-position: center;");
    client.println("filter: sepia(0.3);");
    client.println("opacity: 0.3;");
    client.println("z-index: -1;");
    client.println("}");
    client.println("@media screen and (max-width: 480px) {");
    client.println("h1 {");
    client.println("font-size: 80px;");
    client.println("}");
    client.println("button {");
    client.println("border: solid rgb(80, 69, 58) 2px;");
    client.println("font-size: 32px;");
    client.println("}");
    client.println("div {");
    client.println("background-size: cover;");
    client.println("}");
    client.println("}");
    client.println("</style>");
    client.println("</head>");
    client.println("<body>");
    client.println("<h1>Arr ?</h1>");
    client.println("<div></div>");

    if (value == HIGH)
    {
      // client.print("On");
      client.println("<a href=\"/BTN=OFF\"\"><button>ARR</button></a>");
    }
    else
    {
      // client.print("Off");
      client.println("<a href=\"/BTN=ON\"\"><button>ARR</button></a>");
    }
    client.println("</body>");
    client.println("</html>");

    delay(1);
    Serial.println("Client deconnecté");
    Serial.println("");
  }
}