/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    usb.c

Abstract:

    Code for handling USB related requests

Author:


Environment:

    kernel mode only

Revision History:

--*/

#include <vjoy.h>

#if defined(EVENT_TRACING)
#include "usb.tmh"
#endif

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, vJoyConfigContReaderForInterruptEndPoint)
#endif


//NTSTATUS
//HidFx2EvtDeviceD0Entry(
//    IN  WDFDEVICE Device,
//    IN  WDF_POWER_DEVICE_STATE PreviousState
//    )
///*++
//
//Routine Description:
//
//    EvtDeviceD0Entry event callback must perform any operations that are
//    necessary before the specified device is used.  It will be called every
//    time the hardware needs to be (re-)initialized.
//
//    This function is not marked pageable because this function is in the
//    device power up path. When a function is marked pagable and the code
//    section is paged out, it will generate a page fault which could impact
//    the fast resume behavior because the client driver will have to wait
//    until the system drivers can service this page fault.
//
//    This function runs at PASSIVE_LEVEL, even though it is not paged.  A
//    driver can optionally make this function pageable if DO_POWER_PAGABLE
//    is set.  Even if DO_POWER_PAGABLE isn't set, this function still runs
//    at PASSIVE_LEVEL.  In this case, though, the function absolutely must
//    not do anything that will cause a page fault.
//
//Arguments:
//
//    Device - Handle to a framework device object.
//
//    PreviousState - Device power state which the device was in most recently.
//        If the device is being newly started, this will be
//        PowerDeviceUnspecified.
//
//Return Value:
//
//    NTSTATUS
//
//--*/
//{
//    PDEVICE_EXTENSION   devContext = NULL;
//    NTSTATUS            status = STATUS_SUCCESS;
//    UCHAR               switchState = 0;
//
//    devContext = GetDeviceContext(Device);
//
//    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_PNP,
//        "HidFx2EvtDeviceD0Entry Enter - coming from %s\n",
//                DbgDevicePowerString(PreviousState));
//
//    //
//    // Retrieve the current switch state and store it in device context
//    //
//    //status = vJoyGetSwitchState(Device, &switchState);
//    //if (!NT_SUCCESS(status)) {
//    //    TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
//    //        "Failed to get current swicth state, status: 0x%x\n", status);
//    //    return status;
//    //}
//
//    devContext->CurrentSwitchState = switchState;
//
//    //
//    // Start the target. This will start the continuous reader
//    //
//    status = WdfIoTargetStart(WdfUsbTargetPipeGetIoTarget(devContext->InterruptPipe));
//    if (NT_SUCCESS(status)) {
//        devContext->IsPowerUpSwitchState = TRUE;
//    }
//
//    TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
//        "HidFx2EvtDeviceD0Entry Exit, status: 0x%x\n", status);
//
//    return status;
//}
//
//
//NTSTATUS
//HidFx2EvtDeviceD0Exit(
//    IN  WDFDEVICE Device,
//    IN  WDF_POWER_DEVICE_STATE TargetState
//    )
///*++
//
//Routine Description:
//
//    This routine undoes anything done in EvtDeviceD0Entry.  It is called
//    whenever the device leaves the D0 state, which happens when the device is
//    stopped, when it is removed, and when it is powered off.
//
//    The device is still in D0 when this callback is invoked, which means that
//    the driver can still touch hardware in this routine.
//
//
//    EvtDeviceD0Exit event callback must perform any operations that are
//    necessary before the specified device is moved out of the D0 state.  If the
//    driver needs to save hardware state before the device is powered down, then
//    that should be done here.
//
//    This function runs at PASSIVE_LEVEL, though it is generally not paged.  A
//    driver can optionally make this function pageable if DO_POWER_PAGABLE is set.
//
//    Even if DO_POWER_PAGABLE isn't set, this function still runs at
//    PASSIVE_LEVEL.  In this case, though, the function absolutely must not do
//    anything that will cause a page fault.
//
//Arguments:
//
//    Device - Handle to a framework device object.
//
//    TargetState - Device power state which the device will be put in once this
//        callback is complete.
//
//Return Value:
//
//    Success implies that the device can be used.  Failure will result in the
//    device stack being torn down.
//
//--*/
//{
//    PDEVICE_EXTENSION         devContext;
//
//    PAGED_CODE();
//
//    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_PNP,
//        "HidFx2EvtDeviceD0Exit Enter- moving to %s\n",
//          DbgDevicePowerString(TargetState));
//
//    devContext = GetDeviceContext(Device);
//
//    WdfIoTargetStop(WdfUsbTargetPipeGetIoTarget(
//        devContext->InterruptPipe), WdfIoTargetCancelSentIo);
//
//    TraceEvents(TRACE_LEVEL_ERROR, DBG_PNP,
//        "HidFx2EvtDeviceD0Exit Exit\n");
//
//    return STATUS_SUCCESS;
//}
//

//NTSTATUS
//vJoyGetSwitchState(
//    IN WDFDEVICE Device,
//    OUT PUCHAR SwitchState
//    )
///*++
//
//Routine Description:
//
//    This function gets the swicth state of teh USB device
//
//Arguments:
//
//    Device - Handle to a framework device object.
//
//    SwitchState - Pointer to a variable that receives the switch state
//
//Return Value:
//
//    Success implies that the device can be used.  Failure will result in the
//    device stack being torn down.
//
//--*/
//{
//    PDEVICE_EXTENSION            devContext = NULL;
//    NTSTATUS                     status = STATUS_SUCCESS;
//    WDF_MEMORY_DESCRIPTOR        memDesc;
//    WDF_USB_CONTROL_SETUP_PACKET controlSetupPacket;
//    ULONG                        bytesTransferred = 0;
//
//    devContext = GetDeviceContext(Device);
//
//    //
//    // set the segment state on the USB device
//    //
//    WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR(&controlSetupPacket,
//                                        BmRequestDeviceToHost,
//                                        BmRequestToDevice,
//                                        HIDFX2_READ_SWITCH_STATE, // Request
//                                        0, // Value
//                                        0); // Index
//
//    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&memDesc,
//                                    SwitchState,
//                                    sizeof(UCHAR));
//
//    status = WdfUsbTargetDeviceSendControlTransferSynchronously(
//                                                devContext->UsbDevice,
//                                                NULL, // Optional WDFREQUEST
//                                                NULL, // PWDF_REQUEST_SEND_OPTIONS
//                                                &controlSetupPacket,
//                                                &memDesc,
//                                                &bytesTransferred
//                                                );
//
//    if(!NT_SUCCESS(status)) {
//        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
//            "GetSwitchState: Failed to read switch state - 0x%x \n", status);
//
//    } else {
//        TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL,
//            "GetSwitchState: Switch state is 0x%x\n", *SwitchState);
//    }
//
//    return status;
//}
//
//

PCHAR
DbgDevicePowerString(
    IN WDF_POWER_DEVICE_STATE Type
)
/*++

Updated Routine Description:
    DbgDevicePowerString does not change in this stage of the function driver.

--*/
{
    switch (Type) {
        case WdfPowerDeviceInvalid:
            return "WdfPowerDeviceInvalid";
        case WdfPowerDeviceD0:
            return "WdfPowerDeviceD0";
        case PowerDeviceD1:
            return "WdfPowerDeviceD1";
        case WdfPowerDeviceD2:
            return "WdfPowerDeviceD2";
        case WdfPowerDeviceD3:
            return "WdfPowerDeviceD3";
        case WdfPowerDeviceD3Final:
            return "WdfPowerDeviceD3Final";
        case WdfPowerDevicePrepareForHibernation:
            return "WdfPowerDevicePrepareForHibernation";
        case WdfPowerDeviceMaximum:
            return "PowerDeviceMaximum";
        default:
            return "UnKnown Device Power State";
    }
}



//VOID
//vJoyGetPositionData(
//	IN HID_INPUT_REPORT	*HidReport, 
//	IN DEVICE_EXTENSION *pDevContext
//	)
//{
//			if (!pDevContext->PrevVal_Rudder) pDevContext->PrevVal_Rudder=1;
//			if (!pDevContext->PrevVal_X) pDevContext->PrevVal_X=11;
//			if (!pDevContext->PrevVal_Y) pDevContext->PrevVal_Y=5;
//
//			HidReport->InputReport.bThrottle = pDevContext->PrevVal_Rudder + 10;
//			HidReport->InputReport.bAxisX = pDevContext->PrevVal_X + 11;
//			HidReport->InputReport.bAxisY = pDevContext->PrevVal_Y + 12;
//
//			pDevContext->PrevVal_Rudder = HidReport->InputReport.bThrottle;
//			pDevContext->PrevVal_X		= HidReport->InputReport.bAxisX;
//			pDevContext->PrevVal_Y		= HidReport->InputReport.bAxisY;
//
//}

