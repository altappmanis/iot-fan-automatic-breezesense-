#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Definisikan konstanta dan variabel yang digunakan
#define BLYNK_TEMPLATE_ID "TMPL6NcMEmi2K"
#define BLYNK_TEMPLATE_NAME "IoT Automatic"
#define BLYNK_AUTH_TOKEN "6tHNXmCVHrYP4KuGCTZNINniNtRfwmzb"

#define DHTPIN D5     // Pin digital terhubung ke sensor DHT
#define DHTTYPE DHT22 // Tipe sensor DHT yang digunakan

#define FAN_PIN D2    // Pin relay kipas
WidgetLED FAN(V0);

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "gampang";
char pass[] = "12345678";

float humDHT = 0;
float tempDHT = 0;
int Val = 0; // Batas suhu untuk menghidupkan kipas

// Inisialisasi sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Inisialisasi objek LCD I2C dengan alamat 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW); // Kipas mati pada awalnya
  Serial.println(F("DHTxx test!"));
  dht.begin();
  Blynk.begin(auth, ssid, pass);

  // Inisialisasi LCD I2C dengan pin SDA dan SCL yang ditentukan
  Wire.begin(D6, D7); // Set pin D6 untuk SDA dan D7 untuk SCL
  lcd.begin();
  lcd.backlight();

  // Tampilkan teks di baris pertama
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  
  // Tampilkan teks di baris kedua untuk status kipas
  lcd.setCursor(0, 1);
  lcd.print("Kipas: ");
}

BLYNK_WRITE(V3) {
  Val = param.asInt(); // Menyimpan nilai dari pin virtual V3 ke variabel

  Serial.print("The Threshold value is: ");
  Serial.println(Val);
  Serial.println();
}

void loop() {
  Blynk.run();

  // Membaca suhu dan kelembaban dari sensor DHT
  humDHT = dht.readHumidity();
  tempDHT = dht.readTemperature();

  // Periksa apakah pembacaan suhu gagal
  if (isnan(tempDHT)) {
    Serial.println("Failed to read temperature from DHT sensor!");
    return;
  }

  // Tampilkan data suhu pada LCD
  lcd.setCursor(6, 0); // Mulai menulis suhu dari kolom ke-7
  lcd.print(tempDHT);
  lcd.print("C");

  // Tampilkan status kipas (Kipas ON atau Kipas OFF) pada LCD
  lcd.setCursor(7, 1); // Mulai menulis status kipas dari kolom ke-8
  if (tempDHT >= Val) { // Kipas menyala jika suhu lebih besar atau sama dengan Val
    digitalWrite(FAN_PIN, HIGH);
    FAN.on();
    lcd.print("ON ");
    Serial.println("Fan ON");
  } else { // Kipas mati jika suhu kurang dari Val
    digitalWrite(FAN_PIN, LOW);
    FAN.off();
    lcd.print("OFF");
    Serial.println("Fan OFF");
  }

  // Kirim nilai suhu dan kelembaban ke Blynk
  Blynk.virtualWrite(V1, tempDHT);
  Blynk.virtualWrite(V2, humDHT);

  delay(2000); // Delay untuk memperlambat tampilan pada LCD
}
