// Wraps VJoyInterface with a modern C++ object-oriented interface.

#pragma once

#include <Windows.h>

#include <array>
#include <exception>
#include <format>
#include <string>
#include <tuple>

#include "public.h"

enum VjdStat;

namespace vjoy_modern {

template <typename T>
class InvalidArgumentError : public std::invalid_argument {
 public:
  InvalidArgumentError(const char* error_msg, const T& argument)
      : std::invalid_argument(error_msg), argument_(argument) {}
  const char* what() {
    return std::format("{}: {}", std::string(std::runtime_error::what()),
                       argument_);
  }

 private:
  T argument_;
};

// All the following functions will throw runtime error if they can't
// communicate with the driver or get an invalid response.
std::wstring GetvJoyProductString();
std::wstring GetvJoyManufacturerString();
std::wstring GetvJoySerialNumberString();
// Returns a tuple of the DLL version and driver version if matching.
// Raises runtime error if the driver doesn't match the library.
std::tuple<WORD, WORD> DriverMatch();
bool vJoyFfbCap();
int GetvJoyMaxDevices();
int GetNumberExistingVJD();

struct VjoyDeviceInfo {
  int num_buttons_;
  int num_axes_;
  int num_pov_hats_;
  static constexpr int kMaxHats = 4;  // DirectInput limit.
  std::array<bool, kMaxHats> hat_is_continuous_;
  static constexpr int kMaxAxes = 8;  // DirectInput limit.
  std::array<UINT, kMaxAxes> axis_usage_by_position_;
  std::array<LONG, kMaxAxes> axis_min_by_position_;
  std::array<LONG, kMaxAxes> axis_max_by_position_;
};

enum class AxisUsage : UINT {
  X = HID_USAGE_X,
  Y = HID_USAGE_Y,
  Z = HID_USAGE_Z,
  Rx = HID_USAGE_RX,
  Ry = HID_USAGE_RY,
  Rz = HID_USAGE_RZ,
  SLIDER_0 = HID_USAGE_SL0,
  SLIDER_1 = HID_USAGE_SL1,
  DIAL = HID_USAGE_SL1,
  // The rest can be added if the need arises.
};

// All the functions in this class will throw runtime error if they can't
// communicate with the driver.
class VjoyDevice {
 public:
  // Initialize with the given device number (1-indexed).
  // Throws InvalidArgumentError if specified device does not exist.
  explicit VjoyDevice(int device_number);
  // Returns a copy - suitable for Python binding.
  VjoyDeviceInfo device_info() const;

  // Negative values indicate no owner or an error condition.
  // See GetOwnerPid in vJoyInterface.cpp for details.
  int GetOwnerPid() const;
  VjdStat GetVJDStatus() const;

  void AcquireVJD() const;
  void RelinquishVJD() const;
  void ResetVJD() const;
  void ResetButtons() const;
  void ResetPovs() const;

  // Not a const reference, for 1:1 behavior with Python binding.
  void UpdateVJD(JOYSTICK_POSITION& pData) const;
  // normalized_value lies in [0, 1].
  void SetAxisByUsage(float normalized_value, AxisUsage axis_usage) const;
  // Axis numbers start from 1.
  void SetAxisByNumber(float normalized_value, int axis_number) const;
  // Button numbers start from 1.
  void SetBtn(bool is_on, int button_number) const;
  // POV numbers start from 1.
  // If setting on a discrete hat, the closest of 8 possible discrete positions
  // will be set. See DirectInput documentation for DIJOYSTATE2 for valid
  // values.
  void SetPov(int value, int pov_number) const;

 private:
  void SetAxis(float normalized_value, int usage) const;
  const int device_index_;
  VjoyDeviceInfo device_info_;
};
}  // namespace vjoy_modern