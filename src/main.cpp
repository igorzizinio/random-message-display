#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const char *BASE_URL = "https://random-messages--igorunderplayer.replit.app/api";

const char *SSID = "Remofer do Brasil";
const char *PASSWORD = "amelhor2019";

const uint8_t LCD_I2C_ADDRESS = 0x27;
const uint8_t LCD_COLUMNS = 16;
const uint8_t LCD_ROWS = 2;
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);

WiFiClientSecure secureClient;
HTTPClient client;

String normalizeText(String s)
{
  s.replace("á", "a");
  s.replace("à", "a");
  s.replace("ã", "a");
  s.replace("â", "a");
  s.replace("Á", "A");
  s.replace("À", "A");
  s.replace("Ã", "A");
  s.replace("Â", "A");

  s.replace("é", "e");
  s.replace("ê", "e");
  s.replace("É", "E");
  s.replace("Ê", "E");

  s.replace("í", "i");
  s.replace("Í", "I");

  s.replace("ó", "o");
  s.replace("ô", "o");
  s.replace("õ", "o");
  s.replace("Ó", "O");
  s.replace("Ô", "O");
  s.replace("Õ", "O");

  s.replace("ú", "u");
  s.replace("Ú", "U");

  s.replace("ç", "c");
  s.replace("Ç", "C");

  return s;
}

void setup()
{
  Serial.begin(115200);

  delay(500);
  lcd.init();
  lcd.backlight();

  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, IPAddress(8, 8, 8, 8));
  WiFi.begin(SSID, PASSWORD);

  lcd.setCursor(0, 0);
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print("WiFi");
  uint8_t dots = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    dots = (dots + 1) % 4;
    lcd.setCursor(0, 1);
    lcd.print("WiFi");
    lcd.setCursor(4, 1);
    lcd.print("   "); // Clear previous dots
    lcd.setCursor(4, 1);
    for (uint8_t i = 0; i < dots; i++)
    {
      lcd.print(".");
    }
  }

  lcd.clear();
  lcd.print("Connected!");

  secureClient.setInsecure();

  delay(200);
}

void loop()
{
  lcd.clear();

  client.begin(secureClient, BASE_URL);
  int httpCode = client.GET();
  if (httpCode > 0)
  {
    String payload = client.getString();
    Serial.println(payload);

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    String message = doc["msg"];
    message = normalizeText(message);
    Serial.println("Message: " + message);

    lcd.print(message.substring(0, min((size_t)LCD_COLUMNS, message.length())));
    if (message.length() > LCD_COLUMNS)
    {
      lcd.setCursor(0, 1);
      lcd.print(message.substring(LCD_COLUMNS, min((size_t)(LCD_COLUMNS * 2), message.length())));
    }
  }
  else
  {
    Serial.println("Error on HTTP request");
  }

  delay(3000);
}
