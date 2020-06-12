/*++

Copyright (c) Shaul Eizikovich.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    hid.c

Abstract:

    Code for handling HID related requests

Author:


Environment:

    kernel mode only

Revision History:

--*/

#define USE_EXTERNAL_HARDCODED_HID_REPORT_DESCRIPTOR
//#define USE_HARDCODED_HID_REPORT_DESCRIPTOR_5
#define					NameSize 300

#include <vjoy.h>

#if defined(EVENT_TRACING)
#include "hid.tmh"
#endif

#ifdef ALLOC_PRAGMA
#pragma alloc_text( PAGE, vJoySetFeature)
#pragma alloc_text( PAGE, SendVendorCommand)
#pragma alloc_text (PAGE, vJoyEvtIoDeviceControl)
#pragma alloc_text (PAGE, vJoyCreateControlDevice)
#pragma alloc_text (PAGE, vJoyDeleteControlDevice)
#pragma alloc_text (PAGE, vJoyEvtDeviceContextCleanup)
#pragma alloc_text (PAGE, GetReportDescriptorFromRegistry)
#pragma alloc_text (PAGE, GetInitValueFromRegistry)
#pragma alloc_text (PAGE, CalcInitValue)
//#pragma alloc_text (PAGE, UpdateCollections)
#pragma alloc_text (PAGE, vJoyEvtDeviceReleaseHardware)
#pragma alloc_text (PAGE, vJoyEvtDevicePrepareHardware)
#pragma alloc_text (PAGE, vJoyEvtDeviceSelfManagedIoFlush)
#pragma alloc_text (PAGE, vJoyEvtDevicePnpStateChange)
#endif

// Global handle to the Control Device used for sideband communication
WDFDEVICE	g_ControlDevice = NULL;

VOID
vJoyEvtInternalDeviceControl(
    IN WDFQUEUE     Queue,
    IN WDFREQUEST   Request,
    IN size_t       OutputBufferLength,
    IN size_t       InputBufferLength,
    IN ULONG        IoControlCode
)
/*++

Routine Description:

    This event is called when the framework receives
    IRP_MJ_INTERNAL DEVICE_CONTROL requests from the system.

Arguments:

    Queue - Handle to the framework queue object that is associated
            with the I/O request.
    Request - Handle to a framework request object.

    OutputBufferLength - length of the request's output buffer,
                        if an output buffer is available.
    InputBufferLength - length of the request's input buffer,
                        if an input buffer is available.

    IoControlCode - the driver-defined or system-defined I/O control code
                    (IOCTL) that is associated with the request.
Return Value:

    VOID

--*/

{
    NTSTATUS            status = STATUS_SUCCESS;
    WDFDEVICE           device;
    PDEVICE_EXTENSION   devContext = NULL;

    //WDFMEMORY           memory;
    //size_t				NumBytesTransferred;
    //PUCHAR				switchState = NULL;
    //UCHAR				eb;
    int					id = 0;

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    device = WdfIoQueueGetDevice(Queue);
    devContext = GetDeviceContext(device);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTL, "vJoyEvtInternalDeviceControl: IoControlCode=%x\n", IoControlCode);

    //
    // Please note that HIDCLASS provides the buffer in the Irp->UserBuffer
    // field irrespective of the ioctl buffer type. However, framework is very
    // strict about type checking. You cannot get Irp->UserBuffer by using
    // WdfRequestRetrieveOutputMemory if the ioctl is not a METHOD_NEITHER
    // internal ioctl. So depending on the ioctl code, we will either
    // use retreive function or escape to WDM to get the UserBuffer.
    //

    switch (IoControlCode) {

        case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
            //
            // Retrieves the device's HID descriptor.
            //
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "vJoyEvtInternalDeviceControl[IOCTL_HID_GET_DEVICE_DESCRIPTOR]:\n");
            status = vJoyGetHidDescriptor(device, Request);
            break;
        case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
            //
            //Retrieves a device's attributes in a HID_DEVICE_ATTRIBUTES structure.
            //
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "vJoyEvtInternalDeviceControl[IOCTL_HID_GET_DEVICE_ATTRIBUTES]:\n");
            status = vJoyGetDeviceAttributes(Request);
            break;

        case IOCTL_HID_GET_REPORT_DESCRIPTOR:
            //
            //Obtains the report descriptor for the HID device.
            //
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "vJoyEvtInternalDeviceControl[IOCTL_HID_GET_REPORT_DESCRIPTOR]:\n");
            status = vJoyGetReportDescriptor(device, Request);
            break;


        case IOCTL_HID_READ_REPORT:

            //
            // Returns a report from the device into a class driver-supplied buffer.
            // For now queue the request to the manual queue. The request will
            // be retrived and completd when continuous reader reads new data
            // from the device.
            //
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "vJoyEvtInternalDeviceControl[IOCTL_HID_READ_REPORT]:\n");
            status = WdfRequestForwardToIoQueue(Request, devContext->ReadReportMsgQueue);

            if (!NT_SUCCESS(status)) {
                TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
                    "WdfRequestForwardToIoQueue failed with status: 0x%x\n", status);

                WdfRequestComplete(Request, status);
            }

            return;

            //
            // This feature is only supported on WinXp and later. Compiling in W2K 
            // build environment will fail without this conditional preprocessor statement.
            //
#if (OSVER(NTDDI_VERSION) > NTDDI_WIN2K)

        case IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST:

            //
            // Hidclass sends this IOCTL for devices that have opted-in for Selective
            // Suspend feature. This feature is enabled by adding a registry value
            // "SelectiveSuspendEnabled" = 1 in the hardware key through inf file 
            // (see hidusbfx2.inf). Since hidclass is the power policy owner for 
            // this stack, it controls when to send idle notification and when to 
            // cancel it. This IOCTL is passed to USB stack. USB stack pends it. 
            // USB stack completes the request when it determines that the device is
            // idle. Hidclass's idle notification callback get called that requests a 
            // wait-wake Irp and subsequently powers down the device. 
            // The device is powered-up either when a handle is opened for the PDOs 
            // exposed by hidclass, or when usb stack completes wait
            // wake request. In the first case, hidclass cancels the notification 
            // request (pended with usb stack), cancels wait-wake Irp and powers up
            // the device. In the second case, an external wake event triggers completion
            // of wait-wake irp and powering up of device.
            //
            TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "vJoyEvtInternalDeviceControl[IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST]:\n");
            status = STATUS_NOT_SUPPORTED;
            break;

#endif // (OSVER(NTDDI_VERSION) > NTDDI_WIN2K)


        case IOCTL_HID_GET_FEATURE:
            //
            // returns a feature report associated with a top-level collection
            //
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "vJoyEvtInternalDeviceControl[IOCTL_HID_GET_FEATURE]:\n");
            status = vJoyGetFeature(Request);
            WdfRequestComplete(Request, status);
            return;

        case IOCTL_HID_SET_FEATURE:
            //
            // This sends a HID class feature report to a top-level collection of
            // a HID class device.
            //
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "vJoyEvtInternalDeviceControl[IOCTL_HID_SET_FEATURE]:\n");

        case IOCTL_HID_WRITE_REPORT:

            // Extracting the ID from the request
            id = FfbGetDevIDfromFfbRequest(Request);
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "vJoyEvtInternalDeviceControl[IOCTL_HID_WRITE_REPORT]: id=%d\n", id);

            // If FFB is not active then just reject this request
            if (!devContext->FfbEnable[id-1]) {
                TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "vJoyEvtInternalDeviceControl[IOCTL_HID_WRITE_REPORT]: ffb disabled for id=%d !\n", id);
                WdfRequestComplete(Request, STATUS_SUCCESS);
                return;
            };

            // If FFB is active then forward this request to the WriteQ and return
            status = WdfRequestForwardToIoQueue(Request, devContext->FfbWriteQ[id-1]);
            if (!NT_SUCCESS(status)) {
                TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
                    "WdfRequestForwardToIoQueue (FfbWriteQ[%d]) failed with status: 0x%x\n", id - 1, status);
                WdfRequestComplete(Request, status);
            }
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "vJoyEvtInternalDeviceControl[IOCTL_HID_WRITE_REPORT]: forwarded to queue and leaving with stt=0x%x !\n", status);
            return;


        case IOCTL_HID_GET_INPUT_REPORT:
            //
            // returns a HID class input report associated with a top-level
            // collection of a HID class device.
            //
            TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "vJoyEvtInternalDeviceControl[IOCTL_HID_GET_INPUT_REPORT]:\n");
            status = STATUS_NOT_SUPPORTED;
            break;

        case IOCTL_HID_SET_OUTPUT_REPORT:
            //
            // sends a HID class output report to a top-level collection of a HID
            // class device.
            //
            TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "vJoyEvtInternalDeviceControl[IOCTL_HID_SET_OUTPUT_REPORT]:\n");
            status = STATUS_NOT_SUPPORTED;
            break;

        case IOCTL_HID_GET_STRING:
            //
            // Requests that the HID minidriver retrieve a human-readable string
            // for either the manufacturer ID, the product ID, or the serial number
            // from the string descriptor of the device. The minidriver must send
            // a Get String Descriptor request to the device, in order to retrieve
            // the string descriptor, then it must extract the string at the
            // appropriate index from the string descriptor and return it in the
            // output buffer indicated by the IRP. Before sending the Get String
            // Descriptor request, the minidriver must retrieve the appropriate
            // index for the manufacturer ID, the product ID or the serial number
            // from the device extension of a top level collection associated with
            // the device.
            //
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "vJoyEvtInternalDeviceControl[IOCTL_HID_GET_STRING]:\n");
            status = vJoyGetHidString(Request);
            break;

        case IOCTL_HID_ACTIVATE_DEVICE:
            //
            // Makes the device ready for I/O operations.
            //
            TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "vJoyEvtInternalDeviceControl[IOCTL_HID_ACTIVATE_DEVICE]:\n");
            status = STATUS_NOT_SUPPORTED;
            break;
        case IOCTL_HID_DEACTIVATE_DEVICE:
            //
            // Causes the device to cease operations and terminate all outstanding
            // I/O requests.
            //
            TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "vJoyEvtInternalDeviceControl[IOCTL_HID_DEACTIVATE_DEVICE]:\n");
            status = STATUS_NOT_SUPPORTED;
            break;

        default:
            TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "vJoyEvtInternalDeviceControl[default]: ioctl not supported!\n");
            status = STATUS_NOT_SUPPORTED;
            break;
    }

    WdfRequestComplete(Request, status);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTL, "vJoyEvtInternalDeviceControl: exiting with stt=0x%x\n", status);

    return;
}


// Handle IOCTL_HID_GET_FEATURE
/*
 This function acts as a filter for FFB packets that needs processing, like:
 -
 */
NTSTATUS
vJoyGetFeature(
    IN WDFREQUEST Request
)
{
    NTSTATUS				status = STATUS_SUCCESS;
    WDF_REQUEST_PARAMETERS	params;
    PHID_XFER_PACKET		transferPacket = NULL;
    //UCHAR					ucBuffer[20] = {0};
    PUCHAR					ucTmp;
    WDFQUEUE				ParentQueue = NULL;
    WDFDEVICE				device;
    PDEVICE_EXTENSION		devContext = NULL;
    int						id = 0;


    // Get request parameters
    WDF_REQUEST_PARAMETERS_INIT(&params);
    WdfRequestGetParameters(Request, &params);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTL, "vJoyGetFeature: entering with controlcode=%x\n", params.Parameters.DeviceIoControl.IoControlCode);

    if (params.Parameters.DeviceIoControl.OutputBufferLength < sizeof(HID_XFER_PACKET)) {
        status = STATUS_BUFFER_TOO_SMALL;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "vJoyGetFeature: error Outputbuffer too small (%d should be >= %d)\n", params.Parameters.DeviceIoControl.OutputBufferLength, sizeof(HID_XFER_PACKET));
        return status;
    };

    // Get transfer packet
    transferPacket = (PHID_XFER_PACKET)WdfRequestWdmGetIrp(Request)->UserBuffer;
    if (transferPacket == NULL) {
        status = STATUS_INVALID_DEVICE_REQUEST;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "vJoyGetFeature: error transfer packet is null\n");
        return status;
    }

    // Get the context area
    ParentQueue = WdfRequestGetIoQueue(Request);
    device = WdfIoQueueGetDevice(ParentQueue);
    devContext = GetDeviceContext(device);

    // Check device exists
    id = FfbGetDevIDfromFfbRequest(Request);
    if (!id) {
        status = STATUS_NO_SUCH_DEVICE;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "vJoyGetFeature: error invalid device id (%d)\n", id);
        return status;
    }

    // Retrieve reportID
    BYTE reportID = transferPacket->reportId&0x0F;
    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_FFB, "vJoyGetFeature: id=%d, xfer=%x reportId=%x\n", id, transferPacket->reportId, reportID);

    // Flag to know whether report has been processed or not
    BOOLEAN knownReport = FALSE;

    ////////////////////////////////////////
    // Block Load Report ID 2
    // Byte[1]: Effect Block Index (1-100) max is VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX
    // Byte[2]: Block Load Success (1-3)
    // Byte[3]: Block Load Full (1-3)
    // Byte[4]: Block Load Error (1-3)
    ////////////////////////////////////////
    if (reportID == (HID_ID_BLKLDREP)) {
        ucTmp = (PUCHAR)transferPacket->reportBuffer;
        ucTmp[0] = transferPacket->reportId;
        // Lastly created Effect Block Index start at 1. 0 means not yet created
        ucTmp[1] = (UCHAR)((devContext->FfbPIDData[id-1].PIDBlockLoad.EffectBlockIndex)&0xFF);
        if (devContext->FfbEnable[id-1]) {
            ucTmp[2] = (UCHAR)((devContext->FfbPIDData[id-1].PIDBlockLoad.LoadStatus)&0xFF);
            ucTmp[3] = (UCHAR)((devContext->FfbPIDData[id-1].PIDBlockLoad.RAMPoolAvailable)&0xFF);
            ucTmp[4] = (UCHAR)((devContext->FfbPIDData[id-1].PIDBlockLoad.RAMPoolAvailable>>8)&0xFF);; // Load Error =0
            knownReport = TRUE;
        } else {
            ucTmp[2] = 0; // Load Success = 0
            ucTmp[3] = 0; // Load Full = 0
            ucTmp[4] = 1; // Load Error =1
        };
        TraceEvents(TRACE_LEVEL_VERBOSE, DBG_FFB, "vJoyGetFeature: Block Load ucTmp[1]=%x ucTmp[2]=%x ucTmp[3]=%x ucTmp[4]=%x\n",
            transferPacket->reportBuffer[1], transferPacket->reportBuffer[2], transferPacket->reportBuffer[3], transferPacket->reportBuffer[4]);
    }

    ////////////////////////////////////////
    // Pool Report ID 3
    // Byte[1]: RAM Pool size
    // Byte[2]: RAM Pool size
    // Byte[3]: Simultaneous Effects Max
    // Byte[4]: Device Managed Pool (0-1) + Shared Parameter Blocks (0-1)
    ////////////////////////////////////////
    if (reportID == (HID_ID_POOLREP)) {
        ucTmp = (PUCHAR)transferPacket->reportBuffer;
        ucTmp[0] = transferPacket->reportId;
        if (devContext->FfbEnable[id-1]) {
            ucTmp[1] = (UCHAR)((devContext->FfbPIDData[id-1].PIDPool.RAMPoolSize)&0xFF);
            ucTmp[2] = (UCHAR)((devContext->FfbPIDData[id-1].PIDPool.RAMPoolSize>>8)&0xFF);
            ucTmp[3] = (UCHAR)((devContext->FfbPIDData[id-1].PIDPool.MaxSimultaneousEffects)&0xFF);
            ucTmp[4] = (UCHAR)((devContext->FfbPIDData[id-1].PIDPool.MemoryManagement)&0xFF);
            knownReport = TRUE;
        } else {
            ucTmp[1] = (UCHAR)(0);
            ucTmp[2] = (UCHAR)(0);
            ucTmp[3] = (UCHAR)(0);
            ucTmp[4] = (UCHAR)(0);
        }
        TraceEvents(TRACE_LEVEL_VERBOSE, DBG_FFB, "vJoyGetFeature: Pool ucTmp[1]=%x ucTmp[2]=%x ucTmp[3]=%x ucTmp[4]=%x\n",
            transferPacket->reportBuffer[1], transferPacket->reportBuffer[2], transferPacket->reportBuffer[3], transferPacket->reportBuffer[4]);
    }
    // Report ID 3?
    if ((transferPacket->reportId&0x0F) == 3 && !devContext->FfbEnable[id-1])
        status = STATUS_NO_SUCH_DEVICE;

    ////////////////////////////////////////
    // State Report ID 4
    // Byte[1]: Effect Block Index (1-100) max is VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX
    // Byte[2]: State Report (bitfield)
    ////////////////////////////////////////
    //NOT WORKING FOR NOW, AS WE ARE UNABLE TO SEND A HID REPORT BACK TO
    //DIRECTINPUT
    if (reportID == (HID_ID_STATEREP)) {
        ucTmp = (PUCHAR)transferPacket->reportBuffer;
        ucTmp[0] = transferPacket->reportId;
        if (devContext->FfbEnable[id-1]) {
            ucTmp[1] = (UCHAR)(0);
            ucTmp[2] = (UCHAR)(0);
            knownReport = TRUE;
        } else {
            ucTmp[1] = (UCHAR)(0);
            ucTmp[2] = (UCHAR)(0);
        }
        TraceEvents(TRACE_LEVEL_VERBOSE, DBG_FFB, "vJoyGetFeature: State report ucTmp[1]=%x ucTmp[2]=%x\n",
            transferPacket->reportBuffer[1], transferPacket->reportBuffer[2]);
    }


    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTL, "vJoyGetFeature: exiting with stt=%d\n", status);


    return status;
}


#if 1
NTSTATUS
vJoySetFeature(
    IN WDFREQUEST Request
)
/*++

Routine Description

    This routine sets the state of the Feature: in this
    case Segment Display on the USB FX2 board.

Arguments:

    Request - Wdf Request

Return Value:

    NT status value

--*/
{
    NTSTATUS                     status = STATUS_SUCCESS;
    //UCHAR                        commandData = 0;
    PHID_XFER_PACKET             transferPacket = NULL;
    WDF_REQUEST_PARAMETERS       params;
    //WDFDEVICE                    device;
    //UCHAR                        featureUsage = 0;
    PUCHAR						 TmpBfr;

    PAGED_CODE();

    WDF_REQUEST_PARAMETERS_INIT(&params);
    WdfRequestGetParameters(Request, &params);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTL, "vJoySetFeature: entering with ioctl=0x%x\n", params.Parameters.DeviceIoControl.IoControlCode);

    //
    // IOCTL_HID_SET_FEATURE & IOCTL_HID_GET_FEATURE are not METHOD_NIEHTER
    // IOCTLs. So you cannot retreive UserBuffer from the IRP using Wdf
    // function. As a result we have to escape out to WDM to get the UserBuffer
    // directly from the IRP. 
    //
    if (params.Parameters.DeviceIoControl.InputBufferLength < sizeof(HID_XFER_PACKET)) {
        status = STATUS_BUFFER_TOO_SMALL;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "Userbuffer is small 0x%x\n", status);
        return status;
    }

    //
    // This is a kernel buffer so no need for try/except block when accesssing
    // Irp->UserBuffer.
    //
    transferPacket = (PHID_XFER_PACKET)WdfRequestWdmGetIrp(Request)->UserBuffer;
    if (transferPacket == NULL) {
        status = STATUS_INVALID_DEVICE_REQUEST;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "Irp->UserBuffer is NULL 0x%x\n", status);
        return status;
    }

    if (transferPacket->reportBufferLen == 0) {
        status = STATUS_BUFFER_TOO_SMALL;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "HID_XFER_PACKET->reportBufferLen is 0, 0x%x\n", status);
        return status;
    }

    if (transferPacket->reportBufferLen < sizeof(UCHAR)) {
        status = STATUS_BUFFER_TOO_SMALL;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "HID_XFER_PACKET->reportBufferLen is too small, 0x%x\n", status);
        return status;
    }

    // Get the REPORT_ID of this feature report

    //if (transferPacket->reportId != FEATURE_COLLECTION_REPORT_ID){
    //    status = STATUS_INVALID_DEVICE_REQUEST;
    //    TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
    //        "Incorrect report ID, 0x%x\n", status);
    //    return status;
    //}

    TmpBfr = (UCHAR*)transferPacket->reportBuffer;
    //featureUsage = featureReport->FeatureData;

    //if ((featureUsage >= sizeof(UsageToCommandDataMappingTable)) ||
    //    (featureUsage == 0)) {
    //    status = STATUS_INVALID_DEVICE_REQUEST;
    //    TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
    //        "Incorrect Usage provided: %d\n", featureUsage);
    //    return status;
    //}

    ////
    //// The Usage IDs 1 through 8 map to number displays on 7-segment display. 
    //// and Usage IDs 9 thorugh 18 map to LED display on Bar Graph.
    ////
    //commandData = UsageToCommandDataMappingTable[featureUsage];
    //if (featureUsage <= 8) {
    //    status = SendVendorCommand(
    //        device, 
    //        HIDFX2_SET_7SEGMENT_DISPLAY,
    //        &commandData 
    //        );
    //} 
    //else if(featureUsage <= 18){
    //    status = SendVendorCommand(
    //        device, 
    //        HIDFX2_SET_BARGRAPH_DISPLAY,
    //        &commandData
    //        );
    //}

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "vJoySetFeature Exit\n");
    return status;
}
#endif


NTSTATUS
vJoyGetHidDescriptor(
    IN WDFDEVICE Device,
    IN WDFREQUEST Request
)
/*++

Routine Description:

    Finds the HID descriptor and copies it into the buffer provided by the
    Request.

Arguments:

    Device - Handle to WDF Device Object

    Request - Handle to request object

Return Value:

    NT status code.

--*/
{
    NTSTATUS            status = STATUS_SUCCESS;
    size_t              bytesToCopy = 0;
    WDFMEMORY           memory;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTL, "vJoyGetHidDescriptor: entering\n");

    //
    // This IOCTL is METHOD_NEITHER so WdfRequestRetrieveOutputMemory
    // will correctly retrieve buffer from Irp->UserBuffer. 
    // Remember that HIDCLASS provides the buffer in the Irp->UserBuffer
    // field irrespective of the ioctl buffer type. However, framework is very
    // strict about type checking. You cannot get Irp->UserBuffer by using
    // WdfRequestRetrieveOutputMemory if the ioctl is not a METHOD_NEITHER
    // internal ioctl.
    //
    status = WdfRequestRetrieveOutputMemory(Request, &memory);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "WdfRequestRetrieveOutputMemory failed 0x%x\n", status);
        LogEventWithStatus(ERRLOG_HID_DESC_FAILED, L"WdfRequestRetrieveOutputMemory", WdfDeviceWdmGetDeviceObject(Device), status);
        return status;
    }

    UpdateCollections(Device);
    bytesToCopy = G_DefaultHidDescriptor.bLength;

    if (bytesToCopy == 0) {
        status = STATUS_INVALID_DEVICE_STATE;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "G_DefaultHidDescriptor is zero, 0x%x\n", status);
        LogEvent(ERRLOG_HID_DESC_FAILED1, NULL, WdfDeviceWdmGetDeviceObject(Device));
        return status;
    }

    status = WdfMemoryCopyFromBuffer(memory, 0, (PVOID)&G_DefaultHidDescriptor, bytesToCopy);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "WdfMemoryCopyFromBuffer failed 0x%x\n", status);
        LogEventWithStatus(ERRLOG_HID_DESC_FAILED, L"WdfMemoryCopyFromBuffer", WdfDeviceWdmGetDeviceObject(Device), status);
        return status;
    }

    //
    // Report how many bytes were copied
    //
    WdfRequestSetInformation(Request, bytesToCopy);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTL, "vJoyGetHidDescriptor: exiting with stt=0x%x\n", status);
    return status;
}

NTSTATUS
vJoyGetHidString(
    IN WDFREQUEST Request
)
/*++

Routine Description:

    Respond to IOCTL_HID_GET_STRING.
    Finds the requested string and copies it into the buffer provided by the
    Request.
    The requested string may be one of:
    - HID_STRING_ID_IMANUFACTURER: Manufacturer ID
    - HID_STRING_ID_IPRODUCT: Product ID
    - HID_STRING_ID_ISERIALNUMBER: Serial number

Arguments:

    Request - Handle to request object

Return Value:

    NT status code.

--*/
{
    //PVOID				buffer;
    //size_t				bufSize;
    NTSTATUS			status = STATUS_SUCCESS;
    PWSTR				pwstrID;
    WDFMEMORY           memory;
    size_t				bytesToCopy = 0;
    size_t				NumBytesTransferred = 0;
    WDF_REQUEST_PARAMETERS Parameters;

    WDF_REQUEST_PARAMETERS_INIT(&Parameters);
    WdfRequestGetParameters(Request, &Parameters);
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTL, "vJoyGetHidString: entering with hid string type=0x%x\n", (ULONGLONG)Parameters.Parameters.DeviceIoControl.Type3InputBuffer & 0xffff);

    switch ((ULONGLONG)Parameters.Parameters.DeviceIoControl.Type3InputBuffer & 0xffff) {
        case HID_STRING_ID_IMANUFACTURER:
            pwstrID = VENDOR_STR_ID;
            break;

        case HID_STRING_ID_IPRODUCT:
            pwstrID = PRODUCT_STR_ID;
            break;

        case HID_STRING_ID_ISERIALNUMBER:
            pwstrID = SERIALNUMBER_STR;
            break;

        default:
            pwstrID = NULL;
            break;
    }

    status = WdfRequestRetrieveOutputMemory(Request, &memory);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "WdfRequestRetrieveOutputMemory failed 0x%x\n", status);
        LogEventWithStatus(ERRLOG_HID_STRING_FAILED, L"WdfRequestRetrieveOutputMemory", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        return status;
    }

    bytesToCopy = pwstrID ? wcslen(pwstrID)*sizeof(WCHAR) + sizeof(UNICODE_NULL) : 0;
    if (bytesToCopy == 0) {
        status = STATUS_INVALID_DEVICE_STATE;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "Unable to copy HID string\n");
        LogEvent(ERRLOG_HID_STRING_FAILED1, NULL, WdfDriverWdmGetDriverObject(WdfGetDriver()));
        return status;
    };

    status = WdfMemoryCopyFromBuffer(memory, 0, pwstrID, bytesToCopy);
    WdfMemoryGetBuffer(memory, &NumBytesTransferred);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "WdfMemoryCopyFromBuffer failed 0x%x\n", status);
        LogEventWithStatus(ERRLOG_HID_STRING_FAILED, L"WdfMemoryCopyFromBuffer", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        return status;
    }

    WdfRequestSetInformation(Request, NumBytesTransferred);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTL, "vJoyGetHidString: exiting with stt=0x%x\n", status);
    return status;

}


NTSTATUS
vJoyGetReportDescriptor(
    IN WDFDEVICE Device,
    IN WDFREQUEST Request
)
/*++

Routine Description:

    Finds the Report descriptor and copies it into the buffer provided by the
    Request.

Arguments:

    Device - Handle to WDF Device Object

    Request - Handle to request object

Return Value:

    NT status code.

--*/
{
    NTSTATUS            status = STATUS_SUCCESS;
    size_t				bytesToCopy = 0;
    WDFMEMORY           memory;
    size_t				NumBytesTransferred = 0;
    PUCHAR				switchState = NULL;
    PDEVICE_EXTENSION   pDeviceContext = NULL;


    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTL, "vJoyGetReportDescriptor: entering\n");

    //
    // This IOCTL is METHOD_NEITHER so WdfRequestRetrieveOutputMemory
    // will correctly retrieve buffer from Irp->UserBuffer. 
    // Remember that HIDCLASS provides the buffer in the Irp->UserBuffer
    // field irrespective of the ioctl buffer type. However, framework is very
    // strict about type checking. You cannot get Irp->UserBuffer by using
    // WdfRequestRetrieveOutputMemory if the ioctl is not a METHOD_NEITHER
    // internal ioctl.
    //
    status = WdfRequestRetrieveOutputMemory(Request, &memory);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "WdfRequestRetrieveOutputMemory failed 0x%x\n", status);
        LogEventWithStatus(ERRLOG_REPORT_DESC_FAILED, L"WdfRequestRetrieveOutputMemory", WdfDeviceWdmGetDeviceObject(Device), status);
        return status;
    }

    // Get report descriptor
    UpdateCollections(Device);

    bytesToCopy = G_DefaultHidDescriptor.DescriptorList[0].wReportLength;
    if (bytesToCopy == 0) {
        status = STATUS_INVALID_DEVICE_STATE;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "G_DefaultHidDescriptor's reportLenght is zero, 0x%x\n", status);
        LogEvent(ERRLOG_REPORT_DESC_FAILED1, NULL, WdfDeviceWdmGetDeviceObject(Device));
        return status;
    };

    pDeviceContext = GetDeviceContext(Device);
    if (!pDeviceContext || !pDeviceContext->ReportDescriptor) {
        status = STATUS_INVALID_DEVICE_STATE;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "ReportDescriptor == NULL 0x%x\n", status);
        LogEvent(ERRLOG_REPORT_DESC_FAILED1, NULL, WdfDeviceWdmGetDeviceObject(Device));
        return status;
    };

    status = WdfMemoryCopyFromBuffer(memory, 0, pDeviceContext->ReportDescriptor, bytesToCopy);
    switchState = WdfMemoryGetBuffer(memory, &NumBytesTransferred);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "WdfMemoryCopyFromBuffer failed 0x%x\n", status);
        LogEventWithStatus(ERRLOG_REPORT_DESC_FAILED, L"WdfMemoryCopyFromBuffer", WdfDeviceWdmGetDeviceObject(Device), status);
        return status;
    }

    //
    // Report how many bytes were copied
    //
    WdfRequestSetInformation(Request, bytesToCopy);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTL, "vJoyGetReportDescriptor: exiting with stt=0x%x\n", status);
    return status;
}


NTSTATUS
vJoyGetDeviceAttributes(
    IN WDFREQUEST Request
)
/*++

Routine Description:

    Fill in the given struct _HID_DEVICE_ATTRIBUTES

Arguments:

    Request - Pointer to Request object.

Return Value:

    NT status code.

--*/
{
    NTSTATUS                 status = STATUS_SUCCESS;
    PHID_DEVICE_ATTRIBUTES   deviceAttributes = NULL;
    PDEVICE_EXTENSION        pDeviceContext = NULL;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTL, "vJoyGetDeviceAttributes: entering\n");

    pDeviceContext = GetDeviceContext(WdfIoQueueGetDevice(WdfRequestGetIoQueue(Request)));

    //
    // This IOCTL is METHOD_NEITHER so WdfRequestRetrieveOutputMemory
    // will correctly retrieve buffer from Irp->UserBuffer. 
    // Remember that HIDCLASS provides the buffer in the Irp->UserBuffer
    // field irrespective of the ioctl buffer type. However, framework is very
    // strict about type checking. You cannot get Irp->UserBuffer by using
    // WdfRequestRetrieveOutputMemory if the ioctl is not a METHOD_NEITHER
    // internal ioctl.
    //
    status = WdfRequestRetrieveOutputBuffer(Request, sizeof(HID_DEVICE_ATTRIBUTES), &deviceAttributes, NULL);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "vJoyGetDeviceAttributes: WdfRequestRetrieveOutputBuffer failed 0x%x\n", status);
        LogEventWithStatus(ERRLOG_DEVICE_ATTR_FAILED, L"WdfRequestRetrieveOutputBuffer", WdfDeviceWdmGetDeviceObject(WdfIoQueueGetDevice(WdfRequestGetIoQueue(Request))), status);
        return status;
    }

    deviceAttributes->VendorID = VENDOR_N_ID;
    deviceAttributes->ProductID = PRODUCT_N_ID;
    deviceAttributes->VersionNumber = VERSION_N;

    //
    // Report how many bytes were copied
    //
    WdfRequestSetInformation(Request, sizeof(HID_DEVICE_ATTRIBUTES));

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTL, "vJoyGetDeviceAttributes: exiting with stt=0x%x\n", status);
    return status;
}


PCHAR
DbgHidInternalIoctlString(
    IN ULONG        IoControlCode
)
/*++

Routine Description:

    Returns Ioctl string helpful for debugging

Arguments:

    IoControlCode - IO Control code

Return Value:

    Name String

--*/
{
    switch (IoControlCode) {
        case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
            return "IOCTL_HID_GET_DEVICE_DESCRIPTOR";
        case IOCTL_HID_GET_REPORT_DESCRIPTOR:
            return "IOCTL_HID_GET_REPORT_DESCRIPTOR";
        case IOCTL_HID_READ_REPORT:
            return "IOCTL_HID_READ_REPORT";
        case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
            return "IOCTL_HID_GET_DEVICE_ATTRIBUTES";
        case IOCTL_HID_WRITE_REPORT:
            return "IOCTL_HID_WRITE_REPORT";
        case IOCTL_HID_SET_FEATURE:
            return "IOCTL_HID_SET_FEATURE";
        case IOCTL_HID_GET_FEATURE:
            return "IOCTL_HID_GET_FEATURE";
        case IOCTL_HID_GET_STRING:
            return "IOCTL_HID_GET_STRING";
        case IOCTL_HID_ACTIVATE_DEVICE:
            return "IOCTL_HID_ACTIVATE_DEVICE";
        case IOCTL_HID_DEACTIVATE_DEVICE:
            return "IOCTL_HID_DEACTIVATE_DEVICE";
        case IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST:
            return "IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST";
        default:
            return "Unknown IOCTL";
    }
}





VOID
vJoyEvtIoDeviceControl(
    IN WDFQUEUE     Queue,
    IN WDFREQUEST   Request,
    IN size_t       OutputBufferLength,
    IN size_t       InputBufferLength,
    IN ULONG        IoControlCode
)
/*++
Routine Description:

    This event is called when the framework receives IRP_MJ_DEVICE_CONTROL
    requests from the system.

Arguments:

    Queue - Handle to the framework queue object that is associated
            with the I/O request.
    Request - Handle to a framework request object.

    OutputBufferLength - length of the request's output buffer,
                        if an output buffer is available.
    InputBufferLength - length of the request's input buffer,
                        if an input buffer is available.

    IoControlCode - the driver-defined or system-defined I/O control code
                    (IOCTL) that is associated with the request.

Return Value:

   VOID

--*/
{
    NTSTATUS             status = STATUS_SUCCESS;
    //WDF_DEVICE_STATE     deviceState;
    WDFDEVICE            hDevice = WdfIoQueueGetDevice(Queue);
    PCONTROL_DEVICE_EXTENSION			 ControlDevContext = ControlGetData(hDevice);
    //ULONG  bytes;
    PDEVICE_EXTENSION    pDevContext = NULL;
    //WDFMEMORY  reqMemory;
    PVOID  buffer;
    size_t  bufSize;
    JOYSTICK_POSITION* iReport;
    //JOYSTICK_POSITION_V2   icReport;
    //PHID_DEVICE_ATTRIBUTES   deviceAttributes = NULL;
    size_t	bytesReturned = 0;




    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    KdPrint(("vJoyEvtIoDeviceControl called\n"));
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyEvtIoDeviceControl: IoControlCode=%x\n", IoControlCode);

    PAGED_CODE();

    switch (IoControlCode) {

        case 0x910: // Backward compatibility value of 	LOAD_POSITIONS
        case LOAD_POSITIONS:
            status = WdfRequestRetrieveInputBuffer(Request, sizeof(PJOYSTICK_POSITION), &buffer, &bufSize);
            if (!NT_SUCCESS(status)) break;

            iReport = buffer;
            pDevContext = GetDeviceContext(ControlDevContext->hParentDevice);

            LoadPositions(iReport, pDevContext, bufSize);
            break;


        default:
            status = STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // Complete the Request.
    //
    WdfRequestCompleteWithInformation(Request, status, bytesReturned);

}

NTSTATUS
vJoyCreateControlDevice(
    WDFDEVICE Device
)
/*++

Routine Description:

    This routine is called to create a control device object so that application
    can talk to the filter driver directly instead of going through the entire
    device stack. This kind of control device object is useful if the filter
    driver is underneath another driver which prevents ioctls not known to it
    or if the driver's dispatch routine is owned by some other (port/class)
    driver and it doesn't allow any custom ioctls.

    NOTE: Since the control device is global to the driver and accessible to
    all instances of the device this filter is attached to, we create only once
    when the first instance of the device is started and delete it when the
    last instance gets removed.

Arguments:

    Device - Handle to a filter device object.

Return Value:

    WDF status code

--*/
{
    PCONTROL_DEVICE_EXTENSION	ConDevContext = NULL;
    PWDFDEVICE_INIT             pInit = NULL;
    WDF_OBJECT_ATTRIBUTES       controlAttributes;
    WDF_IO_QUEUE_CONFIG         ioQueueConfig;
    BOOLEAN                     bCreate = FALSE;
    NTSTATUS                    status;
    WDFQUEUE                    queue;
    UNICODE_STRING				ntDeviceName, symbolicLinkName;
    ANSI_STRING					ntDeviceNameA, symbolicLinkNameA;

    //DECLARE_CONST_UNICODE_STRING(ntDeviceName, NTDEVICE_NAME_STRING) ;
    //DECLARE_CONST_UNICODE_STRING(symbolicLinkName, SYMBOLIC_NAME_STRING) ;
    DECLARE_CONST_UNICODE_STRING(__SDDL_DEVOBJ_SYS_ALL_ADM_RWX_WORLD_RW_RES_R, L"D:P(A;;GA;;;SY)(A;;GRGWGX;;;BA)(A;;GRGW;;;WD)(A;;GR;;;RC)");

    PAGED_CODE();
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyCreateControlDevice: entering\n");

    //
    // First find out whether any Control Device has been created. If the
    // collection has more than one device then we know somebody has already
    // created or in the process of creating the device.
    //
    WdfWaitLockAcquire(vJoyDeviceCollectionLock, NULL);

    if (WdfCollectionGetCount(vJoyDeviceCollection) == 1) {
        bCreate = TRUE;
    }

    WdfWaitLockRelease(vJoyDeviceCollectionLock);

    if (!bCreate) {
        TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyCreateControlDevice: Device already created, exiting with success\n");
        //
        // Control device is already created. So return success.
        //
        return STATUS_SUCCESS;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyCreateControlDevice: Creating Control Device\n");

    //
    //
    // In order to create a control device, we first need to allocate a
    // WDFDEVICE_INIT structure and set all properties.
    //


    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyCreateControlDevice: Calling WdfControlDeviceInitAllocate\n");
    pInit = WdfControlDeviceInitAllocate(WdfDeviceGetDriver(Device), &__SDDL_DEVOBJ_SYS_ALL_ADM_RWX_WORLD_RW_RES_R);

    if (pInit == NULL) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "vJoyCreateControlDevice: WdfControlDeviceInitAllocate Failed for insufficient resources\n");
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Error;
    }

    //
    // Set exclusive to false so that more than one app can talk to the
    // control device simultaneously.
    //
    WdfDeviceInitSetExclusive(pInit, FALSE);

    //
    // Assign a name to the Control Device
    // It has to be a UNICODE name hence the conversions
    //
    RtlInitAnsiString(&ntDeviceNameA, TEXT(NTDEVICE_NAME_STRING));
    status = RtlAnsiStringToUnicodeString(&ntDeviceName, &ntDeviceNameA, TRUE);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "vJoyCreateControlDevice: RtlAnsiStringToUnicodeString for device name Failed\n");
        goto Error;
    }
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyCreateControlDevice: Calling WdfDeviceInitAssignName\n");
    status = WdfDeviceInitAssignName(pInit, &ntDeviceName);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "vJoyCreateControlDevice: WdfDeviceInitAssignName Failed\n");
        goto Error;
    }
    RtlFreeUnicodeString(&ntDeviceName);


    //
    // Specify the size of device context & create the Control Device
    //
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyCreateControlDevice: Creating Control Device\n");
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&controlAttributes, CONTROL_DEVICE_EXTENSION);
    status = WdfDeviceCreate(&pInit, &controlAttributes, &g_ControlDevice);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "vJoyCreateControlDevice: WdfDeviceCreate Failed\n");
        goto Error;
    }

    //
    // Create a symbolic link for the control object so that usermode can open
    // the device.
    //
    // It has to be a UNICODE name hence the conversions
    //
    RtlInitAnsiString(&symbolicLinkNameA, TEXT(SYMBOLIC_NAME_STRING));
    status = RtlAnsiStringToUnicodeString(&symbolicLinkName, &symbolicLinkNameA, TRUE);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "vJoyCreateControlDevice: RtlAnsiStringToUnicodeString for symbolic name Failed\n");
        goto Error;
    }
    status = WdfDeviceCreateSymbolicLink(g_ControlDevice, &symbolicLinkName);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "vJoyCreateControlDevice: Failed to create symbolic link (Native)\n");
        goto Error;
    }
    RtlFreeUnicodeString(&symbolicLinkName);

    //
    // Configure the default queue associated with the control device object
    // to be Serial so that request passed to EvtIoDeviceControl are serialized.
    //

    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig, WdfIoQueueDispatchSequential);

    ioQueueConfig.EvtIoDeviceControl = vJoyEvtIoDeviceControl;


    //
    // Framework by default creates non-power managed queues for
    // filter drivers.
    //
    status = WdfIoQueueCreate(g_ControlDevice, &ioQueueConfig, WDF_NO_OBJECT_ATTRIBUTES, &queue);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "vJoyCreateControlDevice: WdfIoQueueCreate Failed\n");
        goto Error;
    }


    ConDevContext = ControlGetData(g_ControlDevice);
    ConDevContext->hParentDevice = Device;


    //
    // Control devices must notify WDF when they are done initializing.   I/O is
    // rejected until this call is made.
    //
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyCreateControlDevice: Calling WdfControlFinishInitializing\n");
    WdfControlFinishInitializing(g_ControlDevice);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyCreateControlDevice: exiting cleanly\n");
    return STATUS_SUCCESS;

Error:

    if (pInit != NULL) {
        WdfDeviceInitFree(pInit);
    }

    if (g_ControlDevice != NULL) {
        //
        // Release the reference on the newly created object, since
        // we couldn't initialize it.
        //
        WdfObjectDelete(g_ControlDevice);
        g_ControlDevice = NULL;
    }
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyCreateControlDevice: exiting with error 0x%x\n", status);

    return status;
}



VOID
vJoyDeleteControlDevice(
    WDFDEVICE Device
)
/*++

Routine Description:

    This routine deletes the control by doing a simple dereference.

Arguments:

    Device - Handle to a framework filter device object.

Return Value:

    WDF status code

--*/
{
    UNREFERENCED_PARAMETER(Device);

    PAGED_CODE();

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyDeleteControlDevice: entering Control Device Start deleting\n");
    if (!g_ControlDevice) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "vJoyDeleteControlDevice: error No Control Device to delete\n");
        return;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyDeleteControlDevice: Control Device: Purging queue\n");
    WdfIoQueuePurge(WdfDeviceGetDefaultQueue(g_ControlDevice), WDF_NO_EVENT_CALLBACK, WDF_NO_CONTEXT);


    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyDeleteControlDevice: Control Device: Deleting\n");

    if (g_ControlDevice) {
        TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyDeleteControlDevice: Control Device: Deleting (Just before WdfObjectDelete)\n");
        WdfObjectDelete(g_ControlDevice);
        //WdfObjectDelete(Device);
        TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyDeleteControlDevice: Control Device: Deleting (Just after WdfObjectDelete)\n");
        g_ControlDevice = NULL;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyDeleteControlDevice: exiting\n");
}


_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
VOID
vJoyEvtDeviceContextCleanup(
    _In_ WDFDEVICE Device
)
/*++

Routine Description:

   EvtDeviceRemove event callback must perform any operations that are
   necessary before the specified device is removed. The framework calls
   the driver's EvtDeviceRemove callback when the PnP manager sends
   an IRP_MN_REMOVE_DEVICE request to the driver stack.

Arguments:

    Device - Handle to a framework device object.

Return Value:

    WDF status code

--*/
{
    // 	_IRQL_requires_max_(DISPATCH_LEVEL);
    PAGED_CODE();


    int   nDevices = 0;
    PDEVICE_EXTENSION             devContext = NULL;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "FilterEvtDeviceContextCleanup: entering\n");

    nDevices = DeviceCount(TRUE, -1); // Decrementing device count
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "FilterEvtDeviceContextCleanup: Device Count before decrementing is %d\n", nDevices);

    WdfWaitLockAcquire(vJoyDeviceCollectionLock, NULL);

    nDevices = WdfCollectionGetCount(vJoyDeviceCollection);

    if (nDevices == 1) {
        //
        // We are the last instance. So let us delete the control-device
        // so that driver can unload when the FilterDevice is deleted.
        // We absolutely have to do the deletion of control device with
        // the collection lock acquired because we implicitly use this
        // lock to protect g_ControlDevice global variable. We need to make
        // sure another thread doesn't attempt to create while we are
        // deleting the device.
        //
        vJoyDeleteControlDevice(Device);
    }

    devContext = GetDeviceContext(Device);
    CleanUpDev(devContext);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "Device Count after decrementing is %d\n", DeviceCount(TRUE, 0));

    //WdfCollectionRemove(vJoyDeviceCollection, Device);
    WdfWaitLockRelease(vJoyDeviceCollectionLock);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "FilterEvtDeviceContextCleanup: exiting with Device Count after decrementing is %d\n", nDevices);
}

NTSTATUS vJoyEvtDeviceReleaseHardware(
    IN  WDFDEVICE    Device,
    IN  WDFCMRESLIST ResourcesTranslated
)
// Test Only
{
    PAGED_CODE();
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(ResourcesTranslated);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyEvtDeviceReleaseHardware: entering/exiting for PNP\n");
    return STATUS_SUCCESS;
}

NTSTATUS vJoyEvtDevicePrepareHardware(
    IN  WDFDEVICE    Device,
    IN  WDFCMRESLIST ResourcesRaw,
    IN  WDFCMRESLIST ResourcesTranslated
)
// Test Only
{
    PAGED_CODE();
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(ResourcesRaw);
    UNREFERENCED_PARAMETER(ResourcesTranslated);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyEvtDevicePrepareHardware: entering/exiting for PNP\n");
    return STATUS_SUCCESS;
}

VOID vJoyEvtDeviceSelfManagedIoFlush(
    IN  WDFDEVICE    Device
)
// Test Only
{
    PAGED_CODE();
    UNREFERENCED_PARAMETER(Device);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyEvtDeviceSelfManagedIoFlush: entering/exiting for PNP\n");
}

VOID vJoyEvtDevicePnpStateChange(
    IN  WDFDEVICE                          Device,
    IN  PCWDF_DEVICE_PNP_NOTIFICATION_DATA NotificationData
)
{
    PAGED_CODE();
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(NotificationData);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyEvtDevicePnpStateChange: entering/exiting for PNP\n");
}


VOID
LoadPositions(PJOYSTICK_POSITION pPosition, PDEVICE_EXTENSION pDevContext, size_t buffsize)
{
    UNREFERENCED_PARAMETER(buffsize);

    int i;

    i = pPosition->bDevice-1; // Index is zero-based

    WdfWaitLockAcquire(pDevContext->positionLock, NULL);

    // Clear 'dearty bit'
    // This means that the position data should not be read (It is not ready)
    pDevContext->PositionReady[i] = FALSE;

    // Copy position to context area
    pDevContext->positions[i]->ValX = pPosition->wAxisX;
    pDevContext->positions[i]->ValY = pPosition->wAxisY;
    pDevContext->positions[i]->ValZ = pPosition->wAxisZ;
    pDevContext->positions[i]->ValRX = pPosition->wAxisXRot;
    pDevContext->positions[i]->ValRY = pPosition->wAxisYRot;
    pDevContext->positions[i]->ValRZ = pPosition->wAxisZRot;
    pDevContext->positions[i]->ValSlider = pPosition->wSlider;
    pDevContext->positions[i]->ValDial = pPosition->wDial;

    pDevContext->positions[i]->ValWheel = pPosition->wWheel;
    pDevContext->positions[i]->ValAccelerator = pPosition->wAccelerator;
    pDevContext->positions[i]->ValBrake = pPosition->wBrake;
    pDevContext->positions[i]->ValClutch = pPosition->wClutch;
    pDevContext->positions[i]->ValSteering = pPosition->wSteering;
    pDevContext->positions[i]->ValThrottle = pPosition->wThrottle;
    pDevContext->positions[i]->ValRudder = pPosition->wRudder;
    pDevContext->positions[i]->ValAileron = pPosition->wAileron;

    pDevContext->positions[i]->ValButtons = pPosition->lButtons;
    pDevContext->positions[i]->ValButtonsEx1 = pPosition->lButtonsEx1;
    pDevContext->positions[i]->ValButtonsEx2 = pPosition->lButtonsEx2;
    pDevContext->positions[i]->ValButtonsEx3 = pPosition->lButtonsEx3;

    pDevContext->positions[i]->ValHats = pPosition->bHats;
    pDevContext->positions[i]->ValHatsEx1 = pPosition->bHatsEx1;
    pDevContext->positions[i]->ValHatsEx2 = pPosition->bHatsEx2;
    pDevContext->positions[i]->ValHatsEx3 = pPosition->bHatsEx3;

    // Set 'dearty bit'
    // This means that the position data is ready to be read
    pDevContext->PositionReady[i] = TRUE;

    WdfWaitLockRelease(pDevContext->positionLock);
}

VOID GetPositions(PJOYSTICK_POSITION pPosition, PDEVICE_EXTENSION pDevContext, UCHAR id, size_t buffsize)
{
    UNREFERENCED_PARAMETER(buffsize);

    int i;

    i = id - 1; // Index is zero-based

    WdfWaitLockAcquire(pDevContext->positionLock, NULL);
    pPosition->bDevice = id;

    pPosition->wAxisX = pDevContext->positions[i]->ValX;
    pPosition->wAxisY = pDevContext->positions[i]->ValY;
    pPosition->wAxisZ = pDevContext->positions[i]->ValZ;
    pPosition->wAxisXRot = pDevContext->positions[i]->ValRX;
    pPosition->wAxisYRot = pDevContext->positions[i]->ValRY;
    pPosition->wAxisZRot = pDevContext->positions[i]->ValRZ;
    pPosition->wSlider = pDevContext->positions[i]->ValSlider;
    pPosition->wDial = pDevContext->positions[i]->ValDial;

    pPosition->wWheel = pDevContext->positions[i]->ValWheel;
    pPosition->wAccelerator = pDevContext->positions[i]->ValAccelerator;
    pPosition->wBrake = pDevContext->positions[i]->ValBrake;
    pPosition->wClutch = pDevContext->positions[i]->ValClutch;
    pPosition->wSteering = pDevContext->positions[i]->ValSteering;
    pPosition->wThrottle = pDevContext->positions[i]->ValThrottle;
    pPosition->wRudder = pDevContext->positions[i]->ValRudder;
    pPosition->wAileron = pDevContext->positions[i]->ValAileron;

    pPosition->lButtons = pDevContext->positions[i]->ValButtons;
    pPosition->lButtonsEx1 = pDevContext->positions[i]->ValButtonsEx1;
    pPosition->lButtonsEx2 = pDevContext->positions[i]->ValButtonsEx2;
    pPosition->lButtonsEx3 = pDevContext->positions[i]->ValButtonsEx3;

    pPosition->bHats = pDevContext->positions[i]->ValHats;
    pPosition->bHatsEx1 = pDevContext->positions[i]->ValHatsEx1;
    pPosition->bHatsEx2 = pDevContext->positions[i]->ValHatsEx2;
    pPosition->bHatsEx3 = pDevContext->positions[i]->ValHatsEx3;

    WdfWaitLockRelease(pDevContext->positionLock);
}


/*++

Routine Description:

   GetReportDescriptor helper function returns a pointer to the HID Report Descriptor
   as requested in vJoyGetReportDescriptor()
   If a corresponding value exists in the registry and is valid it is returned
   Otherwize NULL is returned

   Parameters:
   Size [out]:		Caller suplied pointer to buffer that holds the size of the HID report descriptor
   IdMask [out]:	Caller suplied pointer to a WORD. Each bit represent a top-level report.
                    If bit n is set then Report ID n+1 is valid.
   FFbmask [out]:	Caller suplied pointer to a WORD. Each bit represent a ffb enable bit.
                    If bit n is set then FFB n+1 is enabled in registry.

--*/
PVOID GetReportDescriptorFromRegistry(USHORT* Size, USHORT* IdMask, USHORT* FFbmask)
{

    NTSTATUS				status = STATUS_SUCCESS;
    WDFKEY					KeyDevice, KeyParameters;
    UNICODE_STRING			strDev, strDescName, strDescSize;
    ULONG					ValueLengthQueried;
    USHORT					dDescSize = 0, dDescSizePrev = 0;
    PVOID					out = NULL;
    HANDLE					hKeyParameters = NULL;
    int						iDevice = 0;
    PKEY_BASIC_INFORMATION 	pDeviceBasicInfo = NULL;
    ULONG					ResultLength;
    ULONG					nameLength;
    WCHAR					DeviceKeyName[NameSize];
    size_t					pcb = 0;
    USHORT					reportId = 0;

    PAGED_CODE();

    *Size = 0;
    *IdMask = 0;
    *FFbmask = 0;

    // Get the key of the Parameters key under "SYSTEM\\CurrentControlSet\\services\\vjoy"
    status = WdfDriverOpenParametersRegistryKey(WdfGetDriver(), WRITE_DAC, WDF_NO_OBJECT_ATTRIBUTES, &KeyParameters);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "GetReportDescriptorFromRegistry: error opening registry key stt=0x%x\n", status);
        LogEventWithStatus(ERRLOG_REP_REG_FAILED, L"WdfDriverOpenParametersRegistryKey", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        return NULL;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "GetReportDescriptorFromRegistry: entering\n");

    // Enumerate Subkeys holding the configuration data
    status = STATUS_SUCCESS;
    while (status != STATUS_NO_MORE_ENTRIES) { // Loop on all sub-keys
        // Prepare buffer to receive data and get the name of the next sub-key
        // 1. Enumerate once with zero size just to get the required sizw
        // 2. Alocate the correct size to hold the output struct
        // 3. Enumerate again to get the data
        ResultLength = 0;
        hKeyParameters = WdfRegistryWdmGetHandle(KeyParameters);
        // 1.
        status = ZwEnumerateKey(hKeyParameters, iDevice, KeyBasicInformation, NULL, ResultLength, &ResultLength);
        if (status == STATUS_BUFFER_OVERFLOW || status == STATUS_BUFFER_TOO_SMALL) {
            // 2.
            pDeviceBasicInfo = (PKEY_BASIC_INFORMATION)ExAllocatePoolWithTag(NonPagedPool, ResultLength, 'fnIb');
            if (!pDeviceBasicInfo)
                return NULL;
            RtlZeroMemory(pDeviceBasicInfo, ResultLength);
            // 3.
            status = ZwEnumerateKey(hKeyParameters, iDevice++, KeyBasicInformation, pDeviceBasicInfo, ResultLength, &ResultLength);
        }
        // Check we are done
        if (status == STATUS_NO_MORE_ENTRIES) {
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "GetReportDescriptorFromRegistry: done enumarating\n");
            break;
        }
        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "GetReportDescriptorFromRegistry: error enumerating key, exiting\n");
            LogEventWithStatus(ERRLOG_REP_REG_FAILED, L"ZwEnumerateKey", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
            //ExFreePoolWithTag(pDeviceBasicInfo, 'fnIb');
            return NULL;
        }

        if (!pDeviceBasicInfo) {
            TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "GetReportDescriptorFromRegistry: error geting devicebasicinfo, exiting\n");
            return NULL;
        }

        // Copy name of subkey to unicode buffer and release temporary buffer
        nameLength = pDeviceBasicInfo->NameLength;
        RtlZeroMemory(DeviceKeyName, NameSize);
        status = RtlStringCbCopyNW(DeviceKeyName, NameSize *sizeof(WCHAR), pDeviceBasicInfo->Name, nameLength);
        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "GetReportDescriptorFromRegistry: descriptor key not matching, continue\n");
            LogEventWithStatus(ERRLOG_REP_REG_FAILED, L"RtlStringCbCopyNW", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
            ExFreePoolWithTag(pDeviceBasicInfo, 'fnIb');
            return NULL;
        }
        RtlInitUnicodeString(&strDev, DeviceKeyName);
        ExFreePoolWithTag(pDeviceBasicInfo, 'fnIb');
        
        TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "GetReportDescriptorFromRegistry: opening descriptor key:%Ws\n", DeviceKeyName);
        
        // The sub-key name should range from "Device01" to "Device16"
        status = RtlStringCbLengthW(REG_DEVICE, NameSize, &pcb);
        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "GetReportDescriptorFromRegistry: failed getting size of descriptor key, continue\n");
            continue;
        }
        if (!RtlEqualMemory(DeviceKeyName, REG_DEVICE, pcb)) {
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "GetReportDescriptorFromRegistry: descriptor key not matching, continue\n");
            continue;
        }

        // Get the Subkey holding the configuration data
        status = WdfRegistryOpenKey(KeyParameters, &strDev, GENERIC_READ, WDF_NO_OBJECT_ATTRIBUTES, &KeyDevice);
        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "GetReportDescriptorFromRegistry: failed opening descriptor key, continue\n");
            //WdfRegistryClose(KeyParameters);
            continue;
        }


        // Get the size of value 'HidReportDescriptor' - It should return status of STATUS_BUFFER_OVERFLOW and a positive value in ValueLengthQueried
        // Then compare this value to the one in value 'HidReportDescriptorSize'. 
        // If they are identical create a buffer and get the data in value 'HidReportDescriptor'
        RtlInitUnicodeString(&strDescName, DESC_NAME);
        status = WdfRegistryQueryValue(KeyDevice, &strDescName, 0, NULL, &ValueLengthQueried, NULL);
        if (status != STATUS_BUFFER_OVERFLOW) {
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "GetReportDescriptorFromRegistry: failed opening descriptorsize key, continue\n");
            //WdfRegistryClose(KeyParameters);
            WdfRegistryClose(KeyDevice);
            continue;
        }
        RtlInitUnicodeString(&strDescSize, DESC_SIZE);
        status = WdfRegistryQueryValue(KeyDevice, &strDescSize, 4, &dDescSize, NULL, NULL);
        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "GetReportDescriptorFromRegistry: failed getting descriptorsize value, continue\n");
            //WdfRegistryClose(KeyParameters);
            WdfRegistryClose(KeyDevice);
            continue;
        }

        if ((dDescSize != ValueLengthQueried) | !dDescSize) {
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "GetReportDescriptorFromRegistry: failed matching real descriptor size and descriptorsize value, continue\n");
            //WdfRegistryClose(KeyParameters);
            WdfRegistryClose(KeyDevice);
            continue;
        }

        // The size of the descriptor was verified - Allocate memory and read it
        out = ReAllocatePoolWithTag(out, dDescSizePrev, PagedPool, dDescSizePrev+dDescSize, MEM_TAG_HIDRPRT);
        if (!out) {
            WdfRegistryClose(KeyParameters);
            WdfRegistryClose(KeyDevice);
            TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "GetReportDescriptorFromRegistry: cannot allocate memory for descriptor, exiting\n");
            LogEvent(ERRLOG_REP_REG_FAILED1, NULL, WdfDriverWdmGetDriverObject(WdfGetDriver()));
            return NULL;
        }

        status = WdfRegistryQueryValue(KeyDevice, &strDescName, dDescSize, (BYTE*)out+dDescSizePrev, NULL, NULL);
        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "GetReportDescriptorFromRegistry: cannot copy descriptor, continue\n");
            WdfRegistryClose(KeyDevice);
            //ExFreePoolWithTag(out, MEM_TAG_HIDRPRT);
            continue;
        }



        //////////////////////////////////////////////////////////////////////////////
        // Get the Report ID from the buffer
        // Byte 10 should be 0x85
#if 0
        if ((*((BYTE*)out + dDescSizePrev + 10) != 0x85)) {
            //WdfRegistryClose(KeyParameters);
            WdfRegistryClose(KeyDevice);
            //ExFreePoolWithTag(out, MEM_TAG_HIDRPRT);
            continue;
        };
        // Byte 11 should hold the ID
        id = *((BYTE*)out + dDescSizePrev + 11);

#endif // 0

        reportId = ParseIdInDescriptor((BYTE*)out + dDescSizePrev, dDescSize);
        TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "GetReportDescriptorFromRegistry: parsed id=%d, size=%d\n", reportId, dDescSize);

        SHORT ffbEnabled = ParsePIDCollectionForFFBInDescriptor((BYTE*)out + dDescSizePrev, dDescSize);
        TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "GetReportDescriptorFromRegistry: id=%d, ffb=%d\n", reportId, ffbEnabled);

        // Check if this is a unique ID
        if ((*IdMask) & (1<<(reportId-1))) {
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "GetReportDescriptorFromRegistry: unique ID, continue\n");
            WdfRegistryClose(KeyDevice);
            continue;
        }

        // Update ID mask
        *IdMask |= 1<<(reportId-1);
        //////////////////////////////////////////////////////////////////////////////

        // Update FFB mask
        if (ffbEnabled>=0) {
            *FFbmask |= 1<<(reportId-1);
        }

        TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "GetReportDescriptorFromRegistry: mask=0x%x, ffbmask=0x%x\n", *IdMask, *FFbmask);

        dDescSizePrev += dDescSize;
        WdfRegistryClose(KeyDevice);

    } // Loop on all sub-keys

    WdfRegistryClose(KeyParameters);

    *Size = dDescSizePrev;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "GetReportDescriptorFromRegistry: exiting with size=%d, mask=0x%x\n", *Size, *IdMask);

    return out;
}

/*++
Routine Description:

    GetInitValueFromRegistry helper function reads the initialization values for the controls of a device.

    Returns:
    Bit-Mask where every bit refers to an entry in the output buffer.
    If a bit is set the the entry (Axis, POV or the button mask) is valid.
    If none is valid (or the key is missing) returns 0.

    Parameters:
    id [in]:			The id of the device to initialize
    data_buf[in/out]:	In:		Size of buffer in memger cb
                        Out:	Initialization data

--*/
unsigned int GetInitValueFromRegistry(USHORT		id, PDEVICE_INIT_VALS data_buf)
{
    NTSTATUS				status = STATUS_SUCCESS;
    WDFKEY					KeyDevice, KeyParameters;
    WCHAR					DeviceKeyName[NameSize] = { 0 };
    UNICODE_STRING			strDev, strControl;
    PCWSTR					Axes[] = {
        L"X", L"Y", L"Z", L"RX", L"RY", L"RZ", L"SL1", L"SL2",
        L"WHL", L"ACC", L"BRK", L"CLU", L"STE", L"AIL", L"RUD", L"THR",
        L"POV1", L"POV2", L"POV3", L"POV4" };
    UCHAR					nAxes = 0;
    int						iAxis;
    unsigned int			Mask = 0;
    const int				nButtons = VJOY_NUMBER_OF_BUTTONS;

    PAGED_CODE();

    // Check that buffer size is sufficient
    nAxes = sizeof(Axes) / sizeof(PCWSTR);
    // BM was sizeof(nButtons) / 8
    // Size in bytes, axes include pov (16+4)
    if (data_buf->cb < (2 + nAxes + (sizeof(nButtons)/8))) {
        LogEventWithStatus(ERRLOG_REP_REG_FAILED, L"GetInitValueFromRegistry: Buffer size too small", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        return 0;
    };
    data_buf->id = id;

    // Get the key of the Parameters key under "SYSTEM\\CurrentControlSet\\services\\vjoy"
    status = WdfDriverOpenParametersRegistryKey(WdfGetDriver(), WRITE_DAC, WDF_NO_OBJECT_ATTRIBUTES, &KeyParameters);
    if (!NT_SUCCESS(status)) {
        LogEventWithStatus(ERRLOG_REP_REG_FAILED, L"WdfDriverOpenParametersRegistryKey", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        return 0;
    };
    status = STATUS_SUCCESS;

    // Calculate the string of the registry key
    status = RtlStringCchPrintfExW(DeviceKeyName, sizeof(DeviceKeyName) / sizeof(WCHAR), NULL, NULL, STRSAFE_NO_TRUNCATION, REG_DEVICE L"%02d\\" REG_INIT, id);
    if (!NT_SUCCESS(status)) {
        LogEventWithStatus(ERRLOG_REP_REG_FAILED, L"RtlStringCchPrintfExW", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        return 0;
    };
    RtlUnicodeStringInit(&strDev, DeviceKeyName);


    // Open a handle to the key
    status = WdfRegistryOpenKey(KeyParameters, &strDev, GENERIC_READ, WDF_NO_OBJECT_ATTRIBUTES, &KeyDevice);
    if (!NT_SUCCESS(status)) {
        if (STATUS_OBJECT_NAME_NOT_FOUND != status)
            LogEventWithStatus(ERRLOG_REP_REG_FAILED, L"WdfRegistryOpenKey", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        WdfRegistryClose(KeyParameters);
        return 0;
    };

    // Analyze axes	& POVs
    for (iAxis = 0; iAxis < nAxes; iAxis++) {
        RtlUnicodeStringInit(&strControl, Axes[iAxis]);
        status = WdfRegistryQueryValue(KeyDevice, &strControl, 1, &(data_buf->InitValAxis[iAxis]), NULL, NULL); //val
        if (status == STATUS_SUCCESS)
            Mask |= 0x01;
        Mask = Mask << 1;
    };

    // Analyze buttons BTN_INIT
    RtlUnicodeStringInit(&strControl, BTN_INIT);
    status = WdfRegistryQueryValue(KeyDevice, &strControl, 16, &(data_buf->ButtonMask), NULL, NULL);
    if (status == STATUS_SUCCESS)
        Mask |= 0x01;

    WdfRegistryClose(KeyParameters);
    WdfRegistryClose(KeyDevice);

    return Mask;
}

/*
    CalcInitValue helper function calculates the initialization values for the controls of a device.

    1.	Calls GetInitValueFromRegistry() for device #n.
    2.	If returns all-valid (13 lower bits are set = 0x1FFF) then use the buffer for initialization.
    3.	Otherwise store the buffer (Device Data) and the returned value (Device Validity mask)
        and call GetInitValueFromRegistry() for device #0.
    4.	Store device #0 buffer (Master Data) and  returned value (Master Validity mask).
    5.	For every bit that is 0 in Device Validity mask and 1 in Master Validity mask:
        a.	Copy value from Master to Device data buffer.
        b.	Set bit in Device Validity mask
    6.	For every bit that is 0 in Device Validity mask
        a.	Set value in buffer to hardcoded default value
        b.	Set bit in Device Validity mask

        Parameters:
            id [in]:			The id of the device to initialize
            data_buf[in/out]:	In:		Size of buffer in memger cb
                                Out:	Initialization data

*/
void   CalcInitValue(USHORT		id, PDEVICE_INIT_VALS data_buf)
{

    // Hardcoded values
    DEVICE_INIT_VALS  init_master;
    unsigned int mask_device = 0, mask_master = 0;
    int i, j, k;
    UCHAR InitValAxis[VJOY_NUMBER_OF_AXES] = { 50, 50, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    UCHAR InitValPov[VJOY_NUMBER_OF_HAT] = { (UCHAR)-1, (UCHAR)-1, (UCHAR)-1, (UCHAR)-1 };
    UCHAR ButtonMask[VJOY_NUMBER_OF_BUTTONS/8] = { 0 };
    int nAxes, nPovs, offset;

    PAGED_CODE();

    // Prepare buffer and get init data for device from registry
    data_buf->cb = sizeof(DEVICE_INIT_VALS);
    data_buf->id = id;

    // First, get initialization values for device.
    // Special case: default (hardcoded device) - skip this stage.
    if (id) {
        mask_device = GetInitValueFromRegistry(id, data_buf);
        if (mask_device == 0x1FFF) // all data taken from registry?
            return;
    };

    // Need to get additional data from Master (device 0)
    init_master.cb = sizeof(DEVICE_INIT_VALS);
    init_master.id = id;
    mask_master = GetInitValueFromRegistry(0, &init_master);

    // Merge Axes
    nAxes = (sizeof(data_buf->InitValAxis) / sizeof(data_buf->InitValAxis[0]));
    nPovs = (sizeof(data_buf->InitValPov) / sizeof(data_buf->InitValPov[0]));
    for (i = 0; i <nAxes; i++) {
        offset = nAxes + nPovs- i;
        if (!(mask_device & (1 << offset))) {
            if (mask_master & (1 << offset))
                data_buf->InitValAxis[i] = init_master.InitValAxis[i];
            else
                data_buf->InitValAxis[i] = InitValAxis[i];
        };
    };

    // Merge POVs
    for (j = 0; j < nPovs; i++, j++) {
        offset = nAxes + nPovs - i;
        if (!(mask_device & (1 << offset))) {
            if (mask_master & (1 << offset))
                data_buf->InitValPov[j] = init_master.InitValPov[j];
            else
                data_buf->InitValPov[j] = InitValPov[j];
        };
    };

    // Buttons
    if (!(mask_device & 1)) {
        if (mask_master & 1)
            for (k = 0; k < (sizeof(data_buf->ButtonMask) / sizeof(data_buf->ButtonMask[0])); k++)
                data_buf->ButtonMask[k] = init_master.ButtonMask[k];
        else
            for (k = 0; k < (sizeof(data_buf->ButtonMask) / sizeof(data_buf->ButtonMask[0])); k++)
                data_buf->ButtonMask[k] = ButtonMask[k];
    };

}

/*
    This function updates field wReportLength in structure HID_DESCRIPTOR
*/
void UpdateHidDescriptor(void)
{
    USHORT	Size = 0, Mask = 0, FFBMask = 0;
    PVOID	out;

    // Get the size from the registry
    out = GetReportDescriptorFromRegistry(&Size, &Mask, &FFBMask);

    if (Size) {
        ExFreePoolWithTag(out, MEM_TAG_HIDRPRT);
        G_DefaultHidDescriptor.DescriptorList[0].wReportLength = Size;
    } else
        G_DefaultHidDescriptor.DescriptorList[0].wReportLength = sizeof(G_DefaultReportDescriptor);

}

/*
    This function determines the number of collections (virtual joystick devices)
    attached to this device and the features of each collection (device).
    The data comes from the registry.
    If the registry does not hold valid data then the data is taken from the hard-coded defaults.
    The function updates the following data structures in the context area:
        > positions[]: Allocates a structure for every valid device (NULL for invalid)
        > nDevices: Number of devices
        > ReportDescriptor: pointer to the valid Report Descriptor (May contain 1 to 16 top-level collections)
    and updates G_DefaultHidDescriptor.DescriptorList[0].wReportLength
*/
void UpdateCollections(WDFDEVICE Device)
{
    USHORT	Size = 0, Mask = 0, FFBMask = 0;
    PVOID	out;
    PDEVICE_EXTENSION   devContext = NULL;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "UpdateCollections: entering\n");

    // Initialize
    devContext = GetDeviceContext(Device);
    CleanUpDev(devContext);

    // Get the data, size and IDs from the registry
    out = GetReportDescriptorFromRegistry(&Size, &Mask, &FFBMask);
    if (out && Mask && Size) {	/* Registry holds valid data */

        TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "UpdateCollections: valid registry data\n");

        InitializeDev(devContext, Mask, FFBMask, FALSE);
        devContext->isHardcodedDevice = FALSE;
        // Free HID Report Descriptor
        if (devContext->ReportDescriptor) {
            ExFreePoolWithTag(devContext->ReportDescriptor, MEM_TAG_HIDRPRT);
            devContext->ReportDescriptor = NULL;
        }

        // Store new descriptor
        devContext->ReportDescriptor = out;
        G_DefaultHidDescriptor.DescriptorList[0].wReportLength = Size;
        TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "UpdateCollections: new registry entry, with size=%d\n", Size);

    } else {
        /* Use default (hard-coded) data */
        TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "UpdateCollections: invalid registry data, using hard-coded values\n");

        InitializeDefaultDev(devContext);
        devContext->isHardcodedDevice = TRUE;
    }
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "UpdateCollections: exiting\n");

}

/*
    Initialize data structures in the device context area
*/
void InitializeDeviceContext(PDEVICE_EXTENSION   devContext)
{
    // Init array of pointers to reports
    for (int id = 1; id<=VJOY_MAX_N_DEVICES; id++) {
        devContext->positions[id-1] = NULL;
        Ffb_ResetPIDData(devContext, id);
    }

    devContext->nDevices = 0;
    devContext->ReportDescriptor = NULL;
    devContext->positionLock = NULL;
}

/*
    Initialize data structures representing virtual devices
    to default (hard-coded) values.
    These values may later be overriden by user-defined data
*/
void InitializeDefaultDev(PDEVICE_EXTENSION   devContext)
{
    int index, n;
    UCHAR* cTag;
    ULONG	Tag = '00DI';
    const UCHAR	SerialL[16] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6' };
    const UCHAR	SerialH[16] = { '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', '1', '1', '1' };
    DEVICE_INIT_VALS data_buf;

    cTag = (UCHAR*)&Tag;
    n = devContext->nDevices = N_COLLECTIONS;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "InitializeDefaultDev: entering\n");

    if (devContext->positionLock) {
        WdfWaitLockAcquire(devContext->positionLock, NULL);
    }

    // Create a DEVICE_POSITION structure for each hard-coded top-level collection
    for (index = 0; index<n; index++) {

        // Mark default device(s) as implemented
        devContext->DeviceImplemented[index] = TRUE;
        TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "InitializeDefaultDev: enabling id=%d\n", index+1);

        //id = index+1;
        cTag[3] = SerialL[index];
        cTag[2] = SerialH[index];
        devContext->positions[index] = ExAllocatePoolWithTag(NonPagedPool, sizeof(DEVICE_POSITION), Tag);
        if (!devContext->positions[index])
            break;

        // Get initialization values (From device #0)
        CalcInitValue(0, &data_buf);

        // Initialize all fields
        devContext->positions[index]->ValX = data_buf.InitValAxis[0] * VJOY_AXIS_MAX_VALUE / 100 + 1;//0x7FFF/2+1;
        devContext->positions[index]->ValY = data_buf.InitValAxis[1] * VJOY_AXIS_MAX_VALUE / 100 + 1;//0x7FFF/2+1;
        devContext->positions[index]->ValZ = data_buf.InitValAxis[2] * VJOY_AXIS_MAX_VALUE / 100 + 1;//0x7FFF/2+1;
        devContext->positions[index]->ValRX = data_buf.InitValAxis[3] * VJOY_AXIS_MAX_VALUE / 100 + 1;
        devContext->positions[index]->ValRY = data_buf.InitValAxis[4] * VJOY_AXIS_MAX_VALUE / 100 + 1;
        devContext->positions[index]->ValRZ = data_buf.InitValAxis[5] * VJOY_AXIS_MAX_VALUE / 100 + 1;
        devContext->positions[index]->ValSlider = data_buf.InitValAxis[6] * VJOY_AXIS_MAX_VALUE / 100 + 1;
        devContext->positions[index]->ValDial = data_buf.InitValAxis[7] * VJOY_AXIS_MAX_VALUE / 100 + 1;

        devContext->positions[index]->ValWheel = data_buf.InitValAxis[8] * VJOY_AXIS_MAX_VALUE / 100 + 1;
        devContext->positions[index]->ValAccelerator = data_buf.InitValAxis[9] * VJOY_AXIS_MAX_VALUE / 100 + 1;
        devContext->positions[index]->ValBrake = data_buf.InitValAxis[10] * VJOY_AXIS_MAX_VALUE / 100 + 1;
        devContext->positions[index]->ValClutch = data_buf.InitValAxis[11] * VJOY_AXIS_MAX_VALUE / 100 + 1;
        devContext->positions[index]->ValSteering = data_buf.InitValAxis[12] * VJOY_AXIS_MAX_VALUE / 100 + 1;
        devContext->positions[index]->ValThrottle = data_buf.InitValAxis[13] * VJOY_AXIS_MAX_VALUE / 100 + 1;
        devContext->positions[index]->ValRudder = data_buf.InitValAxis[14] * VJOY_AXIS_MAX_VALUE / 100 + 1;
        devContext->positions[index]->ValAileron = data_buf.InitValAxis[15] * VJOY_AXIS_MAX_VALUE / 100 + 1;

        if (data_buf.InitValPov[0] == -1)
            devContext->positions[index]->ValHats = (DWORD)-1;
        else
            devContext->positions[index]->ValHats = data_buf.InitValPov[0] * 0x7FFF / 100 + 1;

        if (data_buf.InitValPov[1] == -1)
            devContext->positions[index]->ValHatsEx1 = (DWORD)-1;
        else
            devContext->positions[index]->ValHatsEx1 = data_buf.InitValPov[1] * 0x7FFF / 100 + 1;

        if (data_buf.InitValPov[2] == -1)
            devContext->positions[index]->ValHatsEx2 = (DWORD)-1;
        else
            devContext->positions[index]->ValHatsEx2 = data_buf.InitValPov[2] * 0x7FFF / 100 + 1;

        if (data_buf.InitValPov[3] == -1)
            devContext->positions[index]->ValHatsEx3 = (DWORD)-1;
        else
            devContext->positions[index]->ValHatsEx3 = data_buf.InitValPov[3] * 0x7FFF / 100 + 1;

        devContext->positions[index]->ValButtons = ((DWORD*)(data_buf.ButtonMask))[0];
        devContext->positions[index]->ValButtonsEx1 = ((DWORD*)(data_buf.ButtonMask))[1];
        devContext->positions[index]->ValButtonsEx2 = ((DWORD*)(data_buf.ButtonMask))[2];
        devContext->positions[index]->ValButtonsEx3 = ((DWORD*)(data_buf.ButtonMask))[3];
    };

    if (devContext->positionLock) {
        WdfWaitLockRelease(devContext->positionLock);
    }

    // Copy default HID Report Descriptor
    devContext->ReportDescriptor = ExAllocatePoolWithTag(PagedPool, sizeof(G_DefaultReportDescriptor), MEM_TAG_HIDRPRT);
    if (!devContext->ReportDescriptor)
        return;
    RtlCopyMemory(devContext->ReportDescriptor, G_DefaultReportDescriptor, sizeof(G_DefaultReportDescriptor));

    // Init HID Descriptor
    G_DefaultHidDescriptor.DescriptorList[0].wReportLength = sizeof(G_DefaultReportDescriptor);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "InitializeDefaultDev: exiting\n");

}

/*
    Reset values of data structures representing virtual devices
*/
void ResetDeviceControls(int id, PDEVICE_EXTENSION devContext, PDEVICE_INIT_VALS pdata_buf)
{
    int index = id - 1;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "ResetDeviceControls: Resetting device:%d [requested]\n", id);

    // Sanity check
    if (!devContext || id < 1 || id > VJOY_MAX_N_DEVICES || !devContext->positions[index] || !pdata_buf)
        return;

    // Device exists?
    if (!devContext->DeviceImplemented[index])
        return;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "ResetDeviceControls: Resetting device:%d [granted]\n", id);

    // Initialize all fields
    devContext->positions[index]->ValX = pdata_buf->InitValAxis[0] * VJOY_AXIS_MAX_VALUE / 2 + 1;//0x7FFF/2+1;
    devContext->positions[index]->ValY = pdata_buf->InitValAxis[1] * VJOY_AXIS_MAX_VALUE / 2 + 1;//0x7FFF/2+1;
    devContext->positions[index]->ValZ = pdata_buf->InitValAxis[2] * VJOY_AXIS_MAX_VALUE / 2 + 1;//0x7FFF/2+1;
    devContext->positions[index]->ValRX = pdata_buf->InitValAxis[3] * VJOY_AXIS_MAX_VALUE / 2 + 1;
    devContext->positions[index]->ValRY = pdata_buf->InitValAxis[4] * VJOY_AXIS_MAX_VALUE / 2 + 1;
    devContext->positions[index]->ValRZ = pdata_buf->InitValAxis[5] * VJOY_AXIS_MAX_VALUE / 2 + 1;
    devContext->positions[index]->ValSlider = pdata_buf->InitValAxis[6] * VJOY_AXIS_MAX_VALUE / 2 + 1;
    devContext->positions[index]->ValDial = pdata_buf->InitValAxis[7] * VJOY_AXIS_MAX_VALUE / 2 + 1;

    devContext->positions[index]->ValWheel = pdata_buf->InitValAxis[8] * VJOY_AXIS_MAX_VALUE / 2 + 1;
    devContext->positions[index]->ValAccelerator = pdata_buf->InitValAxis[9] * VJOY_AXIS_MAX_VALUE / 2 + 1;
    devContext->positions[index]->ValBrake = pdata_buf->InitValAxis[10] * VJOY_AXIS_MAX_VALUE / 2 + 1;
    devContext->positions[index]->ValClutch = pdata_buf->InitValAxis[11] * VJOY_AXIS_MAX_VALUE / 2 + 1;
    devContext->positions[index]->ValSteering = pdata_buf->InitValAxis[12] * VJOY_AXIS_MAX_VALUE / 2 + 1;
    devContext->positions[index]->ValThrottle = pdata_buf->InitValAxis[13] * VJOY_AXIS_MAX_VALUE / 2 + 1;
    devContext->positions[index]->ValRudder = pdata_buf->InitValAxis[14] * VJOY_AXIS_MAX_VALUE / 2 + 1;
    devContext->positions[index]->ValAileron = pdata_buf->InitValAxis[15] * VJOY_AXIS_MAX_VALUE / 2 + 1;

    if (pdata_buf->InitValPov[0] == -1)
        devContext->positions[index]->ValHats = (DWORD)-1;
    else
        devContext->positions[index]->ValHats = pdata_buf->InitValPov[0] * 35999 / 100;

    if (pdata_buf->InitValPov[1] == -1)
        devContext->positions[index]->ValHatsEx1 = (DWORD)-1;
    else
        devContext->positions[index]->ValHatsEx1 = pdata_buf->InitValPov[1] * 35999 / 100;

    if (pdata_buf->InitValPov[2] == -1)
        devContext->positions[index]->ValHatsEx2 = (DWORD)-1;
    else
        devContext->positions[index]->ValHatsEx2 = pdata_buf->InitValPov[2] * 35999 / 100;

    if (pdata_buf->InitValPov[3] == -1)
        devContext->positions[index]->ValHatsEx3 = (DWORD)-1;
    else
        devContext->positions[index]->ValHatsEx3 = pdata_buf->InitValPov[3] * 35999 / 100;

    devContext->positions[index]->ValButtons = ((DWORD*)(pdata_buf->ButtonMask))[0];
    devContext->positions[index]->ValButtonsEx1 = ((DWORD*)(pdata_buf->ButtonMask))[1];
    devContext->positions[index]->ValButtonsEx2 = ((DWORD*)(pdata_buf->ButtonMask))[2];
    devContext->positions[index]->ValButtonsEx3 = ((DWORD*)(pdata_buf->ButtonMask))[3];
}

/*
    Initialize data structures representing virtual devices
*/
void InitializeDev(PDEVICE_EXTENSION devContext, USHORT Mask, USHORT FFBMask, BOOLEAN ResetOnly)
{
    int index;
    UCHAR* cTag;
    ULONG	Tag = '00DI';
    const UCHAR	SerialL[16] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6' };
    const UCHAR	SerialH[16] = { '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', '1', '1', '1' };
    DEVICE_INIT_VALS data_buf;

    cTag = (UCHAR*)&Tag;
    devContext->nDevices = 0;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "InitializeDev: entering with mask=0x%x FFBMask=0x%x, ResetOnly=0x%x\n", Mask, FFBMask, ResetOnly);

    if (devContext->positionLock) {
        WdfWaitLockAcquire(devContext->positionLock, NULL);
    }

    // Create a DEVICE_POSITION structure for each hard-coded top-level collection
    for (index = 0; index<VJOY_MAX_N_DEVICES; index++) {

        // Mask bit will give implemented status
        if (!((Mask >> index) & 1)) {
            if (!ResetOnly) {
                devContext->DeviceImplemented[index] = FALSE;
                TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "InitializeDev: disabling id=%d\n", index+1);
            }
            continue;
        }

        // Initially - no file objects associated with the device
        devContext->DeviceFileObject[index] = NULL;

        devContext->nDevices++;
        devContext->DeviceImplemented[index] = TRUE;
        TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "InitializeDev: enabling id=%d\n", index+1);

        if ((FFBMask >> index) & 1) {
#ifdef VJOY_HAS_FFB
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "InitializeDev: enabling FFB for id=%d\n", index+1);
            //FfbActiveSet(TRUE, index+1, devContext);
#else
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "InitializeDev: asked for FFB but disabled in driver!! id=%d\n", index+1);
            //FfbActiveSet(FALSE, index+1, devContext);
#endif
        } else {
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "InitializeDev: disabling FFB for id=%d\n", index+1);
            //FfbActiveSet(FALSE, index+1, devContext);
        }

        cTag[3] = SerialL[index];
        cTag[2] = SerialH[index];
        devContext->positions[index] = ExAllocatePoolWithTag(NonPagedPool, sizeof(DEVICE_POSITION), Tag);
        // Check device's position struct was allocated
        if (!devContext->positions[index]) {
            // We got an error here
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "InitializeDev: error for id=%d, position struct not allocated!\n", index);
            break;
        }

        // Get initialization values
        CalcInitValue((USHORT)index + 1, &data_buf);

        // Initialize all fields
        devContext->positions[index]->ValX = data_buf.InitValAxis[0] * VJOY_AXIS_MAX_VALUE / 2 +1;//0x7FFF/2+1;
        devContext->positions[index]->ValY = data_buf.InitValAxis[1] * VJOY_AXIS_MAX_VALUE / 2 + 1;//0x7FFF/2+1;
        devContext->positions[index]->ValZ = data_buf.InitValAxis[2] * VJOY_AXIS_MAX_VALUE / 2 + 1;//0x7FFF/2+1;
        devContext->positions[index]->ValRX = data_buf.InitValAxis[3] * VJOY_AXIS_MAX_VALUE / 2 + 1;
        devContext->positions[index]->ValRY = data_buf.InitValAxis[4] * VJOY_AXIS_MAX_VALUE / 2 + 1;
        devContext->positions[index]->ValRZ = data_buf.InitValAxis[5] * VJOY_AXIS_MAX_VALUE / 2 + 1;
        devContext->positions[index]->ValSlider = data_buf.InitValAxis[6] * VJOY_AXIS_MAX_VALUE / 2 + 1;
        devContext->positions[index]->ValDial = data_buf.InitValAxis[7] * VJOY_AXIS_MAX_VALUE / 2 + 1;
        // Should be 0?
        devContext->positions[index]->ValWheel = data_buf.InitValAxis[8] * VJOY_AXIS_MAX_VALUE / 2 + 1;
        devContext->positions[index]->ValAccelerator = data_buf.InitValAxis[9] * VJOY_AXIS_MAX_VALUE / 2 + 1;
        devContext->positions[index]->ValBrake = data_buf.InitValAxis[10] * VJOY_AXIS_MAX_VALUE / 2 + 1;
        devContext->positions[index]->ValClutch = data_buf.InitValAxis[11] * VJOY_AXIS_MAX_VALUE / 2 + 1;
        devContext->positions[index]->ValSteering = data_buf.InitValAxis[12] * VJOY_AXIS_MAX_VALUE / 2 + 1;
        devContext->positions[index]->ValThrottle = data_buf.InitValAxis[13] * VJOY_AXIS_MAX_VALUE / 2 + 1;
        devContext->positions[index]->ValRudder = data_buf.InitValAxis[14] * VJOY_AXIS_MAX_VALUE / 2 + 1;
        devContext->positions[index]->ValAileron = data_buf.InitValAxis[15] * VJOY_AXIS_MAX_VALUE / 2 + 1;

        // Mark position data as ready to be read
        devContext->PositionReady[index] = TRUE;

        // Test if the initialization values refer to Discrete POVs
        // The sign of Discrete POV initialization is value in the range 0x80-0x8F
        // If one or more values are in the range it is adssumed the POVs are Discrete
        if (
            ((data_buf.InitValPov[0] >> 4) == 8) ||
            ((data_buf.InitValPov[1] >> 4) == 8) ||
            ((data_buf.InitValPov[2] >> 4) == 8) ||
            ((data_buf.InitValPov[3] >> 4) == 8)
            )
            devContext->positions[index]->ValHats =
            ((data_buf.InitValPov[0] & 0xF) << 0) +
            ((data_buf.InitValPov[1] & 0xF) << 4) +
            ((data_buf.InitValPov[2] & 0xF) << 8) +
            ((data_buf.InitValPov[3] & 0xF) << 12);


        // These are Continuous POVs
        else {
            if (data_buf.InitValPov[0] == -1)
                devContext->positions[index]->ValHats = (DWORD)-1;
            else
                devContext->positions[index]->ValHats = data_buf.InitValPov[0] * 35999 / 100;

            if (data_buf.InitValPov[1] == -1)
                devContext->positions[index]->ValHatsEx1 = (DWORD)-1;
            else
                devContext->positions[index]->ValHatsEx1 = data_buf.InitValPov[1] * 35999 / 100;

            if (data_buf.InitValPov[2] == -1)
                devContext->positions[index]->ValHatsEx2 = (DWORD)-1;
            else
                devContext->positions[index]->ValHatsEx2 = data_buf.InitValPov[2] * 35999 / 100;

            if (data_buf.InitValPov[3] == -1)
                devContext->positions[index]->ValHatsEx3 = (DWORD)-1;
            else
                devContext->positions[index]->ValHatsEx3 = data_buf.InitValPov[3] * 35999 / 100;
        };

        devContext->positions[index]->ValButtons = ((DWORD*)(data_buf.ButtonMask))[0];
        devContext->positions[index]->ValButtonsEx1 = ((DWORD*)(data_buf.ButtonMask))[1];
        devContext->positions[index]->ValButtonsEx2 = ((DWORD*)(data_buf.ButtonMask))[2];
        devContext->positions[index]->ValButtonsEx3 = ((DWORD*)(data_buf.ButtonMask))[3];

        TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "InitializeDev: id=%d was initialized with default position values\n", index+1);

    }

    if (devContext->positionLock) {
        WdfWaitLockRelease(devContext->positionLock);
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "InitializeDev: leaving with %d devices initialized\n", devContext->nDevices);

}



/*
    Remove data structures representing virtual devices
*/
void CleanUpDev(PDEVICE_EXTENSION   devContext)
{
    int index;
    UCHAR* cTag;
    ULONG	Tag = '00DI';
    const UCHAR	SerialL[16] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6' };
    const UCHAR	SerialH[16] = { '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', '1', '1', '1' };
    LONGLONG timeout = 0;

    cTag = (UCHAR*)&Tag;


    // Destroy and init DEVICE_POSITION structure for each hard-coded top-level collection
    if (STATUS_SUCCESS == WdfWaitLockAcquire(devContext->positionLock, &timeout)) {
        for (index = 0; index<VJOY_MAX_N_DEVICES; index++) {
            if (devContext->positions[index]) {
                cTag[3] = SerialL[index];
                cTag[2] = SerialH[index];
                ExFreePoolWithTag(devContext->positions[index], Tag);
                devContext->positions[index] = NULL;
            };
        };
        WdfWaitLockRelease(devContext->positionLock);
    };
    devContext->nDevices = 0;


    // Free HID Report Descriptor
    if (devContext->ReportDescriptor) {
        G_DefaultHidDescriptor.DescriptorList[0].wReportLength = 0;
        ExFreePoolWithTag(devContext->ReportDescriptor, MEM_TAG_HIDRPRT);
        devContext->ReportDescriptor = NULL;
    };
}

// Helper function that reallocate a pool
// Syntax follows ExAllocatePoolWithTag + pointer to original pool
PVOID ReAllocatePoolWithTag(PVOID orig, SIZE_T prevSize, POOL_TYPE PoolType, SIZE_T NumberOfBytes, ULONG Tag)
{
    PVOID tmpPool, out;
    ULONG tmpTag = 'pmeT'; // 'Temp'

    if ((PoolType & NonPagedPoolMustSucceed) != 0)
        return NULL;

    if (!orig || !prevSize) {
        out = ExAllocatePoolWithTag(PoolType, NumberOfBytes, Tag);
        if (out)
            return out;
        return NULL;
    }

    tmpPool = ExAllocatePoolWithTag(PoolType, prevSize, tmpTag);
    if (!tmpPool)
        return NULL;

    RtlZeroMemory(tmpPool, prevSize);
    RtlCopyMemory(tmpPool, orig, prevSize);

    ExFreePoolWithTag(orig, Tag);
    orig = NULL;

    out = ExAllocatePoolWithTag(PoolType, NumberOfBytes, Tag);
    if (!out) {
        ExFreePoolWithTag(tmpPool, tmpTag);
        return NULL;
    }
    RtlZeroMemory(out, prevSize);
    RtlCopyMemory(out, tmpPool, prevSize);
    ExFreePoolWithTag(tmpPool, tmpTag);

    return out;
}

USHORT ParseIdInDescriptor(BYTE* desc, DWORD dDescSize)
// Parse descriptor (read from the registry)
// Look for 0x85 in the descriptor
// then read the next byte - this is the id
// Test the ID - should be in the range 1 to MAX_N_DEVICES
// Return valid ID - else return 0
{
    DWORD i;
    USHORT id = 0;

    for (i = 0; i < dDescSize-1; i++) {
        if (*desc == 0x85) {
            id = *(desc + 1);
            break;
        }
        desc++;
    }
    if (id < 1 || id>VJOY_MAX_N_DEVICES) {
        id = 0;
    }

    return id;
}

/*
 Parse descriptor (read from the registry) to see whether a FFB is used (Physical interface)
 Look for first occurrence of 0x05,0x0F in the descriptor (Usage Page Physical Interface)
 Alternatively we could look for 0x09,0x21 (Usage Set Effect Report)

 Returns -1 if not found, else an index of first position found in the descriptor
 */
SHORT ParsePIDCollectionForFFBInDescriptor(BYTE* desc, DWORD dDescSize)
{
    SHORT first_pos = -1;

    for (DWORD i = 0; i < dDescSize-1; i++) {
        if (*desc == 0x05) {
            if (*(desc + 1) == 0x0F) {
                first_pos = (SHORT)i;
                break;
            }
        }
        desc++;
    }

    return first_pos;
}

VOID  FfbNotifyWrite(
    WDFQUEUE Queue,
    WDFCONTEXT Context
)
// Called whenever the WriteQ (FfbWriteQ[]) is ready.
// (Ready means that a first request has arrived)
// This function tests that the ReadQ (FfbReadQ[]) has a pending request.
// If so - it calls the main transfer function
{
    PDEVICE_EXTENSION           devContext = NULL;
    ULONG						QueueRequests;
    PFFB_QUEUE_EXTENSION		queueContext;
    int							id;

    devContext = (PDEVICE_EXTENSION)Context;

    // Extract the device ID from the queue	and test it for legal value (1-16)
    queueContext = GetFfbQueuContext(Queue);
    if (!queueContext)
        return;
    id = queueContext->DeviceID;
    if (id<1 || id>16)
        return;

#if 0
    // FFB Active?
    if (!devContext->FfbActive)
        return;

#endif // 0

    // Other queue has a pending request?
    WdfIoQueueGetState(devContext->FfbReadQ[id-1], &QueueRequests, NULL);
    if (!QueueRequests)
        return;


    // OK - start transfer
    WdfWaitLockAcquire(devContext->FfbXferLock, NULL);
    FfbTransferData(devContext, id);
    WdfWaitLockRelease(devContext->FfbXferLock);
}

VOID FfbNotifyRead(
    WDFQUEUE Queue,
    WDFCONTEXT Context
)
// Called whenever the ReadQ (FfbReadQ[]) is ready.
// (Ready means that a first request has arrived)
// This function tests that the WriteQ (FfbWriteQ[]) has a pending request.
// If so - it calls the main transfer function
{
    PDEVICE_EXTENSION           devContext = NULL;
    ULONG						QueueRequests;
    PFFB_QUEUE_EXTENSION		queueContext;
    int							id;

    devContext = (PDEVICE_EXTENSION)Context;

    // Extract the device ID from the queue	and test it for legal value (1-16)
    queueContext = GetFfbQueuContext(Queue);
    if (!queueContext)
        return;
    id = queueContext->DeviceID;
    if (id<1 || id>16)
        return;

#if 0
    // FFB Active?
    if (!devContext->FfbActive)
        return;

#endif // 0

    // Other queue has a pending request?
    //WdfIoQueueGetState(devContext->FfbWriteQ[id-1], &QueueRequests, NULL);
    WdfIoQueueGetState(devContext->FfbReadQ[id-1], &QueueRequests, NULL);
    if (!QueueRequests)
        return;

    // OK - start transfer
    WdfWaitLockAcquire(devContext->FfbXferLock, NULL);
    FfbTransferData(devContext, id);
    WdfWaitLockRelease(devContext->FfbXferLock);
}

VOID FfbTransferData(
    PDEVICE_EXTENSION devContext,
    int 			  id
)
// This is where the WriteQ to ReadQ data transfer occurs.
// The transfer continues until one of the queues is empty.
{
    BOOLEAN					DataExist = FALSE;
    NTSTATUS				status = STATUS_SUCCESS;
    WDFREQUEST				WriteRequest, ReadRequest;
    PHID_XFER_PACKET		transferPacket = NULL;
    ULONG					DataSize = 0;
    ULONG					Cmd;
    WDF_REQUEST_PARAMETERS	Params;
    unsigned int			bytesToCopy;
    size_t					bytesReturned;
    PUCHAR					ReadBuffer = NULL;
    BOOLEAN					ReadQueueReady = FALSE, WriteQueueReady = FALSE;
    ULONG					QueueRequests;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_FFB, "FfbTransferData Enter\n");

    ///////////  Loop ///////////////////////////////////////
    do {
        // Get a Write request
        status = WdfIoQueueRetrieveNextRequest(devContext->FfbWriteQ[id - 1], &WriteRequest);
        if (!NT_SUCCESS(status))
            return;

        // Get data from the Write request
        transferPacket = (PHID_XFER_PACKET)WdfRequestWdmGetIrp(WriteRequest)->UserBuffer;
        if (!transferPacket)
            return;


        // Get the command from the Write request
        WDF_REQUEST_PARAMETERS_INIT(&Params);
        WdfRequestGetParameters(WriteRequest, &Params);
        Cmd = Params.Parameters.DeviceIoControl.IoControlCode;

        TraceEvents(TRACE_LEVEL_VERBOSE, DBG_FFB, "FfbTransferData IoControlCode=%x\n", Cmd);

        // Calculate the size of the Read data packet
        DataSize += sizeof(ULONG);						// Size of packet
        DataSize += sizeof(ULONG);						// Cmd
        DataSize += transferPacket->reportBufferLen;	// Raw data (First byte is report ID)

        // Get the Read request
        status = WdfIoQueueRetrieveNextRequest(devContext->FfbReadQ[id - 1], &ReadRequest);
        if (!NT_SUCCESS(status)) {
            WdfRequestComplete(WriteRequest, status);
            return;
        };

        // Calculate size of Read data
        WDF_REQUEST_PARAMETERS_INIT(&Params);
        WdfRequestGetParameters(ReadRequest, &Params);
        bytesToCopy = (unsigned int)Params.Parameters.DeviceIoControl.OutputBufferLength;
        if (bytesToCopy < DataSize) {
            status = STATUS_BUFFER_TOO_SMALL;
            WdfRequestComplete(WriteRequest, status);
            WdfRequestComplete(ReadRequest, status);
            return;
        };

        // Get Read request output buffer
        status = WdfRequestRetrieveOutputBuffer(ReadRequest, bytesToCopy, (PVOID)&ReadBuffer, &bytesReturned);
        if (!NT_SUCCESS(status)) {
            WdfRequestComplete(WriteRequest, status);
            WdfRequestComplete(ReadRequest, status);
            return;
        };

        // Copy data to Read request output buffer
        if (bytesReturned >= 4) {
            memcpy((PUCHAR)&(ReadBuffer[0]), (PUCHAR)&DataSize, sizeof(ULONG));
            memcpy((PUCHAR)&(ReadBuffer[sizeof(ULONG)]), (PUCHAR)&Cmd, sizeof(ULONG));
            memcpy((PUCHAR)&(ReadBuffer[2 * sizeof(ULONG)]), transferPacket->reportBuffer, transferPacket->reportBufferLen);
        };


        // Intercept Ffb Create New Effect and Free Effect

        TraceEvents(TRACE_LEVEL_VERBOSE, DBG_FFB, "FfbTransferData transferPacket DataSize=%d, id=%x len=%d  buf=",
            DataSize, transferPacket->reportId, transferPacket->reportBufferLen);
        for (ULONG i = 0; i<transferPacket->reportBufferLen; i++) {
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_FFB, "%x ",
                transferPacket->reportBuffer[i]
            );
        }
        TraceEvents(TRACE_LEVEL_VERBOSE, DBG_FFB, "\n");

        Ffb_ProcessPacket(devContext, id, ReadBuffer);


        // Complete Read request
        WdfRequestCompleteWithInformation(ReadRequest, status, bytesReturned);

        // Complete Write Request
        WdfRequestCompleteWithInformation(WriteRequest, status, transferPacket->reportBufferLen);

        // Ready to next transfer?
        WdfIoQueueGetState(devContext->FfbReadQ[id - 1], &QueueRequests, NULL);
        if (QueueRequests)
            ReadQueueReady = TRUE;
        else
            ReadQueueReady = FALSE;
        WdfIoQueueGetState(devContext->FfbWriteQ[id - 1], &QueueRequests, NULL);
        if (QueueRequests)
            WriteQueueReady = TRUE;
        else
            WriteQueueReady = FALSE;
        DataExist = ReadQueueReady && WriteQueueReady;

        TraceEvents(TRACE_LEVEL_INFORMATION, DBG_FFB, "FfbTransferData dataexist=%d\n", DataExist);

    } while (DataExist);
    ///////////  Loop ///////////////////////////////////////
}

int FfbGetDevIDfromFfbRequest(
    IN WDFREQUEST   Request
)
{
    // Analyse a IOCTL_HID_SET_FEATURE/IOCTL_HID_WRITE_REPORT report
    // Assuming this is an FFB IOCTL and assuming that the report ID:
    // 1. A one-byte ID
    // 2. ID's upper nibble carries is the Joystick ID
    // 3. Joystick ID may range 0x1-0xF
    // Return Joystick ID or 0 for error

    PHID_XFER_PACKET	transferPacket;
    int					id;

    // Get data from the Write request
    transferPacket = (PHID_XFER_PACKET)WdfRequestWdmGetIrp(Request)->UserBuffer;

    // Test transfer packet for validity
    if (!transferPacket || transferPacket->reportBufferLen == 0 || transferPacket->reportBufferLen < sizeof(UCHAR))
        return 0;

    // Get the upper nibble
    id = (transferPacket->reportId)>>4;

    // Verify ID within bounds
    if ((id<0) || (id>0xF))
        return 0;
    else
        return id;
}

void Ffb_ResetPIDData(
    PDEVICE_EXTENSION   devContext,
    int                 id)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_FFB, "Ffb_ResetPIDData: RESET PID for id=%d\n", id);

    // Default lastly created BlockIndex is 0 (not created)
    PFFB_DEVICE_PID pid = &(devContext->FfbPIDData[id-1]);

    pid->PIDPool.MaxSimultaneousEffects = VJOY_FFB_MAX_SIMULTANEOUS_EFFECTS;
    // bit0=1:own managed pool
    // bit1=0:one parameter block set for each effect block
    pid->PIDPool.MemoryManagement = 0;
    pid->PIDPool.RAMPoolSize = 0xFFFF;

    pid->PIDBlockLoad.EffectBlockIndex = 0;
    pid->PIDBlockLoad.LoadStatus = 0;
    pid->PIDBlockLoad.RAMPoolAvailable = 0xFFFF;

    Ffb_BlockIndexFreeAll(devContext, id);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_FFB, "Ffb_ResetPIDData: exiting\n");
}


BOOLEAN Ffb_CheckClientConnected(
    PDEVICE_EXTENSION   devContext,
    int                 id)
{
    ULONG QueueRequests;
    WdfIoQueueGetState(devContext->FfbReadQ[id-1], &QueueRequests, NULL);
    if (!QueueRequests)
        return FALSE;
    return TRUE;
}


/*
 Free all effect blocks index.

 This is used to reset internal data.
*/
void Ffb_BlockIndexFreeAll(
    PDEVICE_EXTENSION   devContext,
    int                 id)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_FFB, "FfbBlockIndexFreeAll: entering\n");

    PFFB_DEVICE_PID pid = &(devContext->FfbPIDData[id-1]);
    pid->LastEID = 0;
    pid->NextFreeEID = VJOY_FFB_FIRST_EFFECT_ID;
    for (int j = 0; j<VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX; j++) {
        pid->EffectStates[j].InUse = VJOY_FFB_EFFECT_FREE;
        pid->EffectStates[j].PIDEffectStateReport = 0;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_FFB, "FfbBlockIndexFreeAll: exiting\n");
}
/*
 Free an Effect Block Index for this device id.

 The next free effect slot index (NextEID) is updated with the freshly freed block.
*/
void Ffb_BlockIndexFree(
    PDEVICE_EXTENSION   devContext,
    int                 id,
    BYTE                EffectID
)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_FFB, "FfbBlockIndexFree: entering\n");
    PFFB_DEVICE_PID pid = &(devContext->FfbPIDData[id-1]);
    if (EffectID<1 || EffectID >= VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_FFB, "FfbBlockIndexFree: error wrong eid=%d\n", EffectID);
        return;
    }
    pid->EffectStates[EffectID-1].InUse = VJOY_FFB_EFFECT_FREE;
    // Force next free block to point to this new empty block to fill holes
    pid->NextFreeEID = EffectID;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_FFB, "FfbBlockIndexFree: exiting with NextEID=%d\n", pid->NextFreeEID);
}

/*
 Find a free Block Index for this device id.

 The function loops over all blocks and returns first free slot.

 LastEID and NextEID are updated.
*/
BYTE Ffb_GetNextFreeEffect(
    PDEVICE_EXTENSION   devContext,
    int                 id)
{
    // Retrieve device PID
    PFFB_DEVICE_PID pid = &(devContext->FfbPIDData[id-1]);
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_FFB, "FfbGetNextFreeEffect: enter with id=%d, NextEID=%d\n", id, pid->NextFreeEID);

    // Check for effect full
    if (pid->NextFreeEID > VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_FFB, "FfbGetNextFreeEffect: full! (NextEID=%d)\n", pid->NextFreeEID);
        return 0;
    }
    // Check current slot not in use
    if (pid->EffectStates[pid->NextFreeEID-1].InUse != VJOY_FFB_EFFECT_FREE) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_FFB, "FfbGetNextFreeEffect: already used! (NextEID=%d)\n", pid->NextFreeEID);
        return 0;
    }

    // Pick next free slot as new current slot
    pid->LastEID = pid->NextFreeEID;
    BYTE effectId = pid->LastEID;
    pid->EffectStates[effectId-1].InUse = VJOY_FFB_EFFECT_ALLOCATED;

    // Find the next free slot by scanning
    int firstFree = 0;
    for (; firstFree<VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX; firstFree++) {
        if (pid->EffectStates[firstFree].InUse == VJOY_FFB_EFFECT_FREE) {
            break;
        }
    }
    // Save index of first free. If not not found, will be equal to VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX
    // and the last free slot was taken.
    pid->NextFreeEID = (BYTE)(firstFree+1);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_FFB, "FfbGetNextFreeEffect: exiting with effectId=%d, NextFreeEID=%d)\n", effectId, pid->NextFreeEID);

    return effectId;
}

/*
 Find a free Block Index for this device id.

 The function loops over all blocks and returns first free slot.

 LastEID and NextEID are updated.
*/
BYTE Ffb_GetNumUsedEffect(
    PDEVICE_EXTENSION   devContext,
    int                 id)
{
    // Retrieve device PID
    PFFB_DEVICE_PID pid = &(devContext->FfbPIDData[id-1]);
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_FFB, "Ffb_GetNumUsedEffect: enter with id=%d, NextEID=%d\n", id, pid->NextFreeEID);

    BYTE numUsed = 0;
    for (int idx = 0; idx<VJOY_FFB_MAX_EFFECTS_BLOCK_INDEX; idx++) {
        if (pid->EffectStates[idx].InUse == VJOY_FFB_EFFECT_ALLOCATED) {
            numUsed++;
        }
    }
    return numUsed;
}

/*
 Process a FFB packet.

 This methods catches (or pre-filter) outgoing packets that are "read" feature report
 to return PID block data from driver side instead of gettig them user land
 feeder application.

 This is because current FFB design only allows single direction communication,
 ie: FFB packets can only be send to feeder application, and cannot go back to
 driver.
 */
BOOLEAN Ffb_ProcessPacket(
    PDEVICE_EXTENSION   devContext,
    int                 id,
    PUCHAR              buffer)
{
    // Routine validity checks
    PFFB_DEVICE_PID pid = &(devContext->FfbPIDData[id-1]);
    ULONG len = 0;
    ULONG cmd = 0;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_FFB, "Ffb_ProcessPacket: entering\n");

    //KdBreakPoint();
    if (!buffer)
        return FALSE;

    memcpy((PUCHAR)&len, (PUCHAR)&(buffer[0]), sizeof(ULONG));
    memcpy((PUCHAR)&cmd, (PUCHAR)&(buffer[sizeof(ULONG)]), sizeof(ULONG));

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "Ffb_ProcessPacket: before len=%d, cmd=%x buf=", len, cmd);

    if (len<8)
        return FALSE;

    PUCHAR packet = (PUCHAR)&buffer[2*sizeof(ULONG)];
    for (ULONG i = 0; i<(len-8); i++) {
        TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "%x ",
            packet[i]
        );
    }
    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_INIT, "\n");



    // Get the type
    BYTE tp = (packet[0]) & 0x0F;
    BYTE DeviceID = ((packet[0]&0xF0) >> 4);
    // This is a feature then mark it as such
    if (cmd == 0xb0191) // IOCTL_HID_SET_FEATURE ?
        tp += 0x10;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_FFB, "Ffb_ProcessPacket: id=%d DeviceID=%d tp=%x\n", id, DeviceID, tp);

    // Now catches (filters) packets data when PID block ionformation is needed
    // from driver side
    switch (tp) {
        // Next are write reports

        case HID_ID_CTRLREP: {
            // Catch PID control report
            BYTE Control = packet[1];
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_FFB, "Ffb_ProcessPacket: PID DEVICE CONTROL=%d\n", Control);
            switch (Control) {
                // CTRL_DEVRST
                // Device Reset– Clears any device paused condition, enables all actuators and clears all effects from memory.
                case 4: {
                    Ffb_ResetPIDData(devContext, id);
                    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_FFB, "Ffb_ProcessPacket: PID RESET !\n");
                } break;
            }
        } break;

        case HID_ID_BLKFRREP: {
            // Catch Block free report
            BYTE eid = packet[1];
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_FFB, "Ffb_ProcessPacket: BLOCK FREE eid=%d\n", eid);
            Ffb_BlockIndexFree(devContext, id, eid);
        } break;

            // Next are Feature report

        case (HID_ID_NEWEFREP+0x10): {
            // Catch New effect report

            // Find new effect block index. 0 will be returned in case of an error
            BYTE eid = Ffb_GetNextFreeEffect(devContext, id);
            // Save effect id in the current FFB frame to notify the feeder
            packet[2] = eid;
            // If not 0, then we succeed to find a free slot
            if (eid!=0) {
                // Save new effect block index for PID Block Load
                pid->PIDBlockLoad.EffectBlockIndex = eid;
                // Flag that created was ok
                pid->PIDBlockLoad.LoadStatus = 1;
            } else {
                // Error on last allocated block index
                // Flag error : full
                pid->PIDBlockLoad.LoadStatus = 2;
            }

            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_FFB, "Ffb_ProcessPacket: CREATE NEW EFFECT eid=%d\n", eid);
        } break;

        case (HID_ID_BLKLDREP+0x10): {
            // Handled in vJoyGetFeature()
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_FFB, "Ffb_ProcessPacket: BLOCK LOAD\n");
        } break;

        case (HID_ID_POOLREP+0x10): {
            // Handled in vJoyGetFeature()
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_FFB, "Ffb_ProcessPacket: POOL\n");
        } break;

        case (HID_ID_STATEREP+0x10): {
            // Handled in vJoyGetFeature()
            TraceEvents(TRACE_LEVEL_VERBOSE, DBG_FFB, "Ffb_ProcessPacket: STATE REPORT\n");
        } break;

    }

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_FFB, "Ffb_ProcessPacket: after buf=");
    packet = (PUCHAR)&buffer[2*sizeof(ULONG)];
    for (ULONG i = 0; i<(len-8); i++) {
        TraceEvents(TRACE_LEVEL_VERBOSE, DBG_FFB, "%x ",
            packet[i]
        );
    }
    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_FFB, "\n");

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_FFB, "Ffb_ProcessPacket: exiting");

    return TRUE;
}

