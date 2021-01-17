#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

const char* ssid     = "dlink-D520";
const char* password = "rvhvc24446";

int motorRele = 23;

LiquidCrystal_I2C lcd(0x27, 16, 2);

WiFiServer server(80);

void setup()
{
  Serial.begin(115200);
  pinMode(motorRele, OUTPUT); 
  
  digitalWrite(motorRele, HIGH);
  
  lcd.init();
  lcd.backlight();
  lcd.print("Bem Vindo!");

  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());

  server.begin();

}

int value = 0;

void loop() {
  WiFiClient client = server.available();

  if (client) {                            
    Serial.println("Novo Cliente");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            String html = "<!DOCTYPE html><html><head><title>ESP32 WEB</title>";
            html += "<meta name='viewport' content='user-scalable=no'>";
            html += "<style>button{background: red; width: 20vw; height: 4vh; border-radius: 20px; border: none; color: white;}";
            html += "</style></head>";
            html += "<body bgcolor='ffffff'><center><h1>";
            html += "<form method='GET' action='/H'><button value='H' type='submit'>Desligar Laser</button></form>";
            html += "<form method='GET' action='/L'><button value='L' type='submit'>Ligar Laser</button></form>";
            html += "</h1></center></body></html>";

            client.print(html);
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        if (currentLine.endsWith("GET /H?")) {
          digitalWrite(motorRele, HIGH);
          lcd.clear();
          lcd.setCursor(3,0);
          lcd.print("Desligado");
        }
        if (currentLine.endsWith("GET /L?")) {
          digitalWrite(motorRele, LOW);
          lcd.clear();
          lcd.setCursor(5,0);
          lcd.print("Ligado");
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
