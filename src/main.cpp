#include <FirebaseESP32.h>
#include <WiFi.h>
#include <addons/TokenHelper.h>

/* 1. Define the WiFi credentials */
// #define WIFI_SSID "Wokwi-GUEST"
// #define WIFI_PASSWORD ""

#define WIFI_SSID "Galaxy S10e0604"
#define WIFI_PASSWORD "jonascz123"

#define FIREBASE_DATABASE_URL "https://placarteste-f3d3f-default-rtdb.firebaseio.com"
#define FIREBASE_TOPIC "/placar/gols_casa"
#define FIREBASE_API_KEY "AIzaSyDDxxT35nu6DoidikMpza6n_c339J-8Dis"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

bool hasResult = true;
unsigned long currentMillis = millis();

int newText = 0;
int currentText = 0;
bool newTextAvaible = true;

int buttonPin = 2;
int displayValue = 0;
int lastButtonState = LOW;
int buttonState;

unsigned long previousMillis = 0UL;
unsigned long interval = 1000UL;

const int pin_a = 19;
const int pin_b = 21;
const int pin_c = 15;
const int pin_d = 2;
const int pin_e = 4;
const int pin_f = 18;
const int pin_g = 5;

const int pin_com1 = 12;
const int pin_com2 = 13;
const int pin_com3 = 14;

// const int pin_a = 0;
// const int pin_b = 4;
// const int pin_c = 16;
// const int pin_d = 17;
// const int pin_e = 5;
// const int pin_f = 18;
// const int pin_g = 19;

// const int pin_com1 = 27;
// const int pin_com2 = 14;
// const int pin_com3 = 12;

void streamCallback(StreamData data);
void setDisplayText(int h_text);
void streamTimeoutCallback(bool timeout);

int pausa = 1;

void setup()
{

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi... ");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
  }

  Serial.println("ok");

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  config.database_url = FIREBASE_DATABASE_URL;
  config.api_key = FIREBASE_API_KEY;
  config.token_status_callback = tokenStatusCallback;

  Firebase.reconnectNetwork(true);

  fbdo.setBSSLBufferSize(4096, 1024);

  Serial.print("Sign up new user... ");

  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("ok");
  }
  else
  {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);

  if (!Firebase.beginStream(fbdo, FIREBASE_TOPIC))
  {
    Serial.println(fbdo.errorReason());
  }

  Firebase.setStreamCallback(fbdo, streamCallback, streamTimeoutCallback);

  Serial.println("Firebase initialized.");
  pinMode(pin_a, OUTPUT);
  pinMode(pin_b, OUTPUT);
  pinMode(pin_c, OUTPUT);
  pinMode(pin_d, OUTPUT);
  pinMode(pin_e, OUTPUT);
  pinMode(pin_f, OUTPUT);
  pinMode(pin_g, OUTPUT);

  pinMode(pin_com1, OUTPUT);
  pinMode(pin_com2, OUTPUT);
  pinMode(pin_com3, OUTPUT);
}

int get_7SD_value(int n)
{
  if (n > 9)
    n = 9;
  if (n < 0)
    n = 0;

  switch (n)
  {
  case 0:
    return 63;
    break;
  case 1:
    return 6;
    break;
  case 2:
    return 91;
    break;
  case 3:
    return 79;
    break;
  case 4:
    return 102;
    break;
  case 5:
    return 109;
    break;
  case 6:
    return 125;
    break;
  case 7:
    return 7;
    break;
  case 8:
    return 127;
    break;
  case 9:
    return 103;
  }
  return 0;
}

void check_pin(int pin, int b)
{
  if (b == 1)
  {
    digitalWrite(pin, LOW);
    delay(pausa);
    digitalWrite(pin, HIGH);
  }
  else
    digitalWrite(pin, HIGH);
}

void display_7sd(int v, int pin_com)
{
  if (v > 9)
    v = 9;
  if (v < 0)
    v = 0;

  int n = get_7SD_value(v);

  digitalWrite(pin_com, HIGH);

  check_pin(pin_a, bitRead(n, 0));
  check_pin(pin_b, bitRead(n, 1));
  check_pin(pin_c, bitRead(n, 2));
  check_pin(pin_d, bitRead(n, 3));
  check_pin(pin_e, bitRead(n, 4));
  check_pin(pin_f, bitRead(n, 5));
  check_pin(pin_g, bitRead(n, 6));

  digitalWrite(pin_com, LOW);
}

void display_value(float v)
{
  int n;

  n = v / 100;
  v = v - (n * 100);
  display_7sd(n, pin_com1);

  n = v / 10;
  v = v - (n * 10);
  display_7sd(n, pin_com2);

  display_7sd(v, pin_com3);
}

void streamCallback(StreamData data)
{
  if (data.dataTypeEnum() == firebase_rtdb_data_type_integer)
  {
    int dataText = data.to<int>();
    setDisplayText(dataText);
  }
}

void setDisplayText(int h_text)
{
  Serial.print("Data: ");
  Serial.println(h_text);

  newText = h_text;
  newTextAvaible = true;
}

void streamTimeoutCallback(bool timeout)
{
  if (timeout)
  {
    Serial.println("Stream timeout, resume streaming...");
  }
}

void loop()
{

  unsigned long currentMillis = millis();

  // if (currentMillis - previousMillis > interval)
  // {
  //   if (newTextAvaible)
  //   {
  //     currentText = newText;
  //     display_value(currentText);
  //   }
  //   previousMillis = currentMillis;
  // }

  currentText = newText;
  display_value(currentText);

  if (Firebase.ready())
  {
    // ready
  }
}