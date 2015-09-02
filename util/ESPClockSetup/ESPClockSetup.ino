#define OEM
#define SIZE18
#define SIZE50

#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#ifdef OEM
#define _Digole_Serial_I2C_
#include <DigoleSerial.h>
#else
#include <Digole.h>
#endif

#ifdef SIZE18
#include "DroidMono18.h"
#endif
#ifdef SIZE50
#include "DroidMono50.h"
#endif

#ifdef OEM
static DigoleSerialDisp lcd(&Wire, 0x27);
#else
static DigoleI2C lcd(Wire, 0x27);
#endif

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Serial.begin(115200);
  Serial.println("UP");
  Serial.flush();
  delay(500);
  digitalWrite(13, LOW);
  lcd.begin();
  Serial.println("LCD UP");
  Serial.flush();
  delay(100);

#ifdef SIZE18
  lcd.clearScreen();
  lcd.setFont(100);
  Serial.println("Uploading Droind Mono 18...");
  lcd.println("Uploading Droid Mono 18...");
  delay(500);
  digitalWrite(13, HIGH);
#ifdef OEM
  lcd.uploadUserFont((uint16_t)sizeof(DroidMono18), DroidMono18, 0);
#else
  lcd.uploadUserFont(0, DroidMono18, (uint16_t)sizeof(DroidMono18));
#endif
  digitalWrite(13, LOW);
  delay(500);
  lcd.clearScreen();
  lcd.setFont(200);
  lcd.println("0123 ABC abc");

  delay(5000);
#endif /* SIZE18 */

#ifdef SIZE50
  lcd.clearScreen();
  lcd.setFont(100);
  Serial.println("Uploading Droind Mono 50...");
  lcd.print("Uploading Droid Mono 50...");
  delay(500);
  digitalWrite(13, HIGH);
#ifdef OEM
  lcd.uploadUserFont((uint16_t)sizeof(DroidMono50), DroidMono50, 1);
#else
  lcd.uploadUserFont(1, DroidMono50, (uint16_t)sizeof(DroidMono50));
#endif
  digitalWrite(13, LOW);
  delay(500);
  lcd.clearScreen();
  lcd.setFont(201);
  lcd.print("0123456789:");
  
  delay(5000);
#endif /* SIZE50 */

  lcd.clearScreen();
  lcd.setFont(100);
  lcd.print("Done!\nSafe to turn off!");
}

void loop() {
  // Just a heartbeat
  Serial.println("ping");
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
}
