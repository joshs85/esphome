#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/i2c/i2c_bus.h"

namespace esphome {
namespace m5unit_acmeasure {

/// This class implements support for the m5unit_acmeasure sensor.
class ACMeasureComponent : public PollingComponent, public i2c::I2CDevice {
 public:
  void set_voltage_sensor(sensor::Sensor *t) { this->voltage_sensor_ = t; }
  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  void setup() override;
  float get_setup_priority() const override;
  void update() override;

 protected:
  sensor::Sensor *voltage_sensor_{nullptr};
  enum ErrorCode {
    NONE = 0,
    COMMUNICATION_FAILED,
    STATUS_FAILED,
  } error_code_{NONE};
};

}  // namespace m5unit_acmeasure
}  // namespace esphome
