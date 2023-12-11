#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char * WIFI_SSID = "Isaac123";
const char * WIFI_PASS = "1234IJRP";

const char * MQTT_BROKER = "a30kzystkjltf5-ats.iot.us-east-1.amazonaws.com";
const int MQTT_BROKER_PORT = 8883;

const char * MQTT_CLIENT_ID = "YOUR_CLIENT_ID";

const char * SUBSCRIBE_TOPIC = "cortina/ucb/sub";
const char * PUBLISH_TOPIC = "cortina/ucb/pub";

const char * UPDATE_ACCEPTED_TOPIC = "$aws/things/cortina/shadow/update/accepted";//pub
const char * UPDATE_TOPIC = "$aws/things/cortina/shadow/update";//sub

const int Tope = 34;       // GPIO 13 para el sensor de luz que servira de tope
const int SensorLuz = 33;          // GPIO 12 para el sensor de activacion del motor
const int AccionMotor = 2;        // Número total de acciones del motor
int Motor[AccionMotor] = {4, 5};  // Pines GPIO del motor

const char AMAZON_ROOT_CA1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char CERTIFICATE[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUODA7TyMOgkILNRnQXbhS3pPJ3P0wDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIzMTExOTE2Mzgw
NVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAK0+688hCwZEKeJO2zgt
sE1YYjPHNAMcsBmV4a6t5zsJ0FTWl8Jb8iwXKbjScB3nsUnAUuISFNa5s2elxnmi
tl0nrxsXg9PzwXxmpExctT/xHTUa0QDAywmipJJaynak7Fn+cxAgDBog5dCPmAJi
N6fViYdygRXt5UtMeH5p2A8eLSymdqzKnRPFt7yGfXxgBcvCBnKzn+2rOs/UwE/k
FXFfxTKo+iNQHDcO9m70AhNpPWtc55ZpZ4jl6nXFzpkqK65uy6Iev4QmN2syF2a0
aoUaGJ42vbx9oE20o57KjhICJk3HMA3eTd9ZrFxRBbwpwb0NUyfGo4eTiOED5AWv
Bx8CAwEAAaNgMF4wHwYDVR0jBBgwFoAUPyRz6UQczle9oRJwuUzvxcWBQp8wHQYD
VR0OBBYEFAxjZM5+DpDFaWqK3u7594grm0vMMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCmnxHydyEhrSp6tp50rhjz5qbe
/KSyVRw3cO7lgBQQV7VTKlnalHmHpZr5gC6+r7JQjlSesrkuaNyLpuGjZJaJYudO
hm/ci42FwiGRfCIqe8kf7KL6YHjkC3ZT8Mc8cDIDbmAzY4eLa5ZwDvAHTVOcdJgW
NgzPwYnVGYKp5XTGITJEwMqHqt1/3aU8iLfJsvBcrdyqAQCAjWXpYy6rp+JeNw/q
OATbT6IlT3rtnY1/wvP4REegGnl5bZlxuXE5/DGCD8+a9Edrx2j3ZdTnVYclJfiy
vpST+BJyi27RRAJHKXdSN8aTmlRKf67EBjEi0yJkeFsNPK1NEdKQROml4cRG
-----END CERTIFICATE-----
)KEY";

const char PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEArT7rzyELBkQp4k7bOC2wTVhiM8c0AxywGZXhrq3nOwnQVNaX
wlvyLBcpuNJwHeexScBS4hIU1rmzZ6XGeaK2XSevGxeD0/PBfGakTFy1P/EdNRrR
AMDLCaKkklrKdqTsWf5zECAMGiDl0I+YAmI3p9WJh3KBFe3lS0x4fmnYDx4tLKZ2
rMqdE8W3vIZ9fGAFy8IGcrOf7as6z9TAT+QVcV/FMqj6I1AcNw72bvQCE2k9a1zn
lmlniOXqdcXOmSorrm7Loh6/hCY3azIXZrRqhRoYnja9vH2gTbSjnsqOEgImTccw
Dd5N31msXFEFvCnBvQ1TJ8ajh5OI4QPkBa8HHwIDAQABAoIBAGsLi8yp0BQJHhAV
RWLK6DqCN/tiRIL/wNeuQooaLdvOzoj9JzwSb+pKTZ9Dd78in+rACPZi0blzeKS8
S4xwu1mH4axNtUjLPmxNR4tfZSajJrbfYPCkL1A+zI0ZyCvS768Th6kZBCFI3ncl
onkmktkDGj00xQn1uk3ybCk/4R5FNBz8xkyr5KUBhlTeWeaUX5vAb8sZpzefcFDZ
gQh2rq+oHCGzzDRX/+f1Jgpr4Og3WJjvZ5lgdxTd5D3xTr3JgdupshNEmGWZkd9d
wKilJLKt0UjN9LIvHhBrJyRIRXqMInfHtMi9RJAL1jVXPyvb1qE7aR3MothYPLcy
pO3RL3ECgYEA4j+mi9mV8o+zt+ceC1ZAuP/O9oD6fTFtutzx+FiULFF2vfrYFACI
xadDGQOLyLxzONsV3wplTGWEUyjSyzbDjd0KBzUCtVgKyo2o5ZBYQ8dLJV8ol0sU
uii4WXU9bX61Hx7+8lez+lh7mJmYbfy/z/4AsUBHGVgmltmzlGfyi3MCgYEAxAcA
oT3HjdJCQKGALGzC/6sc9EhMktcasRF8MI5h9XNEIOLgBYeWNxdoWF79ytT0kj+v
gZGkY6EubU+4m2G95jJS/d+vEIStcJOly37aZ4EdWtvUxondOIjjtnxYzvq+HvDq
LG/w6juygT+4uoYhffJ4vDIi62XJitIGIPUawqUCgYEAyk60NBVqWa1ZvLYzTVvh
IAvOGuoWEmBiiFcNH07FoCNwFEz7bldT9bms6+8ZzXOFbG+rTPfUwM1914aHhyL/
Fhly4CLsuw9xBtJJFo1N/71zWoWOnEqMzDGhNoz5UrGcb6mdu0HlM5RvYHLntGHv
eqLcJ6MccHM8fmNIiwmv/jUCgYAJkk31kY6s3hdeEISxpp3e3fwL8XvLr7Ud8Dyg
IlOmKHqrIGjP3E75Yas2pWy++qb2fSWUrgfY1YXu0HWzIJKlKs/ilYl2xQOy+4Kc
7RS8+RD13sYE67ZhjE2SG4A6mwOtfxXwFYct6LxOk03xzZrVC4HZrpkF5XJx1M93
zIACAQKBgQCxopcxgqPElrHGILeVNVNsiziCmIbhwbbrEgtxJQOtMMXZ6oK9P80n
REWzr4GOzG+hLQZ1M/oLD/16DLAoCEun9iRQMeFhpPK4o1NCvThM8HRpf4IA4rI9
qCiYIC+BV7vc1aL/qMGROQwvTKwaK46dxDId62WMEC+ObecoTC0Hcw==
-----END RSA PRIVATE KEY-----
)KEY";

WiFiClientSecure wiFiClient;
PubSubClient mqttClient(wiFiClient);
//
StaticJsonDocument<JSON_OBJECT_SIZE(3)> outputDocEstadoShadow;
char outputBufferEstadoShadow[128];
String estado = "unknown";

void reportEstado(String str){
  estado = str;
  outputDocEstadoShadow["state"]["reported"]["estado"] = estado.c_str();
  serializeJson(outputDocEstadoShadow, outputBufferEstadoShadow);
  mqttClient.publish(UPDATE_TOPIC, outputBufferEstadoShadow);
}


DynamicJsonDocument inputDoc(2048);

void callback(const char * topic, byte * payload, unsigned int lenght) {
  String message;
  for (int i = 0; i < lenght; i++) {
    message += String((char) payload[i]);
  }
  if (String(topic) == UPDATE_ACCEPTED_TOPIC) {
    Serial.println("Message from topic " + String(topic) + ":" + message);
    
    DeserializationError err = deserializeJson(inputDoc, payload);
    if (!err) {
      //String action = inputDoc["estado"].as<String>();
      String action = inputDoc["state"]["reported"]["estado"].as<String>();
      Serial.println("");
      Serial.print("action: ");
      Serial.print(action);
      Serial.println("");
      Serial.println("");
      Serial.print("aoc: ");
      Serial.print(analogRead(Tope));
      Serial.println("");
      if (action == "abrir") {
        digitalWrite(4, HIGH);
        digitalWrite(5, LOW);
        delay(1100);
        digitalWrite(4, LOW);
        digitalWrite(5, LOW);
       } 
      if(action == "cerrar")
       {
        digitalWrite(4, LOW);
        digitalWrite(5, HIGH);
        delay(1100);
        digitalWrite(4, LOW);
        digitalWrite(5, LOW);
       }
      if(action == "detener")
       {
        digitalWrite(4, LOW);
        digitalWrite(5, LOW); 
       }
      //Serial.println(action);
      //setBuiltInLed(action);
      /*if(!action.isEmpty() && !action.equals(estado)){
        setBuiltInLed(action);
      } */   
    }
    
  }
}

boolean mqttClientConnect() {
  Serial.print("Connecting to " + String(MQTT_BROKER));
  if (mqttClient.connect(MQTT_CLIENT_ID)) {
    Serial.println(" DONE!");

    mqttClient.subscribe(UPDATE_ACCEPTED_TOPIC);
    Serial.println("Subscribed to " + String(UPDATE_TOPIC));
  } else {
    Serial.println("Can't connect to " + String(MQTT_BROKER));
  }
  return mqttClient.connected();
}

void setup() {
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  Serial.begin(115200);
  pinMode(Tope, ANALOG);  // Configuración del pin Trigger como salida
  pinMode(SensorLuz, ANALOG);      // Configuración del pin Echo como entrada
  
  // Configuración de los pines GPIO de los LEDs como salidas
  for (int i = 0; i < AccionMotor; i++) {
    pinMode(Motor[i], OUTPUT);
  } 
  Serial.print("Connecting to " + String(WIFI_SSID));

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println(" DONE!");

  wiFiClient.setCACert(AMAZON_ROOT_CA1);
  wiFiClient.setCertificate(CERTIFICATE);
  wiFiClient.setPrivateKey(PRIVATE_KEY);

  mqttClient.setServer(MQTT_BROKER, MQTT_BROKER_PORT);
  mqttClient.setCallback(callback);
}

unsigned long previousConnectMillis = 0;
unsigned long previousPublishMillis = 0;

unsigned char counter = 0;

StaticJsonDocument<JSON_OBJECT_SIZE(3)> outputDocLuz;
char outputBufferLuz[128];

void publishValueLuz(String valor) {
  outputDocLuz["accion"] = valor;
  serializeJson(outputDocLuz, outputBufferLuz);
  mqttClient.publish(PUBLISH_TOPIC, outputBufferLuz);
}

int luz; //variable para guardar datos del sensor de luz
int aoc; //variable para medir si aun hay luz para cerrar la cortina
String est;
String Time;
String dat;
void loop() {
  unsigned long now = millis();
  if (!mqttClient.connected()) {
    if (now - previousConnectMillis >= 2000) {
      previousConnectMillis = now;
      if (mqttClientConnect()) previousConnectMillis = 0;
      else delay(10);
    }
  } else { // Connected to the MQTT Broker
    mqttClient.loop();
    delay(20);

    luz = analogRead(SensorLuz);
    aoc = analogRead(Tope);
    //Serial.print("luz: ");
    //Serial.print(luz);
    //Serial.print(" -- ");
    //Serial.print("abierto?: ");
    if(aoc > 1800)
    {
      est = "abierta";   
    }
    else
    {
      est = "cerrada";
    }
    
    if(luz > 800)
    {
      Time = "dia";   
    }
    if(luz < 800)
    {
      Time = "noche";   
    }
    if(Time == "dia" && est == "abierta"){
      dat = "detener";
      }
    if(Time == "dia" && est == "cerrada"){
      dat = "abrir";
      }
    if(Time == "noche" && est == "cerrada"){
      dat = "detener";
      }
    if(Time == "noche" && est == "abierta"){
      dat = "cerrar";
      }
    if (now - previousPublishMillis >= 1000) {
      previousPublishMillis = now;
      publishValueLuz(dat);//rule con los topics
      delay(500);
      reportEstado(dat);//shadow
    }
  }
}
