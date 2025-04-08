#include "auto_door.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/hal.h"
//#include "esphome/core/component.h"
//#include "esphome/core/time.h"

Servo Engage;

bool ES_off = false;
bool ES_on = false;

int f_e = 0;
int f_d = 0;
int f_a = 0;
int f_f = 0;

// bool cmd_e = 0;
// bool cmd_d = 0;

char cmd = 'n';

bool Estado_EM = true;
char Estado_DM = 'n';

int stop_vel = 1500;
int max_vel = 1000;

// int ang_aberto = 238;
// int ang_fechado = 118;
const int set_drive_vel = 255;
const int set_engage_vel = 170;

const int stop_vel_dm = 255;
int drive_vel_dm = stop_vel_dm - set_drive_vel;
int engage_vel_dm = stop_vel_dm - set_engage_vel;

int pot;
int pos;

float tempo;

char serial;

bool busy = false;

namespace esphome {
namespace auto_door {

static const char *const TAG = "auto_dooor";

float AUTODOORComponent::get_setup_priority() const { return setup_priority::PROCESSOR; }

// void AUTODOORComponent::set_drive_pin(GPIOPin *drive_pin) {this->drive_pin_ = drive_pin; }
// void AUTODOORComponent::set_dir_pin(GPIOPin *dir_pin) {this->dir_pin_ = dir_pin; }
// void AUTODOORComponent::set_rotsen_pin(GPIOPin *rotsen_pin) {this->rotsen_pin_ = rotsen_pin; }
// void AUTODOORComponent::set_engage_pin(GPIOPin *engage_pin) {this->engage_pin_ = engage_pin; }
// void AUTODOORComponent::set_pot_pin(GPIOPin *pot_pin) {this->pot_pin_ = pot_pin; }
// void AUTODOORComponent::set_esoff_pin(GPIOPin *esoff_pin) {this->esoff_pin_ = esoff_pin; }
// void AUTODOORComponent::set_eson_pin(GPIOPin *eson_pin) {this->eson_pin_ = eson_pin; }

void AUTODOORComponent::set_writer(auto_door_writer_t &&writer) { this->writer_ = writer; }

void AUTODOORComponent::set_ang_open(uint8_t ang_open) { this->ang_open_ = ang_open; }
void AUTODOORComponent::set_ang_close(uint8_t ang_close) { this->ang_close_ = ang_close; }

void AUTODOORComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Auto_Door...");
  // Serial.begin(9600);

  pinMode(esoff_pin_, INPUT_PULLUP);
  pinMode(eson_pin_, INPUT_PULLUP);

  pinMode(drive_pin_, OUTPUT);
  pinMode(dir_pin_, OUTPUT);
  pinMode(rotsen_pin_, INPUT);

  Engage.attach(engage_pin_);
  Engage.writeMicroseconds(stop_vel);

  analogWrite(drive_pin_, stop_vel_dm);
  digitalWrite(dir_pin_, 0);

  ES_off = digitalRead(esoff_pin_);
  if (ES_off == false)
    cmd = 'd';
  else
    Estado_EM = false;

  delay(500);
}

void AUTODOORComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Auto Door:");
  ESP_LOGCONFIG(TAG, "  Angulo Aberto: %u", this->ang_open_);
  ESP_LOGCONFIG(TAG, "  Angulo Fechado: %u", this->ang_close_);
  LOG_UPDATE_INTERVAL(this);
}

void AUTODOORComponent::loop() {
  pot = analogRead(pot_pin_);
  ES_off = digitalRead(esoff_pin_);
  ES_on = digitalRead(eson_pin_);
  // serial = Serial.read();

  pos = map(pot, 4096, 0, 0, 270);

  // if (Open_CMD == 1 && busy == 0){
  // cmd = 'a';
  // Open_CMD = 0;
  // busy = 1;
  // }

  // if (Close_CMD == 1 && busy == 0){
  //  busy = 1;
  // cmd = 'f';
  // Close_CMD = 0;
  // }

  if ((cmd == 'e' || serial == 'e') && Estado_EM == false) {
    f_e = 1;
    cmd = 'n';
    // Serial.println("F_en");
  }

  if ((cmd == 'd' || serial == 'd') && Estado_EM == true) {
    f_d = 1;
    cmd = 'n';
    // Serial.println("F_de");
  }

  if ((cmd == 'a' || serial == 'a') && busy == false) {
    f_a = 1;
    cmd = 'n';
    busy = true;
    // Serial.println("F_ab");
  }

  if ((cmd == 'f' || serial == 'f') && busy == false) {
    f_f = 1;
    cmd = 'n';
    busy = true;
    // Serial.println("F_fe");
  }

  engate();
  desengate();
  abrir();
  fechar();

  // if((millis() - tempo)>2000){
  //   Serial.print("POT:  ");
  //   Serial.print(pos);
  //   Serial.print("  ES_en:  ");
  //   Serial.print(ES_on);
  //   Serial.print("  ES_de:  ");
  //   Serial.print(ES_off);
  //   Serial.print("    Estado_EM:  ");
  //   Serial.print(Estado_EM);
  //   Serial.print("  Estado_DM:  ");
  //   Serial.println(Estado_DM);
  //
  //   tempo = millis();
  // }
}

void AUTODOORComponent::display() {}

void AUTODOORComponent::update() {
  if (this->writer_.has_value())
    (*this->writer_)(*this);
  this->display();
}

void AUTODOORComponent::engate() {
  if (ES_on == false && f_e == 1) {
    f_e = 2;
    Engage.attach(engage_pin_);
    Engage.writeMicroseconds(stop_vel - max_vel);
    // Serial.println("engatando");
  } else if (ES_on == true && f_e == 2) {
    f_e = 0;
    Engage.writeMicroseconds(stop_vel);
    Estado_EM = true;
    // Serial.println("Estado EM :");
    // Serial.println(Estado_EM);
    // Engage.detach();
  }
}
void AUTODOORComponent::desengate() {
  if (ES_off == false && f_d == 1) {
    f_d = 2;
    Engage.attach(engage_pin_);
    Engage.writeMicroseconds(stop_vel + max_vel);
    // Serial.println("desengatando");
  } else if (ES_off == true && f_d == 2) {
    f_d = 0;
    Engage.writeMicroseconds(stop_vel);
    Estado_EM = false;
    // Serial.println("Estado EM :");
    // Serial.println(Estado_EM);
    // Engage.detach();
  }
}
void AUTODOORComponent::abrir() {
  if (f_a == 1) {
    cmd = 'e';
    f_a = 2;
    analogWrite(drive_pin_, engage_vel_dm);
    digitalWrite(dir_pin_, 1);
  }

  else if (f_a == 2 && Estado_EM == true) {
    f_a = 3;
    analogWrite(drive_pin_, drive_vel_dm);
    digitalWrite(dir_pin_, 1);
    // Serial.println("abrindo");
  }

  else if (pos >= ang_open_ && f_a == 3) {
    f_a = 4;
    analogWrite(drive_pin_, stop_vel_dm);
    // Serial.println("aberto");
  }

  else if (f_a == 4) {
    cmd = 'd';
    f_a = 0;
    Estado_DM = 'a';
    busy = 0;
  }
}
void AUTODOORComponent::fechar() {
  if (f_f == 1) {
    cmd = 'e';
    f_f = 2;
    analogWrite(drive_pin_, engage_vel_dm);
    digitalWrite(dir_pin_, 0);
  }

  else if (f_f == 2 && Estado_EM == true) {
    f_f = 3;
    analogWrite(drive_pin_, drive_vel_dm);
    digitalWrite(dir_pin_, 0);
    // Serial.println("fechando");
  }

  else if (pos <= ang_close_ && f_f == 3) {
    f_f = 4;
    analogWrite(drive_pin_, stop_vel_dm);
    // Serial.println("fechado");
  }

  else if (f_f == 4) {
    cmd = 'd';
    f_f = 0;
    Estado_DM = 'f';
    busy = 0;
  }
}

}  // namespace auto_door
}  // namespace esphome
