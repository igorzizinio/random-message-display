#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

constexpr const char *BASE_URL = "https://random-messages--igorunderplayer.replit.app/api";

constexpr const char *SSID = "Remofer do Brasil";
constexpr const char *PASSWORD = "amelhor2019";

constexpr const uint8_t LCD_I2C_ADDRESS = 0x27;
constexpr const uint8_t LCD_COLUMNS = 16;
constexpr const uint8_t LCD_ROWS = 2;

constexpr const uint8_t RESET_PIN = 23;

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

void get_random_message()
{
  lcd.clear();

  client.begin(secureClient, BASE_URL);
  int httpCode = client.GET();
  if (httpCode > 0)
  {
    String payload = client.getString();
    Serial.println(payload);

    JsonDocument doc;
    if (DeserializationError error = deserializeJson(doc, payload); error)
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

  client.end();
}

void setup()
{
  pinMode(RESET_PIN, INPUT_PULLUP);

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

constexpr uint32_t fetchInterval = 3000; // 3 seconds
void loop()
{
  // "static" mark the variable to keep its value between function calls
  static uint32_t lastFetch = 0;
  static uint32_t lastResetPress = 0;

  // check for reset button press + debounce to avoid multiple reset calls
  if (digitalRead(RESET_PIN) == LOW && millis() - lastResetPress > 1000)
  {
    lastResetPress = millis();

    Serial.println("Reset via botão");
    lcd.clear();
    lcd.print("Resetting... in");
    lcd.setCursor(0, 1);

    for (int i = 3; i > 0; i--)
    {
      lcd.print("   ");
      lcd.setCursor(0, 1);
      lcd.print(i);
      delay(1000);
    }

    ESP.restart();
  }

  if (millis() - lastFetch >= fetchInterval)
  {
    get_random_message();
    lastFetch = millis();
  }
}
