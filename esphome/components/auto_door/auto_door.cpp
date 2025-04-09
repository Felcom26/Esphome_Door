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
int ha_pos;

float tempo;

bool busy = false;

const int pwmFreq = 1000;     // 1kHz
const int pwmResolution = 8;  // 8 bits = valores de 0 a 255
const int chan_drive_pin_ = 0;

namespace esphome {
namespace auto_door {

static const char *const TAG = "auto_door";

float AUTODOORComponent::get_setup_priority() const { return setup_priority::PROCESSOR; }

// void AUTODOORComponent::set_drive_pin(GPIOPin *drive_pin) {this->drive_pin_ = drive_pin; }
// void AUTODOORComponent::set_dir_pin(GPIOPin *dir_pin) {this->dir_pin_ = dir_pin; }
// void AUTODOORComponent::set_rotsen_pin(GPIOPin *rotsen_pin) {this->rotsen_pin_ = rotsen_pin; }
// void AUTODOORComponent::set_engage_pin(GPIOPin *engage_pin) {this->engage_pin_ = engage_pin; }
// void AUTODOORComponent::set_pot_pin(GPIOPin *pot_pin) {this->pot_pin_ = pot_pin; }
// void AUTODOORComponent::set_esoff_pin(GPIOPin *esoff_pin) {this->esoff_pin_ = esoff_pin; }
// void AUTODOORComponent::set_eson_pin(GPIOPin *eson_pin) {this->eson_pin_ = eson_pin; }

// void AUTODOORComponent::set_writer(auto_door_writer_t &&writer) { this->writer_ = writer; }
// void AUTODOORComponent::set_writer(Trigger<AUTODOORComponent *> *t) {
//   this->writer_ = t;
// }

// void AUTODOORComponent::set_writer(std::function<void()> &&writer) { this->writer_ = writer; }
void AUTODOORComponent::set_writer(auto_door_writer_t &&writer) { this->writer_ = writer; }

void AUTODOORComponent::update() {
  // ESP_LOGD(TAG, "Executando update() - verificando comando...");
  if (writer_)
    writer_(*this);
}

void AUTODOORComponent::set_ang_open(uint8_t ang_open) { this->ang_open_ = ang_open; }
void AUTODOORComponent::set_ang_close(uint8_t ang_close) { this->ang_close_ = ang_close; }

void AUTODOORComponent::set_position_sensor(sensor::Sensor *sensor) { this->position_sensor_ = sensor; }

void AUTODOORComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Auto_Door...");

  // Pinos de EndStop
  pinMode(esoff_pin_, INPUT_PULLUP);
  pinMode(eson_pin_, INPUT_PULLUP);

  // Pinos Drive Motor
  pinMode(drive_pin_, OUTPUT);
  pinMode(dir_pin_, OUTPUT);
  pinMode(rotsen_pin_, INPUT);

  // Inicia Motores
  ledcSetup(chan_drive_pin_, pwmFreq, pwmResolution);
  ledcAttachPin(drive_pin_, chan_drive_pin_);
  Engage.attach(engage_pin_);

  // Desliga Motores
  Engage.writeMicroseconds(stop_vel);
  ledcWrite(chan_drive_pin_, stop_vel_dm);
  digitalWrite(dir_pin_, 0);
  delay(1000);

  // Desengata motor para iniciar
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

  pos = map(pot, 4096, 0, 0, 270);
  ha_pos = map(pos, ang_close_, ang_open_, 0, 100);

  if (cmd == 'e' && Estado_EM == false) {
    f_e = 1;
    cmd = 'n';
  }

  if (cmd == 'd' && Estado_EM == true) {
    f_d = 1;
    cmd = 'n';
  }

  if (cmd == 'a' && busy == false) {
    f_a = 1;
    cmd = 'n';
    busy = true;
  }

  if (cmd == 'f' && busy == false) {
    f_f = 1;
    cmd = 'n';
    busy = true;
  }

  engate();
  desengate();
  abrir();
  fechar();

  if ((millis() - tempo) > 1000) {
    DEBUG_prints();
    tempo = millis();
  }
}

void AUTODOORComponent::DEBUG_prints() {
  ESP_LOGD(TAG, "Flag A: %d", f_a);
  ESP_LOGD(TAG, "Flag F: %d", f_f);
  ESP_LOGD(TAG, "Flag E: %d", f_e);
  ESP_LOGD(TAG, "Flag D: %d", f_d);
  // ESP_LOGD(TAG, "Estado_DM: %s", Estado_DM);
  ESP_LOGD(TAG, "Estado_EM: %d", Estado_EM);
  ESP_LOGD(TAG, "Posição do sensor: %d", pos);
  ESP_LOGD(TAG, "ES_on: %d", ES_on);
  ESP_LOGD(TAG, "ES_off: %d", ES_off);
  if (this->position_sensor_ != nullptr) {
    this->position_sensor_->publish_state(ha_pos);
  }
}

void AUTODOORComponent::engate() {
  if (ES_on == false && f_e == 1) {
    f_e = 2;
    Engage.attach(engage_pin_);
    Engage.writeMicroseconds(stop_vel - max_vel);
    ESP_LOGI(TAG, "engatando");
  } else if (ES_on == true && f_e == 2) {
    f_e = 0;
    Engage.writeMicroseconds(stop_vel);
    Estado_EM = true;
    ESP_LOGI(TAG, "engatado");
    // Engage.detach();
  }
}
void AUTODOORComponent::desengate() {
  if (ES_off == false && f_d == 1) {
    f_d = 2;
    Engage.attach(engage_pin_);
    Engage.writeMicroseconds(stop_vel + max_vel);
    ESP_LOGI(TAG, "desengatando");
  } else if (ES_off == true && f_d == 2) {
    f_d = 0;
    Engage.writeMicroseconds(stop_vel);
    Estado_EM = false;
    ESP_LOGI(TAG, "desengatado");
    // Engage.detach();
  }
}
void AUTODOORComponent::abrir() {
  if (f_a == 1) {
    cmd = 'e';
    f_a = 2;
    ledcWrite(chan_drive_pin_, engage_vel_dm);
    // digitalWrite(drive_pin_, 0);
    digitalWrite(dir_pin_, 1);
  }

  else if (f_a == 2 && Estado_EM == true) {
    f_a = 3;
    ledcWrite(chan_drive_pin_, drive_vel_dm);
    // digitalWrite(drive_pin_, 0);
    digitalWrite(dir_pin_, 1);
    ESP_LOGI(TAG, "Abrindo");
  }

  else if (pos >= ang_open_ && f_a == 3) {
    f_a = 4;
    ledcWrite(chan_drive_pin_, stop_vel_dm);
    // digitalWrite(drive_pin_, 1);
    ESP_LOGI(TAG, "Aberto");
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
    ledcWrite(chan_drive_pin_, engage_vel_dm);
    // digitalWrite(drive_pin_, 0);
    digitalWrite(dir_pin_, 0);
  }

  else if (f_f == 2 && Estado_EM == true) {
    f_f = 3;
    ledcWrite(chan_drive_pin_, drive_vel_dm);
    // digitalWrite(drive_pin_, 0);
    digitalWrite(dir_pin_, 0);
    ESP_LOGI(TAG, "fechando");
  }

  else if (pos <= ang_close_ && f_f == 3) {
    f_f = 4;
    ledcWrite(chan_drive_pin_, stop_vel_dm);
    // digitalWrite(drive_pin_, 1);
    ESP_LOGI(TAG, "fechado");
  }

  else if (f_f == 4) {
    cmd = 'd';
    f_f = 0;
    Estado_DM = 'f';
    busy = 0;
  }
}

void AUTODOORComponent::CMD_abrir() {
  cmd = 'a';
  ESP_LOGI(TAG, "CMD_ABRIR");
  // ledcWrite(chan_drive_pin_, stop_vel_dm);
}
void AUTODOORComponent::CMD_fechar() {
  cmd = 'f';
  ESP_LOGI(TAG, "CMD_FECHAR");
  // ledcWrite(chan_drive_pin_, drive_vel_dm);
}

void AUTODOORComponent::set_speed(float speed) {
  // Aqui você implementa o controle do motor como quiser
  ESP_LOGD("SPEED", "Velocidade recebida: %.1f", speed);
}

}  // namespace auto_door
}  // namespace esphome
