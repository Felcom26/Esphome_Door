//#include <LiquidCrystal_I2C.h>
//#include <OneButton.h>
//#include <Adafruit_PWMServoDriver.h>
#include "analogWrite.h"
//#include <Wire.h>
#include "ESP32Servo.h"

//#include "esphome.h"

#define drive_pin 12
#define dir_pin 14
#define rot_s_pin 27
#define engage_pin 13
#define pot_pin 35
#define ES_off_pin 16
#define ES_on_pin 17

class Door_CMD : public Component {
 public:
  bool Open_CMD = false;
  bool Close_CMD = false;
  Door_CMD(esphome::template_::TemplateSwitch *&_Ocmd, esphome::template_::TemplateSwitch *&_Ccmd) {
    _Ocmd->add_on_state_callback([this](bool newOpen) { Open_CMD = newOpen; });
    _Ccmd->add_on_state_callback([this](bool newClose) { Close_CMD = newClose; });
  }

  Servo Engage;

  bool ES_off = 0;
  bool ES_on = 0;

  int f_e = 0;
  int f_d = 0;
  int f_a = 0;
  int f_f = 0;

  // bool cmd_e = 0;
  // bool cmd_d = 0;

  char cmd = 'n';

  bool Estado_EM = 1;
  char Estado_DM = 'n';

  int stop_vel = 1500;
  int max_vel = 1000;

  int ang_aberto = 238;
  int ang_fechado = 118;
  int set_drive_vel = 255;
  int set_engage_vel = 170;

  int stop_vel_dm = 255;
  int drive_vel_dm = stop_vel_dm - set_drive_vel;
  int engage_vel_dm = stop_vel_dm - set_engage_vel;

  int pot;
  int pos;

  float tempo;

  char serial;

  bool busy = 0;

  void engate() {
    if (ES_on == 0 && f_e == 1) {
      f_e = 2;
      Engage.attach(engage_pin);
      Engage.writeMicroseconds(stop_vel - max_vel);
      Serial.println("engatando");
    } else if (ES_on == 1 && f_e == 2) {
      f_e = 0;
      Engage.writeMicroseconds(stop_vel);
      Estado_EM = 1;
      Serial.println("Estado EM :");
      Serial.println(Estado_EM);
      // Engage.detach();
    }
  }

  void desengate() {
    if (ES_off == 0 && f_d == 1) {
      f_d = 2;
      Engage.attach(engage_pin);
      Engage.writeMicroseconds(stop_vel + max_vel);
      Serial.println("desengatando");
    } else if (ES_off == 1 && f_d == 2) {
      f_d = 0;
      Engage.writeMicroseconds(stop_vel);
      Estado_EM = 0;
      Serial.println("Estado EM :");
      Serial.println(Estado_EM);
      // Engage.detach();
    }
  }

  void abrir() {
    if (f_a == 1) {
      cmd = 'e';
      f_a = 2;
      analogWrite(drive_pin, engage_vel_dm);
      digitalWrite(dir_pin, 1);
    }

    else if (f_a == 2 && Estado_EM == 1) {
      f_a = 3;
      analogWrite(drive_pin, drive_vel_dm);
      digitalWrite(dir_pin, 1);
      Serial.println("abrindo");
    }

    else if (pos >= ang_aberto && f_a == 3) {
      f_a = 4;
      analogWrite(drive_pin, stop_vel_dm);
      Serial.println("aberto");
    }

    else if (f_a == 4) {
      cmd = 'd';
      f_a = 0;
      Estado_DM = 'a';
      busy = 0;
    }
  }

  void fechar() {
    if (f_f == 1) {
      cmd = 'e';
      f_f = 2;
      analogWrite(drive_pin, engage_vel_dm);
      digitalWrite(dir_pin, 0);
    }

    else if (f_f == 2 && Estado_EM == 1) {
      f_f = 3;
      analogWrite(drive_pin, drive_vel_dm);
      digitalWrite(dir_pin, 0);
      Serial.println("fechando");
    }

    else if (pos <= ang_fechado && f_f == 3) {
      f_f = 4;
      analogWrite(drive_pin, stop_vel_dm);
      Serial.println("fechado");
    }

    else if (f_f == 4) {
      cmd = 'd';
      f_f = 0;
      Estado_DM = 'f';
      busy = 0;
    }
  }

  void setup() override {
    Serial.begin(9600);

    pinMode(ES_off_pin, INPUT_PULLUP);
    pinMode(ES_on_pin, INPUT_PULLUP);

    pinMode(drive_pin, OUTPUT);
    pinMode(dir_pin, OUTPUT);
    pinMode(rot_s_pin, INPUT);

    Engage.attach(engage_pin);
    Engage.writeMicroseconds(stop_vel);

    analogWrite(drive_pin, stop_vel_dm);
    digitalWrite(dir_pin, 0);

    ES_off = digitalRead(ES_off_pin);
    if (ES_off == 0)
      cmd = 'd';
    else
      Estado_EM = 0;

    delay(500);
  }

  void loop() override {
    pot = analogRead(pot_pin);
    ES_off = digitalRead(ES_off_pin);
    ES_on = digitalRead(ES_on_pin);
    serial = Serial.read();

    pos = map(pot, 4096, 0, 0, 270);

    if (Open_CMD == 1 && busy == 0) {
      cmd = 'a';
      Open_CMD = 0;
      busy = 1;
    }

    if (Close_CMD == 1 && busy == 0) {
      busy = 1;
      cmd = 'f';
      Close_CMD = 0;
    }

    if ((cmd == 'e' || serial == 'e') && Estado_EM == 0) {
      f_e = 1;
      cmd = 'n';
      Serial.println("F_en");
    }

    if ((cmd == 'd' || serial == 'd') && Estado_EM == 1) {
      f_d = 1;
      cmd = 'n';
      Serial.println("F_de");
    }

    if (cmd == 'a' || serial == 'a') {
      f_a = 1;
      cmd = 'n';
      Serial.println("F_ab");
    }

    if (cmd == 'f' || serial == 'f') {
      f_f = 1;
      cmd = 'n';
      Serial.println("F_fe");
    }

    engate();
    desengate();
    abrir();
    fechar();

    if ((millis() - tempo) > 2000) {
      Serial.print("POT:  ");
      Serial.print(pos);
      Serial.print("  ES_en:  ");
      Serial.print(ES_on);
      Serial.print("  ES_de:  ");
      Serial.print(ES_off);
      Serial.print("    Estado_EM:  ");
      Serial.print(Estado_EM);
      Serial.print("  Estado_DM:  ");
      Serial.println(Estado_DM);

      tempo = millis();
    }
  }
};