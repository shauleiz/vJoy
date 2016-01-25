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
    int					id=0;

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    device = WdfIoQueueGetDevice(Queue);
    devContext = GetDeviceContext(device);

    //
    // Please note that HIDCLASS provides the buffer in the Irp->UserBuffer
    // field irrespective of the ioctl buffer type. However, framework is very
    // strict about type checking. You cannot get Irp->UserBuffer by using
    // WdfRequestRetrieveOutputMemory if the ioctl is not a METHOD_NEITHER
    // internal ioctl. So depending on the ioctl code, we will either
    // use retreive function or escape to WDM to get the UserBuffer.
    //

    switch(IoControlCode) {

    case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
        //
        // Retrieves the device's HID descriptor.
        //
        status = vJoyGetHidDescriptor(device, Request);
        break;
    case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
        //
        //Retrieves a device's attributes in a HID_DEVICE_ATTRIBUTES structure.
        //
        status = vJoyGetDeviceAttributes(Request);
        break;

    case IOCTL_HID_GET_REPORT_DESCRIPTOR:
        //
        //Obtains the report descriptor for the HID device.
        //
        status = vJoyGetReportDescriptor(device, Request);
        break;
        

    case IOCTL_HID_READ_REPORT:

        //
        // Returns a report from the device into a class driver-supplied buffer.
        // For now queue the request to the manual queue. The request will
        // be retrived and completd when continuous reader reads new data
        // from the device.
        //
        status = WdfRequestForwardToIoQueue(Request, devContext->ReadReportMsgQueue);

        if(!NT_SUCCESS(status)){
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
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST\n");
        status = STATUS_NOT_SUPPORTED;
        break;

#endif // (OSVER(NTDDI_VERSION) > NTDDI_WIN2K)

        
    case IOCTL_HID_GET_FEATURE:
        //
        // returns a feature report associated with a top-level collection
        //
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "IOCTL_HID_GET_FEATURE\n");
        status = vJoyGetFeature(Request);
        WdfRequestComplete(Request, status);
        return;

    case IOCTL_HID_SET_FEATURE:
        //
        // This sends a HID class feature report to a top-level collection of
        // a HID class device.
        //

    case IOCTL_HID_WRITE_REPORT:
        
        // Extracting the ID from the request
        id = FfbGetDevIDfromFfbRequest(Request);

        // If FFB is not active then just reject this request
        if (!devContext->FfbEnable[id-1])
        {
            WdfRequestComplete(Request, STATUS_SUCCESS);
            return;
        };

        // If FFB is active then forward this request to the WriteQ and return
        status = WdfRequestForwardToIoQueue(Request, devContext->FfbWriteQ[id-1]);
        if(!NT_SUCCESS(status)){
            TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
                "WdfRequestForwardToIoQueue (FfbWriteQ[%d]) failed with status: 0x%x\n", id - 1, status);
            WdfRequestComplete(Request, status);
        }
        return;


    case IOCTL_HID_GET_INPUT_REPORT:
        //
        // returns a HID class input report associated with a top-level
        // collection of a HID class device.
        //
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "IOCTL_HID_GET_INPUT_REPORT\n");
        status = STATUS_NOT_SUPPORTED;
        break;

    case IOCTL_HID_SET_OUTPUT_REPORT:
        //
        // sends a HID class output report to a top-level collection of a HID
        // class device.
        //
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,   "IOCTL_HID_SET_OUTPUT_REPORT\n");
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
        status = vJoyGetHidString(Request);
        //TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,   "IOCTL_HID_GET_STRING\n");
        //status = STATUS_NOT_SUPPORTED;
        break;

    case IOCTL_HID_ACTIVATE_DEVICE:
        //
        // Makes the device ready for I/O operations.
        //
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,   "IOCTL_HID_ACTIVATE_DEVICE\n");
        status = STATUS_NOT_SUPPORTED;
        break;
    case IOCTL_HID_DEACTIVATE_DEVICE:
        //
        // Causes the device to cease operations and terminate all outstanding
        // I/O requests.
        //
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,   "IOCTL_HID_DEACTIVATE_DEVICE\n");
        status = STATUS_NOT_SUPPORTED;
        break;
        
        default:
        status = STATUS_NOT_SUPPORTED;
        break;
    }

    WdfRequestComplete(Request, status);

    return;
}


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
    int						id=0;


    // Get request parameters
    WDF_REQUEST_PARAMETERS_INIT(&params);
    WdfRequestGetParameters(Request, &params);
    if (params.Parameters.DeviceIoControl.OutputBufferLength < sizeof(HID_XFER_PACKET))
    {
        status = STATUS_BUFFER_TOO_SMALL;
        return status;
    };

    // Get transfer packet
    transferPacket = (PHID_XFER_PACKET) WdfRequestWdmGetIrp(Request)->UserBuffer;
    if (transferPacket == NULL)
    {
        status = STATUS_INVALID_DEVICE_REQUEST;
        return status;
    }

    // Get the context area
    ParentQueue = WdfRequestGetIoQueue(Request);
    device = WdfIoQueueGetDevice(ParentQueue);
    devContext = GetDeviceContext(device);

    id = FfbGetDevIDfromFfbRequest(Request);
    if (!id)
    {
        status =  STATUS_NO_SUCH_DEVICE;;
        return status;
    }


    ////////////////////////////////////////
    // Report ID 2
    // Byte[1]: Effect Block Index (1-40)
    // Byte[2]: Block Load Success (1-3)
    // Byte[3]: Block Load Full (1-3)
    // Byte[4]: Block Load Error (1-3)
    ////////////////////////////////////////
    if ((transferPacket->reportId&0x0F) == 0x02)
    {
        ucTmp = (PUCHAR)transferPacket->reportBuffer;
        ucTmp[0] = transferPacket->reportId;
        ucTmp[1] = 1; // Effect Block Index = 1
        ucTmp[3] = 0; // Load Full = 0
        if (devContext->FfbEnable[id-1])
        {
            ucTmp[2] = 1; // Load Success = 1
            ucTmp[4] = 0; // Load Error =0
        }
        else
        {
            ucTmp[2] = 0; // Load Success = 0
            ucTmp[4] = 1; // Load Error =1
        };
    };

    if ((transferPacket->reportId&0x0F) == 3 && !devContext->FfbEnable[id-1])
        status = STATUS_NO_SUCH_DEVICE;

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

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "vJoySetFeature Enter\n");

    WDF_REQUEST_PARAMETERS_INIT(&params);
    WdfRequestGetParameters(Request, &params);

    //
    // IOCTL_HID_SET_FEATURE & IOCTL_HID_GET_FEATURE are not METHOD_NIEHTER
    // IOCTLs. So you cannot retreive UserBuffer from the IRP using Wdf
    // function. As a result we have to escape out to WDM to get the UserBuffer
    // directly from the IRP. 
    //
    if (params.Parameters.DeviceIoControl.InputBufferLength < sizeof(HID_XFER_PACKET)) {
        status = STATUS_BUFFER_TOO_SMALL;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,"Userbuffer is small 0x%x\n", status);
        return status;
    }

    //
    // This is a kernel buffer so no need for try/except block when accesssing
    // Irp->UserBuffer.
    //
    transferPacket = (PHID_XFER_PACKET) WdfRequestWdmGetIrp(Request)->UserBuffer;
    if (transferPacket == NULL) {
        status = STATUS_INVALID_DEVICE_REQUEST;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "Irp->UserBuffer is NULL 0x%x\n", status);
        return status;
    }

    if (transferPacket->reportBufferLen == 0){
        status = STATUS_BUFFER_TOO_SMALL;
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "HID_XFER_PACKET->reportBufferLen is 0, 0x%x\n", status);
        return status;
    }

    if (transferPacket->reportBufferLen < sizeof(UCHAR)){
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
    
    TmpBfr = (UCHAR *)transferPacket->reportBuffer;
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

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "vJoyGetHidDescriptor Entry\n");

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
        LogEventWithStatus(ERRLOG_HID_DESC_FAILED ,L"WdfRequestRetrieveOutputMemory", WdfDeviceWdmGetDeviceObject(Device), status);
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
    
    status = WdfMemoryCopyFromBuffer(memory, 0, (PVOID) &G_DefaultHidDescriptor, bytesToCopy);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "WdfMemoryCopyFromBuffer failed 0x%x\n", status);
        LogEventWithStatus(ERRLOG_HID_DESC_FAILED ,L"WdfMemoryCopyFromBuffer", WdfDeviceWdmGetDeviceObject(Device), status);
        return status;
    }

    //
    // Report how many bytes were copied
    //
    WdfRequestSetInformation(Request, bytesToCopy);

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "vJoyGetHidDescriptor Exit = 0x%x\n", status);
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
    size_t				bytesToCopy=0;
    size_t				NumBytesTransferred=0;
    WDF_REQUEST_PARAMETERS Parameters;

    WDF_REQUEST_PARAMETERS_INIT(&Parameters);
    WdfRequestGetParameters(Request, &Parameters);

    switch ((ULONGLONG)Parameters.Parameters.DeviceIoControl.Type3InputBuffer & 0xffff)
    {
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
        LogEventWithStatus(ERRLOG_HID_STRING_FAILED ,L"WdfRequestRetrieveOutputMemory", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        return status;
    }

    bytesToCopy = pwstrID? wcslen(pwstrID)*sizeof(WCHAR) + sizeof(UNICODE_NULL): 0;
    if (bytesToCopy == 0) {
        status = STATUS_INVALID_DEVICE_STATE;
        LogEvent(ERRLOG_HID_STRING_FAILED1, NULL, WdfDriverWdmGetDriverObject(WdfGetDriver()));
        return status;        
    };

    status = WdfMemoryCopyFromBuffer(memory, 0,  pwstrID, bytesToCopy);
    WdfMemoryGetBuffer(memory, &NumBytesTransferred);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "WdfMemoryCopyFromBuffer failed 0x%x\n", status);
        LogEventWithStatus(ERRLOG_HID_STRING_FAILED ,L"WdfMemoryCopyFromBuffer", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        return status;
    }

    WdfRequestSetInformation(Request, NumBytesTransferred);

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "vJoyGetHidString Exit = 0x%x\n", status);
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
    size_t				bytesToCopy=0;
    WDFMEMORY           memory;
    size_t				NumBytesTransferred=0;
    PUCHAR				switchState = NULL;
    PDEVICE_EXTENSION   pDeviceContext = NULL;


    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "vJoyGetReportDescriptor Entry\n");

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
        LogEventWithStatus(ERRLOG_REPORT_DESC_FAILED ,L"WdfRequestRetrieveOutputMemory", WdfDeviceWdmGetDeviceObject(Device), status);
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
    status = WdfMemoryCopyFromBuffer(memory, 0,  pDeviceContext->ReportDescriptor, bytesToCopy);
    switchState = WdfMemoryGetBuffer(memory, &NumBytesTransferred);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "WdfMemoryCopyFromBuffer failed 0x%x\n", status);
        LogEventWithStatus(ERRLOG_REPORT_DESC_FAILED ,L"WdfMemoryCopyFromBuffer", WdfDeviceWdmGetDeviceObject(Device), status);
        return status;
    }

    //
    // Report how many bytes were copied
    //
    WdfRequestSetInformation(Request, bytesToCopy);

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "vJoyGetReportDescriptor Exit = 0x%x\n", status);
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

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "vJoyGetDeviceAttributes Entry\n");

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
    status = WdfRequestRetrieveOutputBuffer(Request, sizeof (HID_DEVICE_ATTRIBUTES), &deviceAttributes, NULL);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "WdfRequestRetrieveOutputBuffer failed 0x%x\n", status);
        LogEventWithStatus(ERRLOG_DEVICE_ATTR_FAILED ,L"WdfRequestRetrieveOutputBuffer", WdfDeviceWdmGetDeviceObject(WdfIoQueueGetDevice(WdfRequestGetIoQueue(Request))), status);
        return status;
    }

    deviceAttributes->VendorID		= VENDOR_N_ID;
    deviceAttributes->ProductID		= PRODUCT_N_ID;
    deviceAttributes->VersionNumber = VERSION_N;

    //
    // Report how many bytes were copied
    //
    WdfRequestSetInformation(Request, sizeof (HID_DEVICE_ATTRIBUTES));

    TraceEvents(TRACE_LEVEL_VERBOSE, DBG_IOCTL, "vJoyGetDeviceAttributes Exit = 0x%x\n", status);
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
    switch (IoControlCode)
    {
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
    NTSTATUS             status= STATUS_SUCCESS;
    //WDF_DEVICE_STATE     deviceState;
    WDFDEVICE            hDevice = WdfIoQueueGetDevice(Queue);
    PCONTROL_DEVICE_EXTENSION			 ControlDevContext = ControlGetData(hDevice);
    //ULONG  bytes;
    PDEVICE_EXTENSION    pDevContext = NULL;
    //WDFMEMORY  reqMemory;
    PVOID  buffer;
    size_t  bufSize;
    JOYSTICK_POSITION_V2 * iReport;
    //JOYSTICK_POSITION_V2   icReport;
    //PHID_DEVICE_ATTRIBUTES   deviceAttributes = NULL;
    size_t	bytesReturned = 0;




    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    KdPrint(("vJoyEvtIoDeviceControl called\n"));

    PAGED_CODE();

    switch (IoControlCode) {

    case 0x910: // Backward compatibility value of 	LOAD_POSITIONS
    case LOAD_POSITIONS:
        status = WdfRequestRetrieveInputBuffer( Request, sizeof(PJOYSTICK_POSITION_V2), &buffer, &bufSize);
        if(!NT_SUCCESS(status)) break;

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
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "Entering vJoyCreateControlDevice\n");

    //
    // First find out whether any Control Device has been created. If the
    // collection has more than one device then we know somebody has already
    // created or in the process of creating the device.
    //
    WdfWaitLockAcquire(vJoyDeviceCollectionLock, NULL);

    if(WdfCollectionGetCount(vJoyDeviceCollection) == 1) {
        bCreate = TRUE;
    }

    WdfWaitLockRelease(vJoyDeviceCollectionLock);

    if(!bCreate) {
        //
        // Control device is already created. So return success.
        //
        return STATUS_SUCCESS;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "Creating Control Device\n");

    //
    //
    // In order to create a control device, we first need to allocate a
    // WDFDEVICE_INIT structure and set all properties.
    //
        

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyCreateControlDevice: Calling WdfControlDeviceInitAllocate\n");
    pInit = WdfControlDeviceInitAllocate( WdfDeviceGetDriver(Device), &__SDDL_DEVOBJ_SYS_ALL_ADM_RWX_WORLD_RW_RES_R);

    if (pInit == NULL) {
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
    if (!NT_SUCCESS(status)) 
        goto Error;
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyCreateControlDevice: Calling WdfDeviceInitAssignName\n");
    status = WdfDeviceInitAssignName(pInit, &ntDeviceName);
    if (!NT_SUCCESS(status)) 
        goto Error;
    RtlFreeUnicodeString(&ntDeviceName);


    //
    // Specify the size of device context & create the Control Device
    //
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyCreateControlDevice: Creating Control Device\n");
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&controlAttributes, CONTROL_DEVICE_EXTENSION);
    status = WdfDeviceCreate(&pInit, &controlAttributes, &g_ControlDevice);
    if (!NT_SUCCESS(status))
        goto Error;

    //
    // Create a symbolic link for the control object so that usermode can open
    // the device.
    //
    // It has to be a UNICODE name hence the conversions
    //
    RtlInitAnsiString(&symbolicLinkNameA, TEXT(SYMBOLIC_NAME_STRING));
    status = RtlAnsiStringToUnicodeString(&symbolicLinkName, &symbolicLinkNameA, TRUE);
    if (!NT_SUCCESS(status)) 
        goto Error;
    status = WdfDeviceCreateSymbolicLink(g_ControlDevice, &symbolicLinkName);
    if (!NT_SUCCESS(status))
    {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "Failed to create symbolic link (Native)\n");
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
    if (!NT_SUCCESS(status))
        goto Error;


    ConDevContext = ControlGetData(g_ControlDevice);
    ConDevContext->hParentDevice = Device;


    //
    // Control devices must notify WDF when they are done initializing.   I/O is
    // rejected until this call is made.
    //
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoyCreateControlDevice: Calling WdfControlFinishInitializing\n");
    WdfControlFinishInitializing(g_ControlDevice);

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

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "Control Device: Start deleting\n");
    if (!g_ControlDevice)
    {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "No Control Device to delete\n");
        return;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "Control Device: Purging queue\n");
    WdfIoQueuePurge(WdfDeviceGetDefaultQueue(g_ControlDevice), WDF_NO_EVENT_CALLBACK, WDF_NO_CONTEXT);


    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "Control Device: Deleting\n");

    if (g_ControlDevice) {
        TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "Control Device: Deleting (Just before WdfObjectDelete)\n");
        WdfObjectDelete(g_ControlDevice);
        //WdfObjectDelete(Device);
        TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "Control Device: Deleting (Just after WdfObjectDelete)\n");
       g_ControlDevice = NULL;
    }
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

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "Entered FilterEvtDeviceContextCleanup\n");

    nDevices = DeviceCount(TRUE, -1); // Decrementing device count
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "Device Count before decrementing is %d\n", nDevices);

    WdfWaitLockAcquire(vJoyDeviceCollectionLock, NULL);

    nDevices = WdfCollectionGetCount(vJoyDeviceCollection);

    if(nDevices == 1)
    {
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

    WdfCollectionRemove(vJoyDeviceCollection, Device);
    WdfWaitLockRelease(vJoyDeviceCollectionLock);
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

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "PNP: Entered vJoyEvtDeviceReleaseHardware\n");
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

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "PNP: Entered vJoyEvtDevicePrepareHardware\n");
    return STATUS_SUCCESS;
}

VOID vJoyEvtDeviceSelfManagedIoFlush(
    IN  WDFDEVICE    Device
    )
    // Test Only
{
    PAGED_CODE();
    UNREFERENCED_PARAMETER(Device);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "PNP: Entered vJoyEvtDeviceSelfManagedIoFlush\n");
}

VOID vJoyEvtDevicePnpStateChange(
    IN  WDFDEVICE                          Device,
    IN  PCWDF_DEVICE_PNP_NOTIFICATION_DATA NotificationData
    )
{
    PAGED_CODE();
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(NotificationData);

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "PNP: Entered vJoyEvtDevicePnpStateChange\n");
}


VOID
LoadPositions(PJOYSTICK_POSITION_V2 pPosition, PDEVICE_EXTENSION pDevContext, size_t buffsize)
{
    int i;

    i = pPosition->bDevice-1; // Index is zero-based

    WdfWaitLockAcquire(pDevContext->positionLock, NULL);
    pDevContext->positions[i]->ValX			= pPosition->wAxisX;
    pDevContext->positions[i]->ValY			= pPosition->wAxisY;
    pDevContext->positions[i]->ValZ			= pPosition->wAxisZ;
    pDevContext->positions[i]->ValRX			= pPosition->wAxisXRot;
    pDevContext->positions[i]->ValRY			= pPosition->wAxisYRot;
    pDevContext->positions[i]->ValRZ			= pPosition->wAxisZRot;
    pDevContext->positions[i]->ValSlider		= pPosition->wSlider;
    pDevContext->positions[i]->ValDial			= pPosition->wDial;
    pDevContext->positions[i]->ValButtons		= pPosition->lButtons;
    pDevContext->positions[i]->ValHats			= pPosition->bHats;
    pDevContext->positions[i]->ValHatsEx1		= pPosition->bHatsEx1; 
    pDevContext->positions[i]->ValHatsEx2		= pPosition->bHatsEx2;
    pDevContext->positions[i]->ValHatsEx3		= pPosition->bHatsEx3;
    
    // Supporting 128 buttons: Upper buttons
    if (buffsize >= sizeof(JOYSTICK_POSITION_V2))
    {
        pDevContext->positions[i]->ValButtonsEx1		= pPosition->lButtonsEx1;
        pDevContext->positions[i]->ValButtonsEx2		= pPosition->lButtonsEx2;
        pDevContext->positions[i]->ValButtonsEx3		= pPosition->lButtonsEx3;
    };

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

--*/
PVOID GetReportDescriptorFromRegistry(USHORT * Size, USHORT * IdMask)
{

    NTSTATUS				status = STATUS_SUCCESS;
    WDFKEY					KeyDevice, KeyParameters;
    UNICODE_STRING			strDev, strDescName, strDescSize;
    ULONG					ValueLengthQueried;
	USHORT					dDescSize=0, dDescSizePrev=0;
    PVOID					out=NULL;
    HANDLE					hKeyParameters=NULL;
    int						iDevice=0;
    PKEY_BASIC_INFORMATION 	pDeviceBasicInfo=NULL;
    ULONG					ResultLength;
    ULONG					nameLength;
    WCHAR					DeviceKeyName[512];
    size_t					pcb=0;
    USHORT					id=0;

    PAGED_CODE();

    *Size = 0;
    // Get the key of the Parameters key under "SYSTEM\\CurrentControlSet\\services\\vjoy"
    status =  WdfDriverOpenParametersRegistryKey(WdfGetDriver(), WRITE_DAC, WDF_NO_OBJECT_ATTRIBUTES, &KeyParameters);
    if (!NT_SUCCESS(status))
    {
        LogEventWithStatus(ERRLOG_REP_REG_FAILED ,L"WdfDriverOpenParametersRegistryKey", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        return NULL;
    };


    // Enumerate Subkeys holding the configuration data
    status = STATUS_SUCCESS;
    while (status != STATUS_NO_MORE_ENTRIES)
    { // Loop on all sub-keys
        // Prepare buffer to receive data and get the name of the next sub-key
        // 1. Enumerate once with zero size just to get the required sizw
        // 2. Alocate the correct size to hold the output struct
        // 3. Enumerate again to get the data
        ResultLength = 0;
        hKeyParameters = WdfRegistryWdmGetHandle(KeyParameters);
        // 1.
        status = ZwEnumerateKey(hKeyParameters, iDevice, KeyBasicInformation, NULL, ResultLength, &ResultLength);
        if (status == STATUS_BUFFER_OVERFLOW || status == STATUS_BUFFER_TOO_SMALL)
        {
            // 2.
            pDeviceBasicInfo = (PKEY_BASIC_INFORMATION)ExAllocatePoolWithTag(NonPagedPool, ResultLength, 'fnIb');
            if (!pDeviceBasicInfo)
                return NULL;
            RtlZeroMemory(pDeviceBasicInfo, ResultLength);
            // 3.
            status = ZwEnumerateKey(hKeyParameters, iDevice++, KeyBasicInformation, pDeviceBasicInfo, ResultLength, &ResultLength);
        };
        if (status == STATUS_NO_MORE_ENTRIES)
            break;
        if (!NT_SUCCESS(status))
        {
            LogEventWithStatus(ERRLOG_REP_REG_FAILED ,L"ZwEnumerateKey", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
            //ExFreePoolWithTag(pDeviceBasicInfo, 'fnIb');
            return NULL;
        };

        if (!pDeviceBasicInfo)
            return NULL;

        // Copy name of subkey to unicode buffer and release temporary buffer
        nameLength = pDeviceBasicInfo->NameLength;
        RtlZeroMemory(DeviceKeyName, 512);
        status = RtlStringCbCopyNW(DeviceKeyName, 512*sizeof(WCHAR), pDeviceBasicInfo->Name, nameLength);
        if (!NT_SUCCESS(status))
        {
            LogEventWithStatus(ERRLOG_REP_REG_FAILED, L"RtlStringCbCopyNW", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
            ExFreePoolWithTag(pDeviceBasicInfo, 'fnIb');
            return NULL;
        };
        RtlInitUnicodeString(&strDev, DeviceKeyName);
        ExFreePoolWithTag(pDeviceBasicInfo, 'fnIb');

        // The sub-key name should range from "Device01" to "Device16"
        RtlStringCbLengthW(REG_DEVICE, 512, &pcb);
        if (!RtlEqualMemory(DeviceKeyName, REG_DEVICE, pcb))
            continue;

        // Get the Subkey holding the configuration data
        status =   WdfRegistryOpenKey(KeyParameters, &strDev, GENERIC_READ, WDF_NO_OBJECT_ATTRIBUTES, &KeyDevice);
        if (!NT_SUCCESS(status))
        {
            //WdfRegistryClose(KeyParameters);
            continue;
        };


        // Get the size of value 'HidReportDesctiptor' - It should return status of STATUS_BUFFER_OVERFLOW and a positive value in ValueLengthQueried
        // Then compare this value to the one in value 'HidReportDesctiptorSize'. 
        // If they are identical create a buffer and get the data in value 'HidReportDesctiptor'
        RtlInitUnicodeString(&strDescName, DESC_NAME);
        status =   WdfRegistryQueryValue(KeyDevice, &strDescName, 0, NULL, &ValueLengthQueried, NULL);
        if (status != STATUS_BUFFER_OVERFLOW)
        {
            //WdfRegistryClose(KeyParameters);
            WdfRegistryClose(KeyDevice);
            continue;
        };
        RtlInitUnicodeString(&strDescSize, DESC_SIZE);
        status =   WdfRegistryQueryValue(KeyDevice, &strDescSize, 4, &dDescSize, NULL, NULL);
        if (!NT_SUCCESS(status))
        {
            //WdfRegistryClose(KeyParameters);
            WdfRegistryClose(KeyDevice);
            continue;
        };

        if ((dDescSize != ValueLengthQueried) | !dDescSize)
        {
            //WdfRegistryClose(KeyParameters);
            WdfRegistryClose(KeyDevice);
            continue;
        };

        // The size of the descriptor was verified - Allocate memory and read it
        out = ReAllocatePoolWithTag(out, dDescSizePrev, PagedPool, dDescSizePrev+dDescSize, MEM_TAG_HIDRPRT);
        if (!out)
        {
            WdfRegistryClose(KeyParameters);
            WdfRegistryClose(KeyDevice);
            LogEvent(ERRLOG_REP_REG_FAILED1, NULL, WdfDriverWdmGetDriverObject(WdfGetDriver()));
            return NULL;
        };

        status =   WdfRegistryQueryValue(KeyDevice, &strDescName, dDescSize, (BYTE*)out+dDescSizePrev, NULL, NULL);
        if (!NT_SUCCESS(status))
        {
            WdfRegistryClose(KeyDevice);
            //ExFreePoolWithTag(out, MEM_TAG_HIDRPRT);
            continue;
        };



        //////////////////////////////////////////////////////////////////////////////
        // Get the Report ID from the buffer
        // Byte 10 should be 0x85
#if 0
        if ((*((BYTE*)out + dDescSizePrev + 10) != 0x85))
        {
            //WdfRegistryClose(KeyParameters);
            WdfRegistryClose(KeyDevice);
            //ExFreePoolWithTag(out, MEM_TAG_HIDRPRT);
            continue;
        };
        // Byte 11 should hold the ID
        id = *((BYTE*)out + dDescSizePrev + 11);

#endif // 0

        id = ParseIdInDescriptor(
            (BYTE*)out + dDescSizePrev, 
            dDescSize);

        // Check if this is a unique ID
        if ((*IdMask) & (1<<(id-1)))
        {
            WdfRegistryClose(KeyDevice);
            continue;
        };

        // Update ID mask
        *IdMask |= 1<<(id-1);
        //////////////////////////////////////////////////////////////////////////////
        
        dDescSizePrev += dDescSize;
        WdfRegistryClose(KeyDevice);

    }; // Loop on all sub-keys

    WdfRegistryClose(KeyParameters);

    *Size = dDescSizePrev;
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
    WCHAR					DeviceKeyName[512] = { 0 };
    UNICODE_STRING			strDev, strControl;
    PCWSTR					Axes[] = { L"X", L"Y", L"Z", L"RX", L"RY", L"RZ", L"SL1", L"SL2", L"POV1", L"POV2", L"POV3", L"POV4" };
    UCHAR					nAxes = 0;
    int						iAxis;
    unsigned int			Mask=0;
    const int				nButtons = 128;

    PAGED_CODE();

    // Check that buffer size is sufficient
    nAxes = sizeof(Axes) / sizeof(PCWSTR);
    if (data_buf->cb < (2 + nAxes + sizeof(nButtons) / 8))
    {
        LogEventWithStatus(ERRLOG_REP_REG_FAILED, L"GetInitValueFromRegistry: Buffer size too small", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        return 0;
    };
    data_buf->id = id;

    // Get the key of the Parameters key under "SYSTEM\\CurrentControlSet\\services\\vjoy"
    status = WdfDriverOpenParametersRegistryKey(WdfGetDriver(), WRITE_DAC, WDF_NO_OBJECT_ATTRIBUTES, &KeyParameters);
    if (!NT_SUCCESS(status))
    {
        LogEventWithStatus(ERRLOG_REP_REG_FAILED, L"WdfDriverOpenParametersRegistryKey", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        return 0;
    };
    status = STATUS_SUCCESS;

    // Calculate the string of the registry key
    status = RtlStringCchPrintfExW(DeviceKeyName, sizeof(DeviceKeyName) / sizeof(WCHAR), NULL, NULL, STRSAFE_NO_TRUNCATION, REG_DEVICE L"%02d\\" REG_INIT, id);
    if (!NT_SUCCESS(status))
    {
        LogEventWithStatus(ERRLOG_REP_REG_FAILED, L"RtlStringCchPrintfExW", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        return 0;
    };
    RtlUnicodeStringInit(&strDev, DeviceKeyName);


    // Open a handle to the key
    status = WdfRegistryOpenKey(KeyParameters, &strDev, GENERIC_READ, WDF_NO_OBJECT_ATTRIBUTES, &KeyDevice);
    if (!NT_SUCCESS(status))
    {
        LogEventWithStatus(ERRLOG_REP_REG_FAILED, L"WdfRegistryOpenKey", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        WdfRegistryClose(KeyParameters);
        return 0;
    };

    // Analyze axes	& POVs
    for (iAxis = 0; iAxis < nAxes; iAxis++)
    {
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
    unsigned int mask_device=0, mask_master=0;
    int i,j, k;
    UCHAR InitValAxis[8] = { 50, 50, 50, 0, 0, 0, 0, 0 };
    UCHAR InitValPov[4] = { (UCHAR)-1, (UCHAR)-1, (UCHAR)-1, (UCHAR)-1 };
    UCHAR ButtonMask[16] = { 0 };
    int nAxes, nPovs, offset;

    PAGED_CODE();

    // Prepare buffer and get init data for device from registry
    data_buf->cb = sizeof(DEVICE_INIT_VALS);
    data_buf->id = id;	

    // First, get initialization values for device.
    // Special case: default (hardcoded device) - skip this stage.
    if (id)
    {
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
    for (i = 0; i <nAxes ; i++)
    {
        offset = nAxes + nPovs- i;
        if (!(mask_device & (1 << offset)))
        {
            if (mask_master & (1 << offset))
                data_buf->InitValAxis[i] = init_master.InitValAxis[i];
            else
                data_buf->InitValAxis[i] = InitValAxis[i];
        };
    };

    // Merge POVs
    for (j = 0; j < nPovs; i++, j++)
    {
        offset = nAxes + nPovs - i;
        if (!(mask_device & (1 << offset)))
        {
            if (mask_master & (1 << offset))
                data_buf->InitValPov[j] = init_master.InitValPov[j];
            else
                data_buf->InitValPov[j] = InitValPov[j];
        };
    };

    // Buttons
    if (!(mask_device & 1))
    {
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
    USHORT	Size = 0, Mask=0;
    PVOID	out;

    // Get the size from the registry
    out = GetReportDescriptorFromRegistry(&Size, &Mask);

    if (Size)
    {
        ExFreePoolWithTag(out, MEM_TAG_HIDRPRT);
        G_DefaultHidDescriptor.DescriptorList[0].wReportLength = Size;
    }
    else
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
    USHORT	Size = 0, Mask=0;
    PVOID	out;
    PDEVICE_EXTENSION   devContext = NULL;

    // Initialize
    devContext = GetDeviceContext(Device);
    CleanUpDev(devContext);

    // Get the data, size and IDs from the registry
    out = GetReportDescriptorFromRegistry(&Size, &Mask);
    if (out && Mask && Size)
    {	/* Registry holds valid data */
        InitializeDev(devContext, Mask, FALSE);
        devContext->isHardcodedDevice = FALSE;
        // Free HID Report Descriptor
        if (devContext->ReportDescriptor)
        {
            ExFreePoolWithTag(devContext->ReportDescriptor, MEM_TAG_HIDRPRT);
            devContext->ReportDescriptor = NULL;
        };
        devContext->ReportDescriptor = out;
        G_DefaultHidDescriptor.DescriptorList[0].wReportLength = Size;
    }
    else
    {	/* Use default (hard-coded) data */
        InitializeDefaultDev(devContext);
        devContext->isHardcodedDevice = TRUE;
    };

}

/*
    Initialize data structures in the device context area
*/
void InitializeDeviceContext(PDEVICE_EXTENSION   devContext)
{
    int i/*, j*/;

    // Init array of pointers to reports
    for (i=0; i<MAX_N_DEVICES; i++)
        devContext->positions[i] = NULL;

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
    UCHAR * cTag;
    ULONG	Tag = '00DI';
    const UCHAR	SerialL[16] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6'};
    const UCHAR	SerialH[16] = {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', '1', '1', '1'};
    DEVICE_INIT_VALS data_buf;

    cTag = (UCHAR *)&Tag;
    n = devContext->nDevices = N_COLLECTIONS;

    if (devContext->positionLock)
        WdfWaitLockAcquire(devContext->positionLock, NULL);
    // Create a DEVICE_POSITION structure for each hard-coded top-level collection
    for (index=0; index<n; index++)
    {

        // Mark default device(s) as implemented
        devContext->DeviceImplemented[index] = TRUE;
        
        //id = index+1;
        cTag[3] = SerialL[index];
        cTag[2] = SerialH[index];
        devContext->positions[index]  = ExAllocatePoolWithTag(NonPagedPool, sizeof(DEVICE_POSITION_V2),Tag);
        if (!devContext->positions[index])
            break;

        // Get initialization values (From device #0)
        CalcInitValue(0, &data_buf);

        // Initialize all fields
        devContext->positions[index]->ValThrottle = 0;
        devContext->positions[index]->ValRudder = 0;
        devContext->positions[index]->ValAileron = 0;
        devContext->positions[index]->ValX = data_buf.InitValAxis[0] * 0x7FFF / 100 + 1;//0x7FFF/2+1;
        devContext->positions[index]->ValY = data_buf.InitValAxis[1] * 0x7FFF / 100 + 1;//0x7FFF/2+1;
        devContext->positions[index]->ValZ = data_buf.InitValAxis[2] * 0x7FFF / 100 + 1;//0x7FFF/2+1;
        devContext->positions[index]->ValRX = data_buf.InitValAxis[3] * 0x7FFF / 100 + 1;
        devContext->positions[index]->ValRY = data_buf.InitValAxis[4] * 0x7FFF / 100 + 1;
        devContext->positions[index]->ValRZ = data_buf.InitValAxis[5] * 0x7FFF / 100 + 1;
        devContext->positions[index]->ValSlider = data_buf.InitValAxis[6] * 0x7FFF / 100 + 1;
        devContext->positions[index]->ValDial = data_buf.InitValAxis[7] * 0x7FFF / 100 + 1;
        devContext->positions[index]->ValWheel = 0;

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

        devContext->positions[index]->ValButtons = ((DWORD *)(data_buf.ButtonMask))[0];
        devContext->positions[index]->ValButtonsEx1 = ((DWORD *)(data_buf.ButtonMask))[1];
        devContext->positions[index]->ValButtonsEx2 = ((DWORD *)(data_buf.ButtonMask))[2];
        devContext->positions[index]->ValButtonsEx3 = ((DWORD *)(data_buf.ButtonMask))[3];
    };

    if (devContext->positionLock)
        WdfWaitLockRelease(devContext->positionLock);

    // Copy HID Report Descriptor
    devContext->ReportDescriptor = ExAllocatePoolWithTag(PagedPool, sizeof(G_DefaultReportDescriptor),MEM_TAG_HIDRPRT);
	if (!devContext->ReportDescriptor)
		return;
    RtlCopyMemory(devContext->ReportDescriptor, G_DefaultReportDescriptor, sizeof(G_DefaultReportDescriptor));

    // Init HID Descriptor
    G_DefaultHidDescriptor.DescriptorList[0].wReportLength = sizeof(G_DefaultReportDescriptor);

}

/*
    Reset values of data structures representing virtual devices
*/
void ResetDeviceControls(int id, PDEVICE_EXTENSION devContext, PDEVICE_INIT_VALS pdata_buf)
{
    int index = id - 1;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "ResetDeviceControls: Resetting device:%d [requested]\n", id);

    // Sanity check
    if (!devContext || id < 1 || id > MAX_N_DEVICES || !devContext->positions[index] || !pdata_buf)
        return;

    // Device exists?
    if (!devContext->DeviceImplemented[index])
        return;

    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "ResetDeviceControls: Resetting device:%d [granted]\n", id);

    // Initialize all fields
    devContext->positions[index]->ValThrottle = 0;
    devContext->positions[index]->ValRudder = 0;
    devContext->positions[index]->ValAileron = 0;
    devContext->positions[index]->ValX = pdata_buf->InitValAxis[0] * 0x7FFF / 100 + 1;//0x7FFF/2+1;
    devContext->positions[index]->ValY = pdata_buf->InitValAxis[1] * 0x7FFF / 100 + 1;//0x7FFF/2+1;
    devContext->positions[index]->ValZ = pdata_buf->InitValAxis[2] * 0x7FFF / 100 + 1;//0x7FFF/2+1;
    devContext->positions[index]->ValRX = pdata_buf->InitValAxis[3] * 0x7FFF / 100 + 1;
    devContext->positions[index]->ValRY = pdata_buf->InitValAxis[4] * 0x7FFF / 100 + 1;
    devContext->positions[index]->ValRZ = pdata_buf->InitValAxis[5] * 0x7FFF / 100 + 1;
    devContext->positions[index]->ValSlider = pdata_buf->InitValAxis[6] * 0x7FFF / 100 + 1;
    devContext->positions[index]->ValDial = pdata_buf->InitValAxis[7] * 0x7FFF / 100 + 1;
    devContext->positions[index]->ValWheel = 0;

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

    devContext->positions[index]->ValButtons = ((DWORD *)(pdata_buf->ButtonMask))[0];
    devContext->positions[index]->ValButtonsEx1 = ((DWORD *)(pdata_buf->ButtonMask))[1];
    devContext->positions[index]->ValButtonsEx2 = ((DWORD *)(pdata_buf->ButtonMask))[2];
    devContext->positions[index]->ValButtonsEx3 = ((DWORD *)(pdata_buf->ButtonMask))[3];
}

/*
    Initialize data structures representing virtual devices
*/
void InitializeDev(PDEVICE_EXTENSION   devContext, USHORT Mask, BOOLEAN ResetOnly)
{
    int index;
    UCHAR * cTag;
    ULONG	Tag = '00DI';
    const UCHAR	SerialL[16] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6'};
    const UCHAR	SerialH[16] = {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', '1', '1', '1'};
    DEVICE_INIT_VALS data_buf;

    cTag = (UCHAR *)&Tag;
    devContext->nDevices = 0;

    if (devContext->positionLock)
        WdfWaitLockAcquire(devContext->positionLock, NULL);
    // Create a DEVICE_POSITION structure for each hard-coded top-level collection
    for (index=0; index<MAX_N_DEVICES; index++)
    {
        if (!((Mask >> index) & 1))
        {
            if (!ResetOnly)
                devContext->DeviceImplemented[index] = FALSE;
            continue;
        };

        // Initially - no file objects associated with the device
        devContext->DeviceFileObject[index] = NULL;

        devContext->nDevices++;
        devContext->DeviceImplemented[index] = TRUE;

        cTag[3] = SerialL[index];
        cTag[2] = SerialH[index];
        devContext->positions[index]  = ExAllocatePoolWithTag(NonPagedPool, sizeof(DEVICE_POSITION_V2),Tag);
        if (!devContext->positions[index])
            break;

        // Get initialization values
        CalcInitValue((USHORT)index + 1, &data_buf);

        // Initialize all fields
        devContext->positions[index]->ValThrottle = 0;
        devContext->positions[index]->ValRudder = 0;
        devContext->positions[index]->ValAileron = 0;
        devContext->positions[index]->ValX = data_buf.InitValAxis[0] * 0x7FFF / 100 +1;//0x7FFF/2+1;
        devContext->positions[index]->ValY = data_buf.InitValAxis[1] * 0x7FFF / 100 + 1;//0x7FFF/2+1;
        devContext->positions[index]->ValZ = data_buf.InitValAxis[2] * 0x7FFF / 100 + 1;//0x7FFF/2+1;
        devContext->positions[index]->ValRX = data_buf.InitValAxis[3] * 0x7FFF / 100 + 1;
        devContext->positions[index]->ValRY = data_buf.InitValAxis[4] * 0x7FFF / 100 + 1;
        devContext->positions[index]->ValRZ = data_buf.InitValAxis[5] * 0x7FFF / 100 + 1;
        devContext->positions[index]->ValSlider = data_buf.InitValAxis[6] * 0x7FFF / 100 + 1;
        devContext->positions[index]->ValDial = data_buf.InitValAxis[7] * 0x7FFF / 100 + 1;
        devContext->positions[index]->ValWheel = 0;

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

        devContext->positions[index]->ValButtons = ((DWORD *)(data_buf.ButtonMask))[0];
        devContext->positions[index]->ValButtonsEx1 = ((DWORD *)(data_buf.ButtonMask))[1];
        devContext->positions[index]->ValButtonsEx2 = ((DWORD *)(data_buf.ButtonMask))[2];
        devContext->positions[index]->ValButtonsEx3 = ((DWORD *)(data_buf.ButtonMask))[3];
    }
    if (devContext->positionLock)
        WdfWaitLockRelease(devContext->positionLock);

}



/*
    Remove data structures representing virtual devices
*/
void CleanUpDev(PDEVICE_EXTENSION   devContext)
{
    int index;
    UCHAR * cTag;
    ULONG	Tag = '00DI';
    const UCHAR	SerialL[16] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6'};
    const UCHAR	SerialH[16] = {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', '1', '1', '1'};
    LONGLONG timeout = 0;

    cTag = (UCHAR *)&Tag;


    // Destroy and init DEVICE_POSITION structure for each hard-coded top-level collection
    if (STATUS_SUCCESS == WdfWaitLockAcquire(devContext->positionLock, &timeout))
    {
        for (index = 0; index<MAX_N_DEVICES; index++)
        {
            if (devContext->positions[index])
            {
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
    if (devContext->ReportDescriptor)
    {
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

    if (!orig || !prevSize)
        return ExAllocatePoolWithTag(PoolType, NumberOfBytes, Tag);

    tmpPool = ExAllocatePoolWithTag(PoolType, prevSize, tmpTag);
    if (!tmpPool)
        return NULL;

    RtlZeroMemory(tmpPool, prevSize);
    RtlCopyMemory(tmpPool, orig, prevSize);

    ExFreePoolWithTag(orig, Tag);
    orig=NULL;

#pragma warning (push)
#pragma warning( disable:28160 )
    out = ExAllocatePoolWithTag(PoolType, NumberOfBytes, Tag);
#pragma warning (pop)
    if (!out)
        return NULL;
    RtlZeroMemory(out, prevSize);
    RtlCopyMemory(out, tmpPool, prevSize);
    ExFreePoolWithTag(tmpPool, tmpTag);

    return out;
}

USHORT ParseIdInDescriptor(BYTE * desc, DWORD dDescSize)
// Parse descriptor (read from the registry)
// Look for 0x85 in the descriptor
// then read the next byte - this is the id
// Test the ID - should be in the range 1 to MAX_N_DEVICES
// Return valid ID - else return 0
{
    DWORD i;
    USHORT id = 0;

    for (i = 0; i < dDescSize-1; i++)
    {
        if (*desc == 0x85)
        { 
            id = *(desc + 1);
            break;
        }
        desc++;
    }
    if (id < 1 || id>MAX_N_DEVICES)
        id = 0;

    return id;
}

VOID FfbNotifyWrite(
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
    WdfIoQueueGetState(devContext->FfbReadQ[id-1],&QueueRequests, NULL);
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
    WdfIoQueueGetState(devContext->FfbReadQ[id-1],&QueueRequests, NULL);
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
    BOOLEAN					DataExist=FALSE;
    NTSTATUS				status = STATUS_SUCCESS;
    WDFREQUEST				WriteRequest, ReadRequest;
    PHID_XFER_PACKET		transferPacket = NULL;
    ULONG					DataSize = 0;
    ULONG					Cmd;
    WDF_REQUEST_PARAMETERS	Params;
    unsigned int			bytesToCopy;
    size_t					bytesReturned;
    PUCHAR					ReadBuffer = NULL;
    BOOLEAN					ReadQueueReady=FALSE, WriteQueueReady=FALSE;
    ULONG					QueueRequests;


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

		// Calculate the size of the Read data packet
		DataSize += sizeof(ULONG);						// Size of packet
		DataSize += sizeof(ULONG);						// Cmd
		DataSize += transferPacket->reportBufferLen;	// Raw data (First byte is report ID)

		// Get the Read request
		status = WdfIoQueueRetrieveNextRequest(devContext->FfbReadQ[id - 1], &ReadRequest);
		if (!NT_SUCCESS(status))
		{
			WdfRequestComplete(WriteRequest, status);
			return;
		};

		// Calculate size of Read data
		WDF_REQUEST_PARAMETERS_INIT(&Params);
		WdfRequestGetParameters(ReadRequest, &Params);
		bytesToCopy = (unsigned int)Params.Parameters.DeviceIoControl.OutputBufferLength;
		if (bytesToCopy < DataSize)
		{
			status = STATUS_BUFFER_TOO_SMALL;
			WdfRequestComplete(WriteRequest, status);
			WdfRequestComplete(ReadRequest, status);
			return;
		};

		// Get Read request output buffer
		status = WdfRequestRetrieveOutputBuffer(ReadRequest, bytesToCopy, (PVOID)&ReadBuffer, &bytesReturned);
		if (!NT_SUCCESS(status))
		{
			WdfRequestComplete(WriteRequest, status);
			WdfRequestComplete(ReadRequest, status);
			return;
		};

		// Copy data to Read request output buffer
		if (bytesReturned >= 4)
		{
			memcpy(&(ReadBuffer[0]), &DataSize, sizeof(ULONG));
			memcpy(&(ReadBuffer[sizeof(ULONG)]), &Cmd, sizeof(ULONG));
			memcpy(&(ReadBuffer[2 * sizeof(ULONG)]), transferPacket->reportBuffer, transferPacket->reportBufferLen);
		};

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
    transferPacket = (PHID_XFER_PACKET) WdfRequestWdmGetIrp(Request)->UserBuffer;

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