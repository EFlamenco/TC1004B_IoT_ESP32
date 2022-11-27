//Se incluyen librerias para el funcionamiento del programa y los sensores
#include <esp_wpa2.h> //Libreria para el ESP32
#include <WiFi.h> //Libreria para conectar a WIFI
#include <Firebase_ESP_Client.h>  //Libreria para conectarse a Firebase
#include <addons/TokenHelper.h> //Libreria para sacar Token
#include "Arduino.h"
#include "addons/RTDBHelper.h"
#include "DHT.h" //Libreria para el sensor DHT11

//Se decalara la red del tec
const char* ssid = "Tec";
#define EAP_IDENTITY "A01732743@tec.mx"
#define EAP_PASSWORD "Instic$oulz1828"

// Autorización de inserción de Firebase
#define USER_EMAIL "A01732743@tec.mx"
#define USER_PASSWORD "Instic$oulz1828"

//Se define las variables para el sensor de temperatura  DHT11
#define DHTPIN 14 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//Se define las variables para el ciclo (en vez de delay)
long tiempo; //Tiempo que pasa
long tiempoAnterior; //Tiempo anterior al ciclo actual
int dTiempo= 2000; //tiempo que tarda en desplejar datos

//Se define las variables para el joystick
int lecturaX = 0; //Se inicializa en cero
int lecturaY = 0; //Se inicializa en cero
int PINX = 32; //El pin que detecta x
int PINY = 35;  // El pin que detecta y

//Variables para el sensor de sonido KY-038
int MIC = 34; //El pin que detecta el microfono
int sonido ; //Variable en la que se guarda la señal

//Variables para el sensor de proximidad Sensor de Distancia Ultrasónico HC-SR04
int TRIG = 13;  //Pin del que lanza frecuencia
int ECO = 33; //Pin que recibe el sonido
int duracion; //Duración del rebote
int distancia; //Distancia que detecta

// Se declaran los pines del display de 7 segementos g-a
int LEDs[] = {0, 21, 5,23,22,16,19};     // for ESP32 microcontroller
//Se declara el numero que va a leer
String numero;

//se declaran los arreglos que forman los dígitos
int zero[] = {0, 1, 1, 1, 1, 1, 1};   // cero
int one[] = {0, 0, 0, 0, 1, 1, 0};   // uno
int two[] = {1, 0, 1, 1, 0, 1, 1};   // dos
int three[] = {1, 0, 0, 1, 1, 1, 1};   // tres
int four[] = {1, 1, 0, 0, 1, 1, 0};   // cuatro 
int five[] = {1, 1, 0, 1, 1, 0, 1};   // cinco
int six[] = {1, 1, 1, 1, 1, 0, 1};   // seis
int seven[] = {1, 0, 0, 0, 1, 1, 1};   // siete
int eight[] = {1, 1, 1, 1, 1, 1, 1}; // ocho
int nine[] = {1, 1, 0, 1, 1, 1, 1};   // nueve
int otros[] = {1,0,0,0,0,0,0}; //Mayor a nueve

void segment_display(String valor){
    if(valor == "0"){
      for (int i = 0; i<7; i++) digitalWrite(LEDs[i], zero[i]);
    }
    else if(valor == "1"){
      for (int i = 0; i<7; i++) digitalWrite(LEDs[i], one[i]);
    }
    else if(valor == "2"){
      for (int i = 0; i<7; i++) digitalWrite(LEDs[i], two[i]);
    }
    else if(valor == "3"){
      for (int i = 0; i<7; i++) digitalWrite(LEDs[i], three[i]);
    }
    else if(valor == "4"){
      for (int i = 0; i<7; i++) digitalWrite(LEDs[i], four[i]);
    }
    else if(valor == "5"){
      for (int i = 0; i<7; i++) digitalWrite(LEDs[i], five[i]);
    }
    else if(valor == "6"){
      for (int i = 0; i<7; i++) digitalWrite(LEDs[i], six[i]);
    }
    else if(valor == "7"){
      for (int i = 0; i<7; i++) digitalWrite(LEDs[i], seven[i]);
    }
    else if(valor == "8"){
      for (int i = 0; i<7; i++) digitalWrite(LEDs[i], eight[i]);
    }
    else if(valor == "9"){
      for (int i = 0; i<7; i++) digitalWrite(LEDs[i], nine[i]);
    }
    else{
      for (int i = 0; i<7; i++) digitalWrite(LEDs[i], otros[i]);
    }                          
        
}


//Se inserta el API Key de la firebase
#define API_KEY "AIzaSyBdwl6PBpsNMa-6uVwxKVOUqrO2cYlhaEI"//AIzaSyAjjTHMIV0y394tayvijhU-aVVcKdkIZxU

// Se inserta el RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://proyectoesp32-91402-default-rtdb.firebaseio.com/"

//Define Firebase Data object
FirebaseData fbdo; 
FirebaseAuth auth;
FirebaseConfig config;

//Para ver que este bien la base de datos
unsigned long sendDataPrevMillis = 0;
bool signupOK = false;


void setup() {
    //Se inicia en serial
    Serial.begin(115200);
    //Inicializar display 7 segmentos
    for (int i = 0; i<7; i++) pinMode(LEDs[i], OUTPUT);
    //Se actualiza el ciclo
    tiempoAnterior = millis();
    delay(10);

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    // Empieza el acceso protegido inalámbrico
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);   //init wifi mode
    //esp_wifi_set_mac(ESP_IF_WIFI_STA, &masterCustomMac[0]);
    Serial.print("LAP >> ");
    Serial.println(WiFi.macAddress());
    Serial.printf("Connecting to WiFi: %s ", ssid);
    //esp_wifi_sta_wpa2_ent_set_ca_cert((uint8_t *)incommon_ca, strlen(incommon_ca) + 1);
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
    //esp_wpa2_config_t configW = WPA2_CONFIG_INIT_DEFAULT();
    //esp_wifi_sta_wpa2_ent_enable(&configW);
    esp_wifi_sta_wpa2_ent_enable();
    // WPA2 enterprise magic ends here
    WiFi.begin(ssid);
    

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    //Se pone la api key de la base de datos de firebase
    config.api_key = API_KEY;

    //Se pone la url de la base de datos de Firebsae
    config.database_url = DATABASE_URL;

    //Se resgistra a la base de datos de Firebase
    if (Firebase.signUp(&config, &auth, "", "")){
      Serial.println("ok");
      signupOK = true;
    }
    else{
      Serial.printf("%s\n", config.signer.signupError.message.c_str());
    }

    /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

    //Se inicia la base de datos y se conecta a internet
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    Serial.begin(115200);
    Serial.println(F("Han empezado todos los sensores"));
    dht.begin();

    //Para el sensor de proximidad HC-SR04
    pinMode(TRIG,OUTPUT);
    pinMode(ECO,INPUT);

    
}


void loop() {

  //Detecta el sonido más alto (nunca se usa delay)  KY-038
  if (sonido < analogRead(MIC)){
    sonido = analogRead(MIC);
  }
  //Se entra al ciclo del tiempo, cada dTiempo va a ser complido el loop
  if(tiempoAnterior - tiempo > dTiempo ){
    //LECTURA DE LAS VARIABLES 
    //Sensor de temperatura (DHT11)
    // Lee la humedad
    float h = dht.readHumidity();
    // Lee la temperautra en Celsius
    float t = dht.readTemperature();
    // Lee la temperatura en Fahrenheit 
    float f = dht.readTemperature(true);
    //  Pasa la humedad a Fahrenheit
    float hif = dht.computeHeatIndex(f, h);
    // Pasa el index en celsius
    float hic = dht.computeHeatIndex(t, h, false);

    //Joystick 
    lecturaX = analogRead(PINX);
    lecturaY = analogRead(PINY);

    //Sensor de proximidad Sensor de Distancia Ultrasónico HC-SR04
    digitalWrite(TRIG, HIGH);
    delay(1);
    digitalWrite(TRIG, LOW);
    duracion = pulseIn(ECO,HIGH);
    distancia = duracion/58.2; //Especificado por el fabricante


    // CHECA FALLO DE LECTURA DE ALGÚN SENSOR Y REINICIA SI ES ASI
    //dht11
    if (isnan(h) || isnan(t) || isnan(f)) 
    {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    //Joystick
    if (isnan(lecturaX) || isnan(lecturaY)) 
    {
      Serial.println(F("Failed to read from joystick sensor!"));
      return;
    }

    //Sonido
    if (isnan(sonido)) 
    {
      Serial.println(F("Failed to read from KY-038 sensor!"));
      return;
    }

    if (isnan(distancia)) 
    {
      Serial.println(F("Failed to read from HC-SR04 sensor!"));
      return;
    }    

  
    //Cehca si la firebase esta lista y se registro el ESP32 y da un delay de 15000
    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
      sendDataPrevMillis = millis();

      // Escibre la humedad del DHT11
      if (Firebase.RTDB.setInt(&fbdo, "test/Humidity", h)){
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }


      // Escribe la temperatura del DHT11 
      if (Firebase.RTDB.setFloat(&fbdo, "test/Temperatura",  + t)){
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }

      //Escribe la temperatura en fah del DHT11 
      if (Firebase.RTDB.setFloat(&fbdo, "test/TemperaturaFah",  + f)){
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }

      // Escribe la humead de heat del DHT11 
      if (Firebase.RTDB.setFloat(&fbdo, "test/HeatIndex",  + hic)){
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }

      // Escribe heat index en fah
      if (Firebase.RTDB.setFloat(&fbdo, "test/HeatIndexFah",  + hif)){
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }

      //  Escribe el ejex del joystick
      if (Firebase.RTDB.setInt(&fbdo, "test/Ejex", lecturaX)){
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }

      // Escribe el ejey del joystick 
      if (Firebase.RTDB.setInt(&fbdo, "test/Ejey", lecturaY)){
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
      

      //  Escribe el sonido del sensor KY-038
      if (Firebase.RTDB.setInt(&fbdo, "test/Sonido", sonido)){
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
        sonido = 0;
      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }

      //Escibre la distancia del sensor HC-SR04
      if (Firebase.RTDB.setInt(&fbdo, "test/Distancia", distancia)){
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
      //Se lee el dato numero para desplejar en display de 7 segementos
      if (Firebase.RTDB.getString(&fbdo, "test/numero")){
        numero = fbdo.stringData(); 
        Serial.println(numero);
        //Se pone el numero en el display de 7 segmentos
        segment_display(numero);
      }
    }
    //Se pone el tiempo que lleva el sistema
    tiempo=millis();
  }
  //Se reasigna tiempo para pasarlo otra vez el ciclo
  tiempoAnterior=millis();
  
}
