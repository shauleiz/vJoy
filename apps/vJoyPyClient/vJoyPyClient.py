"""Example script for the Python vJoy API.

Ensure that the pyvjoy.pyc file is available in your Python path. The easiest
way to do this may be to copy that file to the same directory as your script.
(The pyvjoy.pyc file in this example directory was manually built for
x64 Python and copied).
"""

import datetime
import time

import pyvjoy

SLEEP_BETWEEN_INPUTS = datetime.timedelta(milliseconds=100).total_seconds()


def SleepBetweenInputs(sleep_s=SLEEP_BETWEEN_INPUTS):
    time.sleep(sleep_s)


def PrintFnNameAndResult(fn, *args, **kwargs):
    print(fn.__name__, fn(*args, **kwargs))


def RunDemo():
    PrintFnNameAndResult(pyvjoy.GetvJoyVersion)
    PrintFnNameAndResult(pyvjoy.GetvJoyProductString)
    PrintFnNameAndResult(pyvjoy.GetvJoyManufacturerString)
    PrintFnNameAndResult(pyvjoy.GetvJoySerialNumberString)
    try:
        PrintFnNameAndResult(pyvjoy.DriverMatch)
    except RuntimeError as e:
        print(e)
    PrintFnNameAndResult(pyvjoy.vJoyFfbCap)
    PrintFnNameAndResult(pyvjoy.GetvJoyMaxDevices)
    PrintFnNameAndResult(pyvjoy.GetNumberExistingVJD)
    PrintFnNameAndResult(pyvjoy.ResetAll)

    vjoy_1 = pyvjoy.VjoyDevice(1)
    PrintFnNameAndResult(vjoy_1.GetOwnerPid)
    print("vJoy device 1 num buttons:", vjoy_1.device_info.num_buttons)
    print("vJoy device 1 num axes:", vjoy_1.device_info.num_axes)
    print("Axes usage by position:", vjoy_1.device_info.axis_usage_by_position)
    print("Axes min values:", vjoy_1.device_info.axis_min_by_position)
    print("Axes max values:", vjoy_1.device_info.axis_max_by_position)
    print("vJoy device 1 num POV hats:", vjoy_1.device_info.num_pov_hats)
    print("POV continuous hats:", vjoy_1.device_info.hat_is_continuous)
    PrintFnNameAndResult(vjoy_1.GetVJDStatus)

    # Acquire device and change some controls.
    # You can monitor these using JoyMonitor.exe.
    vjoy_1.AcquireVJD()
    try:
        # Setting multiple inputs at the same time.
        # These are raw values, as used by the C API.
        position_data = pyvjoy.JoystickPosition()
        position_data.wAxisX = 1000
        position_data.wSlider = 2000
        position_data.wDial = 3000  # a.k.a slider 2.
        position_data.lButtons = 0xABCD
        position_data.bHats = 0x0123
        vjoy_1.UpdateVJD(position_data)
        SleepBetweenInputs(1)

        # Setting individual inputs. See notes on helper methods.
        for btn_i in range(1, vjoy_1.device_info.num_buttons + 1):
            vjoy_1.SetBtn(True, btn_i)
            SleepBetweenInputs()
            vjoy_1.SetBtn(False, btn_i)
        vjoy_1.ResetButtons()
        for pov_position in [-1, 0, 5_000, 10_000, 20_000, 30_000, 35_999]:
            for pov_i in range(1, vjoy_1.device_info.num_pov_hats + 1):
                # This is a helper method. For continuous hats, the value is set
                # directly. For discrete hats, the closest discrete position is
                # set.
                vjoy_1.SetPov(pov_position, pov_i)
                SleepBetweenInputs()
        vjoy_1.ResetPovs()
        for axis_value in [0, 0.25, 0.5, 0.75, 1]:
            for axis_i in range(1, vjoy_1.device_info.num_axes + 1):
                # This is a helper method. axis_value is a normalized value
                # i.e. in range [0, 1]. You don't need to know the axis
                # logical min and max values.
                vjoy_1.SetAxisByNumber(axis_value, axis_i)
                SleepBetweenInputs()
        for axis_usage in vjoy_1.device_info.axis_usage_by_position:
            # Equivalent of the C API method SetAxis.
            vjoy_1.SetAxisByUsage(0, pyvjoy.AxisUsage(axis_usage))
        vjoy_1.ResetVJD()
    finally:
        vjoy_1.RelinquishVJD()
    PrintFnNameAndResult(pyvjoy.ResetAll)


if __name__ == "__main__":
    RunDemo()
