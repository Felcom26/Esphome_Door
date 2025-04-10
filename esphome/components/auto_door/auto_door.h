#pragma once
#include <ESP32Servo.h>
#include <driver/ledc.h>
//#include "analogWrite.h"
#include "esphome/core/component.h"
#include "esphome/core/time.h"
#include "esphome/core/gpio.h"
#include "esphome/core/automation.h"
#include "esphome/components/sensor/sensor.h"
//#include "esphome/components/template/switch/template_switch.h"

namespace esphome {
namespace auto_door {

// class AUTODOORComponent;

class AUTODOORComponent : public PollingComponent {
 public:
  using auto_door_writer_t = std::function<void(AUTODOORComponent &)>;
  void set_writer(auto_door_writer_t &&writer);

  void set_position_sensor(sensor::Sensor *sensor);

  void update() override;

  void setup() override;

  void dump_config() override;

  void loop() override;

  float get_setup_priority() const override;

  void set_speed(float speed);

  void engate();
  void desengate();
  void abrir();
  void fechar();

  void DEBUG_prints();

  void CMD_abrir();
  void CMD_fechar();

  // void set_drive_pin(GPIOPin *drive_pin);
  // void set_dir_pin(GPIOPin *dir_pin);
  // void set_rotsen_pin(GPIOPin *rotsen_pin);
  // void set_engage_pin(GPIOPin *engage_pin);
  // void set_pot_pin(GPIOPin *pot_pin);
  // void set_esoff_pin(GPIOPin *esoff_pin);
  // void set_eson_pin(GPIOPin *eson_pin);

  void set_ang_open(uint8_t ang_open);
  void set_ang_close(uint8_t ang_close);

 protected:
  auto_door_writer_t writer_;
  sensor::Sensor *position_sensor_{nullptr};
  // GPIOPin *drive_pin_{nullptr};
  // GPIOPin *dir_pin_{nullptr};
  // GPIOPin *rotsen_pin_{nullptr};
  // GPIOPin *engage_pin_{nullptr};
  // GPIOPin *pot_pin_{nullptr};
  // GPIOPin *esoff_pin_{nullptr};
  // GPIOPin *eson_pin_{nullptr};

#define drive_pin_ 12
#define dir_pin_ 14
#define rotsen_pin_ 27
#define engage_pin_ 13
#define pot_pin_ 35
#define esoff_pin_ 16
#define eson_pin_ 17

  uint8_t ang_open_{238};
  uint8_t ang_close_{118};
};

}  // namespace auto_door
}  // namespace esphome
