#include <Wire.h>         
#include <BH1750.h>    

BH1750 lightMeter;

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "SDL_Arduino_INA3221.h"

const char* ssid = "Parametrik RnD";
const char* password = "tabassam";
const char* mqtt_server = "broker.mqtt-dashboard.com";

int number = 0;
int urut = 0;
int x;
float lux = 0;
int luxi = 0;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

SDL_Arduino_INA3221 ina3221;

#define SOLAR_CELL_CHANNEL 2
#define OUTPUT_CHANNEL 3

void setup_wifi() {

  delay(10);
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);      
   Serial.print("t7.txt=\"Connecting\"");
   Serial.write(0xFF);
   Serial.write(0xFF);
   Serial.write(0xFF);
    }
   // Serial.println("\nConnected.");
   Serial.print("t7.txt=\"Connected\"");
   Serial.write(0xFF);
   Serial.write(0xFF);
   Serial.write(0xFF);

   Serial.print("t9.txt=\"MQTT\"");
   Serial.write(0xFF);
   Serial.write(0xFF);
   Serial.write(0xFF);
    //Serial.print(".");
 

  randomSeed(micros());

  //Serial.println("");
  //Serial.println("WiFi connected");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  //Serial.print("Message arrived [");
  //Serial.print(topic);
  //Serial.print("] ");
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
  }
  //Serial.println();

  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);
  } else {
    digitalWrite(BUILTIN_LED, HIGH);
  }

}

void reconnect() {

  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
        //Serial.println("connected");
        client.publish("indicator", "Solar Panel");
       client.subscribe("inTopic");
       Serial.print("t9.txt=\"MQTT1\"");
       Serial.write(0xFF);
       Serial.write(0xFF);
       Serial.write(0xFF);
    //Serial.print(".");
    } else {
      //Serial.print("failed, rc=");
      //Serial.print(client.state());
      //Serial.println(" try again in 5 seconds");
      delay(5000);
       Serial.print("t9.txt=\"MQTT0\"");
       Serial.write(0xFF);
       Serial.write(0xFF);
       Serial.write(0xFF);
    //Serial.print(".");
    }
  }
}

void setup() {
  Wire.begin();
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(9600);
  lightMeter.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);



 //Serial.println("SDA_Arduino_INA3221_Test");

  //Serial.println("Measuring voltage and current with ina3221 ...");
  ina3221.begin();

  //Serial.print("Manufactures ID=0x");
  int MID;
  MID = ina3221.getManufID();
  //Serial.println(MID, HEX);
}

void onConnectionEstablished() {

  float boat = ina3221.getBusVoltage_V(SOLAR_CELL_CHANNEL);

}

void loop()
{

 lux = lightMeter.readLightLevel();
 luxi = lux;
 // Serial.print("Light Meter: ");
 // Serial.print(lux);
 // Serial.println(" lx");
  delay(1000);
  
 //Serial.println("------------------------------");
  float shuntvoltage2 = 0;
  float busvoltage2 = 0;
  float current_mA2 = 0;
  float loadvoltage2 = 0;
  float solarcellpower2 = 0;


  busvoltage2 = ina3221.getBusVoltage_V(SOLAR_CELL_CHANNEL);
  shuntvoltage2 = ina3221.getShuntVoltage_mV(SOLAR_CELL_CHANNEL);
  current_mA2 = ina3221.getCurrent_mA(SOLAR_CELL_CHANNEL);
  loadvoltage2 = busvoltage2 + (shuntvoltage2 / 1000);
  solarcellpower2 = current_mA2 * busvoltage2;

  //Serial.print("Solar Cell Current 2:       "); Serial.print(current_mA2); Serial.println(" mA");
  //Serial.print("Solar Cell Power 2:       "); Serial.print(solarcellpower2); Serial.println(" W");
  //Serial.println("");

  float shuntvoltage3 = 0;
  float busvoltage3 = 0;
  float current_mA3 = 0;
  float loadvoltage3 = 0;

  busvoltage3 = ina3221.getBusVoltage_V(OUTPUT_CHANNEL);
  shuntvoltage3 = ina3221.getShuntVoltage_mV(OUTPUT_CHANNEL);
  current_mA3 = ina3221.getCurrent_mA(OUTPUT_CHANNEL);
  loadvoltage3 = busvoltage3 + (shuntvoltage3 / 1000);

  //Serial.print("Output Bus Voltage 3:   "); Serial.print(busvoltage3); Serial.println(" V");
  //Serial.print("Output Load Voltage 3:  "); Serial.print(loadvoltage3); Serial.println(" V");
  //Serial.print("Output Current 3:       "); Serial.print(current_mA3); Serial.println(" mA");

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 20000) {
    lastMsg = now;
switch(urut){
      case 0:
        snprintf (msg, MSG_BUFFER_SIZE, "A%f\n", current_mA2);
        //Serial.print("Publish message solar cell current 2: ");
        //Serial.println(msg);
        client.publish("myTopic", msg);
          luxi = current_mA2*100;
          Serial.print("x0.val=");
          Serial.print(luxi);
          Serial.write(0xFF);
          Serial.write(0xFF);
          Serial.write(0xFF);
           
          Serial.print("t11.txt=\"Success0\"");
          Serial.write(0xFF);
          Serial.write(0xFF);
          Serial.write(0xFF);
        break;
      case 1:
        snprintf (msg, MSG_BUFFER_SIZE, "B%f\n", current_mA2 * busvoltage2);
        //Serial.print("Publish message solar cell power 2: ");
        //Serial.println(msg);
        client.publish("myTopic", msg);
        luxi = busvoltage2*100;
          Serial.print("x1.val=");
          Serial.print(luxi);
          Serial.write(0xFF);
          Serial.write(0xFF);
          Serial.write(0xFF);
           
          Serial.print("t11.txt=\"Success1\"");
          Serial.write(0xFF);
          Serial.write(0xFF);
          Serial.write(0xFF);
        break;
     case 2:
       snprintf (msg, MSG_BUFFER_SIZE, "C%f\n", busvoltage3);
       //Serial.print("Publish message output bus busvoltage 3: ");
       //Serial.println(msg);
       client.publish("myTopic", msg);
       luxi = busvoltage3*100;
          Serial.print("x2.val=");
          Serial.print(luxi);
          Serial.write(0xFF);
          Serial.write(0xFF);
          Serial.write(0xFF);
           
          Serial.print("t11.txt=\"Success2\"");
          Serial.write(0xFF);
          Serial.write(0xFF);
          Serial.write(0xFF);
       break;
     case 3:
       snprintf (msg, MSG_BUFFER_SIZE, "D%f\n", loadvoltage3);
       //Serial.print("Publish message output load voltage 3: ");
       //Serial.println(msg);
       client.publish("myTopic", msg);
       luxi = loadvoltage3*100;
          Serial.print("x3.val=");
          Serial.print(luxi);
          Serial.write(0xFF);
          Serial.write(0xFF);
          Serial.write(0xFF);
           
          Serial.print("t11.txt=\"Success3\"");
          Serial.write(0xFF);
          Serial.write(0xFF);
          Serial.write(0xFF);
       break;
     case 4:
       snprintf (msg, MSG_BUFFER_SIZE, "E%f\n", current_mA3);
       //Serial.print("Publish message output current 3: ");
       //Serial.println(msg);
       client.publish("myTopic", msg);
       luxi = current_mA3*100;
          Serial.print("x4.val=");
          Serial.print(luxi);
          Serial.write(0xFF);
          Serial.write(0xFF);
          Serial.write(0xFF);
           
          Serial.print("t11.txt=\"Success4\"");
          Serial.write(0xFF);
          Serial.write(0xFF);
          Serial.write(0xFF);
        break;
     case 5:
       snprintf (msg, MSG_BUFFER_SIZE, "F%f\n", lux);
       //Serial.print("Publish message light meters: ");
       //Serial.println(msg);
       client.publish("myTopic", msg);
       luxi = lux*100;
          Serial.print("x5.val=");
          Serial.print(luxi);
          Serial.write(0xFF);
          Serial.write(0xFF);
          Serial.write(0xFF);
           
          Serial.print("t11.txt=\"Success5\"");
          Serial.write(0xFF);
          Serial.write(0xFF);
          Serial.write(0xFF);
        break;
      default :
      break;
    }
     if(urut > 5){
      urut = 0;
    }else{
      urut++;
    }
  }
 delay (20000);
}
