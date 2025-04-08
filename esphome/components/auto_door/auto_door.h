#pragma once
#include "ESP32Servo.h"
#include "analogWrite.h"
#include "esphome/core/component.h"
#include "esphome/core/time.h"
#include "esphome/core/gpio.h"

namespace esphome {
namespace auto_door {

class AUTODOORComponent;

using auto_door_writer_t = std::function<void(AUTODOORComponent &)>;

class AUTODOORComponent : public PollingComponent,
                          public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW,
                                                spi::CLOCK_PHASE_LEADING, spi::DATA_RATE_1MHZ> {
 public:
  void set_writer(auto_door_writer_t &&writer);

  void setup() override;

  void dump_config() override;

  void update() override;

  void loop() override;

  float get_setup_priority() const override;

  void display();

  void engate();
  void desengate();
  void abrir();
  void fechar();

  void set_drive_pin(GPIOPin *drive_pin);
  void set_dir_pin(GPIOPin *dir_pin);
  void set_rotsen_pin(GPIOPin *rotsen_pin);
  void set_engage_pin(GPIOPin *engage_pin);
  void set_pot_pin(GPIOPin *pot_pin);
  void set_esoff_pin(GPIOPin *esoff_pin);
  void set_eson_pin(GPIOPin *eson_pin);

  void set_ang_open(uint8_t ang_open);
  void set_ang_close(uint8_t ang_close);

 protected:
  GPIOPin *drive_pin_{nullptr};
  GPIOPin *dir_pin_{nullptr};
  GPIOPin *rotsen_pin_{nullptr};
  GPIOPin *engage_pin_{nullptr};
  GPIOPin *pot_pin_{nullptr};
  GPIOPin *esoff_pin_{nullptr};
  GPIOPin *eson_pin_{nullptr};

  optional<auto_door_writer_t> writer_{};
};

}  // namespace auto_door
}  // namespace esphome
