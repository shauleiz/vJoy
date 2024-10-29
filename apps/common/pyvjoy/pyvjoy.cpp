#include <Windows.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "vjoyinterface.h"
#include "vjoymodern.h"

#pragma comment(lib, "vJoyInterfaceStat.lib")

namespace py = pybind11;

namespace pyvjoy {
PYBIND11_MODULE(pyvjoy, pyvjoy_m) {
  pyvjoy_m.doc() = "Python client library for VJoy";

  // Enums.
  py::enum_<VjdStat>(pyvjoy_m, "VjdStat")
      .value("VJD_STAT_OWN", VjdStat::VJD_STAT_OWN)
      .value("VJD_STAT_FREE", VjdStat::VJD_STAT_FREE)
      .value("VJD_STAT_BUSY", VjdStat::VJD_STAT_BUSY)
      .value("VJD_STAT_MISS", VjdStat::VJD_STAT_MISS)
      .value("VJD_STAT_UNKN", VjdStat::VJD_STAT_UNKN);

  py::enum_<vjoy_modern::AxisUsage>(pyvjoy_m, "AxisUsage")
      .value("X", vjoy_modern::AxisUsage::X)
      .value("Y", vjoy_modern::AxisUsage::Y)
      .value("Z", vjoy_modern::AxisUsage::Z)
      .value("Rx", vjoy_modern::AxisUsage::Rx)
      .value("Ry", vjoy_modern::AxisUsage::Ry)
      .value("Rz", vjoy_modern::AxisUsage::Rz)
      .value("SLIDER_0", vjoy_modern::AxisUsage::SLIDER_0)
      .value("SLIDER_1", vjoy_modern::AxisUsage::SLIDER_1)
      .value("DIAL", vjoy_modern::AxisUsage::DIAL);

  // Structs.
  // Less commonly used fields are not currently exported.
  py::class_<JOYSTICK_POSITION>(pyvjoy_m, "JoystickPosition")
      .def(py::init<>())
      .def_readwrite("bDevice", &JOYSTICK_POSITION::bDevice)
      .def_readwrite("wAxisX", &JOYSTICK_POSITION::wAxisX)
      .def_readwrite("wAxisY", &JOYSTICK_POSITION::wAxisY)
      .def_readwrite("wAxisZ", &JOYSTICK_POSITION::wAxisZ)
      .def_readwrite("wAxisXRot", &JOYSTICK_POSITION::wAxisXRot)
      .def_readwrite("wAxisYRot", &JOYSTICK_POSITION::wAxisYRot)
      .def_readwrite("wAxisZRot", &JOYSTICK_POSITION::wAxisZRot)
      .def_readwrite("wSlider", &JOYSTICK_POSITION::wSlider)
      .def_readwrite("wDial", &JOYSTICK_POSITION::wDial)
      .def_readwrite("lButtons", &JOYSTICK_POSITION::lButtons)
      .def_readwrite("bHats", &JOYSTICK_POSITION::bHats)
      .def_readwrite("bHatsEx1", &JOYSTICK_POSITION::bHatsEx1)
      .def_readwrite("bHatsEx2", &JOYSTICK_POSITION::bHatsEx2)
      .def_readwrite("bHatsEx3", &JOYSTICK_POSITION::bHatsEx3)
      .def_readwrite("lButtonsEx1", &JOYSTICK_POSITION::lButtonsEx1)
      .def_readwrite("lButtonsEx2", &JOYSTICK_POSITION::lButtonsEx2)
      .def_readwrite("lButtonsEx3", &JOYSTICK_POSITION::lButtonsEx3);

  // Generic driver functions.
  pyvjoy_m.def("GetvJoyVersion", &vJoyNS::GetvJoyVersion,
               "Returns the version of the installed vJoy driver.");
  pyvjoy_m.def("vJoyEnabled", &vJoyNS::vJoyEnabled,
               "Returns True if vJoy is enabled.");
  pyvjoy_m.def("GetvJoyProductString", &vjoy_modern::GetvJoyProductString,
               "Returns the vJoy product string.");
  pyvjoy_m.def("GetvJoyManufacturerString",
               &vjoy_modern::GetvJoyManufacturerString,
               "Returns the vJoy manufacturer string.");
  pyvjoy_m.def("GetvJoySerialNumberString",
               &vjoy_modern::GetvJoySerialNumberString,
               "Returns the vJoy serial number string.");
  pyvjoy_m.def("DriverMatch", &vjoy_modern::DriverMatch,
               "Returns the vJoy library and driver versions if they match.");
  pyvjoy_m.def(
      "vJoyFfbCap", &vjoy_modern::vJoyFfbCap,
      "Returns true if force feedback is enabled (for all vJoy devices).");
  pyvjoy_m.def("GetvJoyMaxDevices", &vjoy_modern::GetvJoyMaxDevices,
               "Returns the maximum number of vJoy devices.");
  pyvjoy_m.def("GetNumberExistingVJD", &vjoy_modern::GetNumberExistingVJD,
               "Returns the number of active vJoy devices.");
  pyvjoy_m.def("ResetAll", &vJoyNS::ResetAll,
               "Resets all controls to predefined values in all vJoy devices.");

  // Information about a vJoy device.
  py::class_<vjoy_modern::VjoyDeviceInfo>(pyvjoy_m, "VjoyDeviceInfo")
      .def_readonly("num_buttons", &vjoy_modern::VjoyDeviceInfo::num_buttons_)
      .def_readonly("num_axes", &vjoy_modern::VjoyDeviceInfo::num_axes_)
      .def_readonly("num_pov_hats", &vjoy_modern::VjoyDeviceInfo::num_pov_hats_)
      .def_readonly("hat_is_continuous",
                    &vjoy_modern::VjoyDeviceInfo::hat_is_continuous_)
      .def_readonly("axis_usage_by_position",
                    &vjoy_modern::VjoyDeviceInfo::axis_usage_by_position_)
      .def_readonly("axis_min_by_position",
                    &vjoy_modern::VjoyDeviceInfo::axis_min_by_position_)
      .def_readonly("axis_max_by_position",
                    &vjoy_modern::VjoyDeviceInfo::axis_max_by_position_);

  // A single vJoy device.
  py::class_<vjoy_modern::VjoyDevice>(pyvjoy_m, "VjoyDevice")
      .def(py::init<int>())
      .def_property_readonly("device_info",
                             &vjoy_modern::VjoyDevice::device_info)
      .def("GetOwnerPid", &vjoy_modern::VjoyDevice::GetOwnerPid)
      .def("GetVJDStatus", &vjoy_modern::VjoyDevice::GetVJDStatus)
      .def("AcquireVJD", &vjoy_modern::VjoyDevice::AcquireVJD)
      .def("RelinquishVJD", &vjoy_modern::VjoyDevice::RelinquishVJD)
      .def("ResetVJD", &vjoy_modern::VjoyDevice::ResetVJD)
      .def("ResetButtons", &vjoy_modern::VjoyDevice::ResetButtons)
      .def("ResetPovs", &vjoy_modern::VjoyDevice::ResetPovs)
      .def("UpdateVJD", &vjoy_modern::VjoyDevice::UpdateVJD)
      .def("SetAxisByUsage", &vjoy_modern::VjoyDevice::SetAxisByUsage)
      .def("SetAxisByNumber", &vjoy_modern::VjoyDevice::SetAxisByNumber)
      .def("SetBtn", &vjoy_modern::VjoyDevice::SetBtn)
      .def("SetPov", &vjoy_modern::VjoyDevice::SetPov);
}
}  // namespace pyvjoy