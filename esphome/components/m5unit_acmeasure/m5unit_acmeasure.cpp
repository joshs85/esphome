#include "m5unit_acmeasure.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace m5unit_acmeasure {

static const char *const TAG = "m5unit_acmeasure.sensor";

static const uint8_t UNIT_ACMEASURE_ERROR_STATUS_REG = 0xFC;
static const uint8_t UNIT_ACMEASURE_VOLTAGE_REG = 0x60;
static const uint8_t KMETER_FIRMWARE_VERSION_REG = 0xFE;

void ACMeasureComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ACMeasure...");
  this->error_code_ = NONE;

  // Mark as not failed before initializing. Some devices will turn off sensors to save on batteries
  // and when they come back on, the COMPONENT_STATE_FAILED bit must be unset on the component.
  if ((this->component_state_ & COMPONENT_STATE_MASK) == COMPONENT_STATE_FAILED) {
    this->component_state_ &= ~COMPONENT_STATE_MASK;
    this->component_state_ |= COMPONENT_STATE_CONSTRUCTION;
  }

  auto err = this->bus_->writev(this->address_, nullptr, 0);
  if (err == esphome::i2c::ERROR_OK) {
    ESP_LOGCONFIG(TAG, "Could write to the address %d.", this->address_);
  } else {
    ESP_LOGCONFIG(TAG, "Could not write to the address %d.", this->address_);
    this->error_code_ = COMMUNICATION_FAILED;
    this->mark_failed();
    return;
  }

  uint8_t read_buf[4] = {1};
  if (!this->read_bytes(UNIT_ACMEASURE_ERROR_STATUS_REG, read_buf, 1)) {
    ESP_LOGCONFIG(TAG, "Could not read from the device.");
    this->error_code_ = COMMUNICATION_FAILED;
    this->mark_failed();
    return;
  }
  ESP_LOGCONFIG(TAG, "The device was successfully setup.");
}

float ACMeasureComponent::get_setup_priority() const { return setup_priority::DATA; }

void ACMeasureComponent::update() {
  uint8_t read_buf[4] = {0};
  this->read_bytes(UNIT_ACMEASURE_ERROR_STATUS_REG, read_buf, 1);
  if (read_buf[0] != 0) {
    uint8_t read_buf[4];
    if (this->voltage_sensor_ != nullptr) {
      if (!this->read_bytes(UNIT_ACMEASURE_VOLTAGE_REG, read_buf, 2)) {
        ESP_LOGW(TAG, "Error reading voltage.");
      } else {
        uint16_t value = read_buf[0] | (read_buf[1] << 8);
        float final_value = value / 100;
        ESP_LOGV(TAG, "Got voltage=%.2f °V", final_value);
        this->voltage_sensor_->publish_state(final_value);
      }
    }
  } else {
    ESP_LOGW(TAG, "Got data not ready when checking.");
  }
}

}  // namespace m5unit_acmeasure
}  // namespace esphome
