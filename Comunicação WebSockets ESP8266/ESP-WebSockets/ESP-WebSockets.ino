#include <Servo.h>
#include <ESP8266WebServer.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <Hash.h>

WebSocketsServer webSocket = WebSocketsServer(81);
Servo MeuServo;

const char* ssid     = "NomeDaRede";
const char* password = "SenhaDaRede";

int pos = 0;
String PosF =  "";

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  switch (type) { //verifica o tipo de mensagem recebida
    case WStype_DISCONNECTED:
      Serial.println("Disconneted");
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: { //caso conecte
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connectado com: %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        webSocket.sendTXT(num, "Connected");
        PosF = "g90";
        Serial.print("Valor inicial do servo motor: "); Serial.println(PosF);
        webSocket.sendTXT(num, PosF); //envia a posição
      }
      break;
    case WStype_TEXT: {
        String text = String((char *) &payload[0]);
        //Se a mensagem começar com X, verifique o valor seguido do X
        if (text.startsWith("x")) {
          String xVal = (text.substring(text.indexOf("x") + 1, text.length()));
          pos = xVal.toInt();
          MeuServo.write(pos); //coloca o servo motor na posição recebida
          Serial.print("xVal: "); Serial.println(pos);
        }

        if (text == "r") {
          Serial.println("r");
          MeuServo.write(0);
          delay(100);
          for (pos = 0; pos <= 180; pos += 2) // goes from 0 degrees to 180 degrees
          {
            MeuServo.write(pos);
            delay(15);
          }
          for (pos = 180; pos >= 0; pos -= 2){
            MeuServo.write(pos);
            delay(15);
          }
        }
        //webSocket.sendTXT(num, payload, lenght);
        //webSocket.broadcastTXT(payload, lenght);
        break;
      }
  }
}

void setup() {
  Serial.begin(115200);
  MeuServo.attach(2); //Declarando Servo
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { //Conectando a rede
    Serial.println(".");
    delay(100);
  }
  Serial.println(WiFi.localIP()); //Mostra o IP na rede
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
}
