/*--

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.


Module Name:

    RawPdo.c

Abstract: This module have the code enumerate a raw PDO for every device
          the filter attaches to so that it can provide a direct
          sideband communication with the usermode application.

          The toaster filter driver sample demonstrates an alternation
          approach where you can create one control-device for all the
          instances of the filter device.

Environment:

    Kernel mode only.

--*/
	  
#include <vjoy.h>
#include "rawpdo.h"
#include <Ntddk.h>

#if defined(EVENT_TRACING)
#include "rawpdo.tmh"
#endif

#ifdef ALLOC_PRAGMA
    #pragma alloc_text( PAGE, rawEvtCleanupCallback)
    #pragma alloc_text( PAGE, vJoy_EvtDeviceFileCreate)
    #pragma alloc_text( PAGE, vJoy_EvtFileCleanup)
#endif

VOID
vJoy_EvtIoDeviceControlForRawPdo(
    IN WDFQUEUE      Queue,
    IN WDFREQUEST    Request,
    IN size_t        OutputBufferLength,
    IN size_t        InputBufferLength,
    IN ULONG         IoControlCode
    )
/*++

Routine Description:

    This routine is the dispatch routine for device control requests.

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
    NTSTATUS status = STATUS_SUCCESS;
    //WDFDEVICE parent;
    //WDF_REQUEST_FORWARD_OPTIONS forwardOptions;
    PDEVICE_EXTENSION    pDevContext = NULL;
	//WDFMEMORY  inMemory, outMemory;
    PVOID  buffer;
    size_t  bufSize;
    WDFDEVICE            hRawDevice = WdfIoQueueGetDevice(Queue);
	PRPDO_DEVICE_DATA pdoData = PdoGetData(hRawDevice);
	//WDF_REQUEST_SEND_OPTIONS RequestOptions;
	//WDFIOTARGET TargetOnParent;

	JOYSTICK_POSITION_V2 * iReport;
	WDFFILEOBJECT FileObj;
	USHORT id=0;
	PFILEOBJECT_EXTENSION pExtension=NULL;

    //WDFREQUEST				requestForceFeedback;
	//PHID_XFER_PACKET		transferPacket = NULL;
	//PVOID					ForceFeedbackBuffer = NULL;
	PVOID					GenBuffer = NULL;
	size_t					bytesReturned = 0;
	WDF_REQUEST_PARAMETERS	Params;
	//BOOLEAN					FfbStat = FALSE;
	ULONG					bytesToCopy = 0;
	BYTE					Byte_tmp;




    
    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

	TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "Entered vJoy_EvtIoDeviceControlForRawPdo\n");

    //
    // Process the ioctl and complete it when you are done.
    // Since the queue is configured for serial dispatch, you will
    // not receive another ioctl request until you complete this one.
    //

	  switch (IoControlCode) {
 	case 1234: // TODO: Remove for production
		//RequestOptions.Flags =	WDF_REQUEST_SEND_OPTION_TIMEOUT;
		//RequestOptions.Size =	sizeof(WDF_REQUEST_SEND_OPTIONS);
		//WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(&RequestOptions,  WDF_REL_TIMEOUT_IN_SEC(1));
		//status = WdfIoTargetSendInternalIoctlSynchronously(pdoData->IoTargetToParent, Request, IoControlCode , NULL, NULL, &RequestOptions, NULL);
		break; // Testing 

	case GET_DEV_STAT:
		// Get information for a device by device ID
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: Case GET_DEV_STAT\n");

		// Get the buffer from the request
		// Get the data from the request
		WDF_REQUEST_PARAMETERS_INIT(&Params);
		WdfRequestGetParameters(Request, &Params);
		bytesToCopy = (ULONG)Params.Parameters.DeviceIoControl.OutputBufferLength;
		if (bytesToCopy<5)
		{
			TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo[GET_DEV_STAT]: failed - bytesToCopy=%d\n", bytesToCopy);
			WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
			return;
		};

		// Get the buffer
		status = WdfRequestRetrieveOutputBuffer(Request, bytesToCopy, &GenBuffer, &bytesReturned);
		if (bytesReturned<5)
		{
			TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo[GET_DEV_STAT]: failed - bytesReturned=%d\n", bytesReturned);
			WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
			return;
		};

		// Get the id number from input buffer
		status = WdfRequestRetrieveInputBuffer(Request, sizeof(BYTE), &buffer, &bufSize);
		if (!NT_SUCCESS(status) || (bufSize!=1))
		{
			TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo[GET_DEV_STAT]: failed to retrieve input buffer\n");
			WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
			return;
		};

		// Get target ID
		id = *(BYTE *)buffer;
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo[GET_DEV_STAT]: id=%d\n", id);

		// Put data into output buffer
		// Byte 1:
		// Bit 0: Implemented?
		// Bit 1: FFB Device Enabled?
		// Bit 2: File Object associated with this device?
		pDevContext = GetDeviceContext(pdoData->hParentDevice);
		Byte_tmp = 0;

		// Implemented mask
		if (pDevContext->DeviceImplemented[id - 1])
			Byte_tmp |= 1;
		else
			Byte_tmp &= 0xFE;
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo[GET_DEV_STAT]: Dev Implemented=%x\n", pDevContext->DeviceImplemented[id - 1]);

		// FFB mask
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo[GET_DEV_STAT]: Dev Enabled=%x\n", pDevContext->FfbEnable[id - 1]);
		if (pDevContext->FfbEnable[id - 1])
			Byte_tmp |= 2;
		else
			Byte_tmp &= 0xFD;

		// File Object
		if (pDevContext->DeviceFileObject[id - 1])
			Byte_tmp |= 4;
		else
			Byte_tmp &= 0xFB;

		((BYTE *)GenBuffer)[0] = Byte_tmp;
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo[GET_DEV_STAT]: Buffer[0]=%x\n", ((BYTE *)GenBuffer)[0]);

		// Byte2-5: Process ID
		// Get the context
		FileObj = pDevContext->DeviceFileObject[id - 1];
		if (FileObj)
			*(DWORD *)(&((BYTE *)GenBuffer)[1]) = GetFileObjectContext(FileObj)->CallingProcessId;
		else
			*(DWORD *)(&((BYTE *)GenBuffer)[1]) = 0;
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo[GET_DEV_STAT]: ProcessID=%x\n", *(DWORD *)(&((BYTE *)GenBuffer)[1]));

		// Complete the transaction
		WdfRequestCompleteWithInformation(Request, status, bytesReturned);
		return;



	case GET_DRV_INFO:
		// Get information for this driver
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: [GET_DRV_INFO]\n");
		// Get the buffer from the request
		// Get the data from the request
		WDF_REQUEST_PARAMETERS_INIT(&Params);
		WdfRequestGetParameters(Request, &Params);

		// Number of bytes to copy must be at least one byte
		bytesToCopy = (ULONG)Params.Parameters.DeviceIoControl.OutputBufferLength;
		if (bytesToCopy <1 )
		{
			TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: [GET_DRV_INFO] - bytesToCopy <1\n");
			WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
			return;
		};


		// Output buffer must be at least one byte
		status = WdfRequestRetrieveOutputBuffer(Request, bytesToCopy, &GenBuffer, &bytesReturned);
		if (bytesReturned<1)
		{
			TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: [GET_DRV_INFO] - bytesReturned <1\n");
			WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
			return;
		};

		pDevContext = GetDeviceContext(pdoData->hParentDevice);

		// Return the data in the output buffer
		// BYTE 0 //////////////////////////////
		// Bit 0	: Supports FFB?
		// Bit 1	: Reserved
		// Bit 2	: Mode: Multi-device 
		// Bit 3	: Mode: FFB 
		// BYTE 1 //////////////////////////////
		// Bits 0-7	: Maximum number of possible devices (16 ==> 255)  not regarding to mode
		// BYTE 2 //////////////////////////////
		// Bits 0-7	: Number of existing devices
		// BYTE 3 //////////////////////////////
		// Bits 0-7	: Number of devices that can still be implemented (This is the number of possible devices for the current mode minus the number of already existing devices).

		//////////////////////////////////
		// Byte 0
		Byte_tmp = 0;
		Byte_tmp |= 0x01; // FFB Supported
		Byte_tmp |= 0x00; // Default Mode (TODO: Change to real mode when Implemented) Multi-Device=0x04; FFB=0x80
		((BYTE *)GenBuffer)[0] = Byte_tmp;

		// Byte 1
		if (bytesToCopy >= 2 && bytesReturned >= 2)
		{
			((BYTE *)GenBuffer)[1] = MAX_N_DEVICES;
			TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: [GET_DRV_INFO] - Data byte[1]=%02x\n", ((BYTE *)GenBuffer)[1]);
		}

		// Byte 2
		if (bytesToCopy >= 3 && bytesReturned >= 3)
		{
			((BYTE *)GenBuffer)[2] = (BYTE)(pDevContext->nDevices);
			TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: [GET_DRV_INFO] - Data byte[2]=%02x\n", ((BYTE *)GenBuffer)[2]);
		}

		// Byte 3 - TODO: Change according to mode
		if (bytesToCopy >= 4 && bytesReturned >= 4)
		{
			((BYTE *)GenBuffer)[3] = MAX_N_DEVICES - (BYTE)(pDevContext->nDevices);
			TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: [GET_DRV_INFO] - Data byte[3]=%02x\n", ((BYTE *)GenBuffer)[3]);
		}
		//////////////////////////////////

		// Complete the transaction
		WdfRequestCompleteWithInformation(Request, status, bytesReturned);
		return;


	case GET_DEV_INFO:
		// Get information for this device (and for the driver)

		// Get the buffer from the request
		// Get the data from the request
		WDF_REQUEST_PARAMETERS_INIT(&Params);
		WdfRequestGetParameters(Request, &Params);
		bytesToCopy = (ULONG)Params.Parameters.DeviceIoControl.OutputBufferLength;
		if (bytesToCopy<6)
		{
			WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
			return;
		};

		status = WdfRequestRetrieveOutputBuffer(Request, bytesToCopy, &GenBuffer, &bytesReturned);
		if (bytesReturned<6)
		{
			WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
			return;
		};

		// Get the context, id and the status
		id = GetIdFromRawPdoRequest(Request, pExtension);

		// Illegal ID
		if (id == 0xFFFF)
		{
			WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
			return;
		};

		// Copy the state to the buffer
		pDevContext = GetDeviceContext(pdoData->hParentDevice);
		((BYTE *)GenBuffer)[0] = (BYTE)id;
		((BYTE *)GenBuffer)[1] = (BYTE)(pDevContext->nDevices);
		((BYTE *)GenBuffer)[2] = pDevContext->DeviceImplemented ? 1:0;
		((BYTE *)GenBuffer)[3] = MAX_N_DEVICES; 
 		((BYTE *)GenBuffer)[4] = 1; // Driver does support FFB
		((BYTE *)GenBuffer)[5] = pDevContext->FfbEnable[id - 1]; // Device support FFB

		// Complete the transaction
		WdfRequestCompleteWithInformation(Request, status, bytesReturned);
		return;

	case 0x910: // Backward compatibility value of 	LOAD_POSITIONS
	case LOAD_POSITIONS:
		// KdBreakPoint(); Break When loading position
		status = WdfRequestRetrieveInputBuffer( Request, sizeof(JOYSTICK_POSITION), &buffer, &bufSize);
		if(!NT_SUCCESS(status))
			break;

		// Get interface that this IRP came from,
		// then get the implicated id of the top-level collection
		id = GetIdFromRawPdoRequest(Request, pExtension);


		// Illegal ID
		if (id==0xFFFF)
		{
			WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
			return;
		};

		// Get the incoming report and compare the id in the report
		// to the implicated id of the top-level collection
		// They should match
		iReport = buffer;
		if (iReport->bDevice != id)
		{
			WdfRequestComplete(Request, STATUS_CANCELLED);
			return;
		};

		pDevContext = GetDeviceContext(pdoData->hParentDevice);
		LoadPositions(iReport, pDevContext, bufSize);
		status = vJoyCompleteReadReport(pdoData->hParentDevice, (BYTE)id);
		break;   

	case GET_FFB_STAT:
		/* Get the status of the FFB mechanism */

		// Get the buffer from the request
		// Get the data from the request
		WDF_REQUEST_PARAMETERS_INIT(&Params);
		WdfRequestGetParameters(Request, &Params);
		bytesToCopy = (ULONG)Params.Parameters.DeviceIoControl.OutputBufferLength;
		status = WdfRequestRetrieveOutputBuffer(Request, bytesToCopy, &GenBuffer, &bytesReturned);
		if (!bytesReturned)
		{
			WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
			return;
		};

		// Get the context, id and the status
		id = GetIdFromRawPdoRequest(Request, pExtension);


		// Illegal ID
		if (id == 0xFFFF)
		{
			WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
			return;
		};

		// Copy the state to the buffer
		pDevContext = GetDeviceContext(pdoData->hParentDevice);
		((BYTE *)GenBuffer)[0] =  pDevContext->FfbEnable[id-1];

		// Complete the transaction
		WdfRequestCompleteWithInformation(Request, status, bytesReturned);
		return;

	case SET_FFB_STAT:
		/*Set the status of the FFB mechanism - Obsolete*/

		// Get the data from the request
		WDF_REQUEST_PARAMETERS_INIT(&Params);
		WdfRequestGetParameters(Request, &Params);

		// Get interface that this IRP came from,
		// then get the implicated id of the top-level collection
		// Get the context, id and the status
		id = GetIdFromRawPdoRequest(Request, pExtension);
		// Illegal ID
		if (id == 0xFFFF)
		{
			WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
			return;
		};


		// Get the data, process the data and complete the transaction
		//pDevContext = GetDeviceContext(pdoData->hParentDevice);	   - Obsolete
		//FfbActiveSet(*(BOOLEAN *)Params.Parameters.DeviceIoControl.Type3InputBuffer, id, pDevContext);   - Obsolete
		WdfRequestComplete(Request, status);
		return;

	case GET_FFB_DATA:
		// Get interface that this IRP came from,
		// then get the implicated id of the top-level collection
		id = GetIdFromRawPdoRequest(Request, pExtension);


		// If FFB is not active then just reject this request
		pDevContext = GetDeviceContext(pdoData->hParentDevice);
		if (id == 0xFFFF || !pDevContext->FfbEnable[id - 1])
		{
			WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
			return;
		};

		// If FFB is active then forward this request to the ReadQ and return
		status = WdfRequestForwardToIoQueue(Request, pDevContext->FfbReadQ[id - 1]);
        if(!NT_SUCCESS(status)){
            TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL,
				"WdfRequestForwardToIoQueue (FfbWriteQ[%d]) failed with status: 0x%x\n", id - 1, status);
            WdfRequestComplete(Request, status);
        }
        return;

	case GET_DRV_DEV_EN:
		// Get the number of devices that are currently enabled
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: Case GET_DRV_DEV_EN\n");

		// Get the buffer from the request
		// Get the data from the request
		WDF_REQUEST_PARAMETERS_INIT(&Params);
		WdfRequestGetParameters(Request, &Params);
		bytesToCopy = (ULONG)Params.Parameters.DeviceIoControl.OutputBufferLength;
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: bytesToCopy=%d\n", bytesToCopy);
		if (bytesToCopy<1)
		{
			WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
			return;
		};

		status = WdfRequestRetrieveOutputBuffer(Request, bytesToCopy, &GenBuffer, &bytesReturned);
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: bytesReturned=%d\n", bytesReturned);
		if (bytesReturned<1)
		{
			WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
			return;
		};

		if (!pdoData) break;
		pDevContext = GetDeviceContext(pdoData->hParentDevice);
		if (!pDevContext) break;
		// The number of the max supported devices
		((BYTE *)GenBuffer)[0] = (BYTE)(pDevContext->nDevices);

		// Complete the transaction
		WdfRequestCompleteWithInformation(Request, status, bytesReturned);
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: GenBuffer[0]=%d\n", ((BYTE *)GenBuffer)[0]);
		return;

	case GET_DRV_DEV_MAX:
		// Get the max possible number of devices that this driver supports
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: Case GET_DRV_DEV_MAX\n");

		// Get the buffer from the request
		// Get the data from the request
		WDF_REQUEST_PARAMETERS_INIT(&Params);
		WdfRequestGetParameters(Request, &Params);
		bytesToCopy = (ULONG)Params.Parameters.DeviceIoControl.OutputBufferLength;
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: bytesToCopy=%d\n", bytesToCopy);
		if (bytesToCopy<1)
		{
			WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
			return;
		};

		status = WdfRequestRetrieveOutputBuffer(Request, bytesToCopy, &GenBuffer, &bytesReturned);
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: bytesReturned=%d\n", bytesReturned);
		if (bytesReturned<1)
		{
			WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
			return;
		};

		// The number of the max supported devices
		((BYTE *)GenBuffer)[0] = MAX_N_DEVICES;

		// Complete the transaction
		WdfRequestCompleteWithInformation(Request, status, bytesReturned);
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: GenBuffer[0]=%d\n", ((BYTE *)GenBuffer)[0]);
		return;


	case IS_DRV_FFB_CAP:
		// Test is this version of vJoy driver supports FFB
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: Case IS_DRV_FFB_CAP\n");

		// Get the data from the request (Bytes to copy)
		WDF_REQUEST_PARAMETERS_INIT(&Params);
		WdfRequestGetParameters(Request, &Params);
		bytesToCopy = (ULONG)Params.Parameters.DeviceIoControl.OutputBufferLength;
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: bytesToCopy=%d\n", bytesToCopy);
		if (bytesToCopy<1)
		{
			TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: bytesToCopy=%d (Failed)\n", bytesToCopy);
			WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
			return;
		};

		// Get the buffer from the request
		status = WdfRequestRetrieveOutputBuffer(Request, bytesToCopy, &GenBuffer, &bytesReturned);
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: bytesReturned=%d\n", bytesReturned);
		if (bytesReturned<1)
		{
			TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: bytesReturned=%d (Failed)\n", bytesReturned);
			WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
			return;
		};

		// Yes, this driver supports FFB
		((BYTE *)GenBuffer)[0] = 1;

		// Complete the transaction
		WdfRequestCompleteWithInformation(Request, status, bytesReturned);
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: GenBuffer[0]=%d\n", ((BYTE *)GenBuffer)[0]);
		return;
	case RESET_DEV:
		/* Resets device(s) to predefined values */
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: Case RESET_DEV\n");

		// then get the implicated id of the top-level collection
		id = GetIdFromRawPdoRequest(Request, pExtension);
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtIoDeviceControlForRawPdo: ID=%d\n", id);

		// Sanity check
		if (id == 0xFFFF)
		{
			WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
			return;
		};

		// Get the context of vJoy device
		pDevContext = GetDeviceContext(pdoData->hParentDevice);		
		
		// Reset device(s)
		status = ResetDevice(id, pDevContext);
		WdfRequestCompleteWithInformation(Request, status, 0);
		return;

	default:
        break;
    }
	  WdfRequestComplete(Request, status);

    return;
}

#define MAX_ID_LEN 128

NTSTATUS
vJoy_CreateRawPdo(
    WDFDEVICE       Device,
    ULONG           InstanceNo
    )
/*++

Routine Description:

    This routine creates and initialize a PDO.

Arguments:

Return Value:

    NT Status code.

--*/
{   
    NTSTATUS                    status;
    PWDFDEVICE_INIT             pDeviceInit = NULL;
    PRPDO_DEVICE_DATA           pdoData = NULL;
    WDFDEVICE                   RawPdoDevice = NULL;
    WDF_OBJECT_ATTRIBUTES       pdoAttributes;
    WDF_DEVICE_PNP_CAPABILITIES pnpCaps;
    WDF_IO_QUEUE_CONFIG         ioQueueConfig;
    WDFQUEUE                    queue;
    WDF_DEVICE_STATE            deviceState;
    PDEVICE_EXTENSION			devExt;
    DECLARE_CONST_UNICODE_STRING(deviceId,VJOY_RAW_DEVICE_ID );
    //DECLARE_CONST_UNICODE_STRING(hardwareId,VJOY_HARDWARE_ID );
    DECLARE_CONST_UNICODE_STRING(deviceLocation,L"vJoy Raw Device\0" );
    DECLARE_UNICODE_STRING_SIZE(buffer, MAX_ID_LEN);
	PDEVICE_OBJECT				RawPdoDeviceObject;
	PDEVICE_OBJECT				ParentDeviceObject;
	//DECLARE_CONST_UNICODE_STRING(
	//	SDDL_DEVOBJ_SYS_ALL_ADM_RWX_WORLD_R_RES_R,
	//	L"D:P(A;;GA;;;SY)(A;;GRGWGX;;;BA)(A;;GR;;;WD)(A;;GR;;;RC)"
	//	);
	DECLARE_CONST_UNICODE_STRING(
		SDDL_DEVOBJ_SYS_ALL_ADM_RWX_WORLD_RWX_RES_R,
		L"D:P(A;;GA;;;SY)(A;;GRGWGX;;;BA)(A;;GRGWGX;;;WD)(A;;GR;;;RC)"
		);

	int iInterface;
	WCHAR RefStr[20];
	UNICODE_STRING RefStr2;
	WDF_FILEOBJECT_CONFIG FileObjInit;
	WDF_OBJECT_ATTRIBUTES       FileObjAttributes;


	TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "Entered vJoy_CreateRawPdo\n");


    //
    // Allocate a WDFDEVICE_INIT structure and set the properties
    // so that we can create a device object for the child.
    //
    pDeviceInit = WdfPdoInitAllocate(Device);

    if (pDeviceInit == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
		LogEventWithStatus(ERRLOG_RAW_DEV_FAILED ,L"WdfPdoInitAllocate", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        goto Cleanup;
    }


    //
    // Mark the device RAW so that the child device can be started
    // and accessed without requiring a function driver. Since we are
    // creating a RAW PDO, we must provide a class guid.
    //
    status = WdfPdoInitAssignRawDevice(pDeviceInit, &GUID_DEVINTERFACE_VJOY);
    if (!NT_SUCCESS(status)) {
		LogEventWithStatus(ERRLOG_RAW_DEV_FAILED ,L"WdfPdoInitAssignRawDevice", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        goto Cleanup;
    }

	// TODO: Assign correct SDDL
    ////
    status = WdfDeviceInitAssignSDDLString(pDeviceInit,
                                           &SDDL_DEVOBJ_SYS_ALL_ADM_RWX_WORLD_RWX_RES_R);
    if (!NT_SUCCESS(status)) {
		LogEventWithStatus(ERRLOG_RAW_DEV_FAILED ,L"WdfDeviceInitAssignSDDLString", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        goto Cleanup;
    }


    //
    // Assign DeviceID - This will be reported to IRP_MN_QUERY_ID/BusQueryDeviceID
    //
    status = WdfPdoInitAssignDeviceID(pDeviceInit, &deviceId);
    if (!NT_SUCCESS(status)) {
		LogEventWithStatus(ERRLOG_RAW_DEV_FAILED ,L"WdfPdoInitAssignDeviceID", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        goto Cleanup;
    }


    //
    // We could be enumerating more than one children if the filter attaches
    // to multiple instances of keyboard, so we must provide a
    // BusQueryInstanceID. If we don't, system will throw CA bugcheck.
    //
    status =  RtlUnicodeStringPrintf(&buffer, VJOY_DEVICE_INSTANCE, InstanceNo);
    if (!NT_SUCCESS(status)) {
 		LogEventWithStatus(ERRLOG_RAW_DEV_FAILED ,L"RtlUnicodeStringPrintf (1)", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
       goto Cleanup;
    }

    status = WdfPdoInitAssignInstanceID(pDeviceInit, &buffer);
    if (!NT_SUCCESS(status)) {
 		LogEventWithStatus(ERRLOG_RAW_DEV_FAILED ,L"WdfPdoInitAssignInstanceID", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        goto Cleanup;
    }

    //
    // Provide a description about the device. This text is usually read from
    // the device. This text is displayed momentarily by the PnP manager while
    // it's looking for a matching INF. If it finds one, it uses the Device
    // Description from the INF file to display in the device manager.
    // Since our device is raw device and we don't provide any hardware ID
    // to match with an INF, this text will be displayed in the device manager.
    //
    status = RtlUnicodeStringPrintf(&buffer,VJOY_DEVICE_TEXT_409 ); // English - United States
    if (!NT_SUCCESS(status)) {
 		LogEventWithStatus(ERRLOG_RAW_DEV_FAILED ,L"RtlUnicodeStringPrintf (2)", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        goto Cleanup;
    }

    //
    // You can call WdfPdoInitAddDeviceText multiple times, adding device
    // text for multiple locales. When the system displays the text, it
    // chooses the text that matches the current locale, if available.
    // Otherwise it will use the string for the default locale.
    // The driver can specify the driver's default locale by calling
    // WdfPdoInitSetDefaultLocale.
    //
    status = WdfPdoInitAddDeviceText(pDeviceInit,
                                        &buffer,
                                        &deviceLocation,
                                        0x409 // English - United States
                                        );
    if (!NT_SUCCESS(status)) {
 		LogEventWithStatus(ERRLOG_RAW_DEV_FAILED ,L"WdfPdoInitAddDeviceText (1)", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        goto Cleanup;
    }

#if 0
	// Hebrew (No real ned - just for fun)
	status = RtlUnicodeStringPrintf(&buffer,VJOY_DEVICE_TEXT_40D, InstanceNo ); // Hebrew
	if (!NT_SUCCESS(status)) {
		LogEventWithStatus(ERRLOG_RAW_DEV_FAILED ,L"RtlUnicodeStringPrintf (3)", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
		goto Cleanup;
	}

	status = WdfPdoInitAddDeviceText(pDeviceInit,
		&buffer,
		&deviceLocation,
		0x40D // Hebrew
		);
	if (!NT_SUCCESS(status)) {
		LogEventWithStatus(ERRLOG_RAW_DEV_FAILED ,L"WdfPdoInitAddDeviceText (2)", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
		goto Cleanup;
	}

#endif // 0


    WdfPdoInitSetDefaultLocale(pDeviceInit, 0x409); // English - United States

	WdfDeviceInitSetExclusive(pDeviceInit, FALSE);
	
	// Create a WDFFILEOBJECT
	WDF_OBJECT_ATTRIBUTES_INIT(&FileObjAttributes);
	WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&FileObjAttributes, FILEOBJECT_EXTENSION);
	WDF_FILEOBJECT_CONFIG_INIT(&FileObjInit,  vJoy_EvtDeviceFileCreate, WDF_NO_EVENT_CALLBACK, vJoy_EvtFileCleanup);
	WdfDeviceInitSetFileObjectConfig(pDeviceInit, &FileObjInit, &FileObjAttributes);

    
    //
    // Initialize the attributes to specify the size of PDO device extension.
    // All the state information private to the PDO will be tracked here.
    //
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&pdoAttributes, RPDO_DEVICE_DATA);
	pdoAttributes.EvtCleanupCallback = rawEvtCleanupCallback;

    //
    // Set up our queue to allow forwarding of requests to the parent
    // This is done so that the cached data can be retrieved
    //
    //WdfPdoInitAllowForwardingRequestToParent(pDeviceInit);
	// TODO: Replace the above because it is needed for WdfRequestForwardToParentDeviceIoQueue()

    status = WdfDeviceCreate(&pDeviceInit, &pdoAttributes, &RawPdoDevice);
    if (!NT_SUCCESS(status)) {
  		LogEventWithStatus(ERRLOG_RAW_DEV_FAILED ,L"WdfDeviceCreate", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        goto Cleanup;
    }

    //
    // Get the device context.
    //
    pdoData = PdoGetData(RawPdoDevice);
	pdoData->InstanceNo = InstanceNo;
	pdoData->hParentDevice = Device;

    //
    // Save the I/O target handle and adjust the I/O stack size:
    //
    devExt = GetDeviceContext(Device);
    pdoData->IoTargetToParent = devExt->IoTargetToSelf;
	RawPdoDeviceObject = WdfDeviceWdmGetDeviceObject(RawPdoDevice);
	ParentDeviceObject = WdfDeviceWdmGetDeviceObject(Device);
	RawPdoDeviceObject->StackSize = ParentDeviceObject->StackSize+1;

    //
    // Configure the default queue associated with the control device object
    // to be Serial so that request passed to EvtIoDeviceControl are serialized.
    // A default queue gets all the requests that are not
    // configure-fowarded using WdfDeviceConfigureRequestDispatching.
    //

    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig,
                                    WdfIoQueueDispatchSequential);

    ioQueueConfig.EvtIoDeviceControl = vJoy_EvtIoDeviceControlForRawPdo;

    status = WdfIoQueueCreate(RawPdoDevice,
                                        &ioQueueConfig,
                                        WDF_NO_OBJECT_ATTRIBUTES,
                                        &queue // pointer to default queue
                                        );
    if (!NT_SUCCESS(status)) {
		TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "WdfIoQueueCreate failed 0x%x\n", status);
   		LogEventWithStatus(ERRLOG_RAW_DEV_FAILED ,L"WdfIoQueueCreate", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
       goto Cleanup;
    }

    //
    // Set some properties for the child device.
    //
    WDF_DEVICE_PNP_CAPABILITIES_INIT(&pnpCaps);

    pnpCaps.Removable         = WdfTrue; // Remove Icon from " Devices and Printers"
    pnpCaps.SurpriseRemovalOK = WdfTrue;
    pnpCaps.NoDisplayInUI     = WdfTrue;

    // pnpCaps.Address  = InstanceNo;
    pnpCaps.UINumber = 0;

    WdfDeviceSetPnpCapabilities(RawPdoDevice, &pnpCaps);

    //
    // TODO: In addition to setting NoDisplayInUI in DeviceCaps, we
    // have to do the following to hide the device. Following call
    // tells the framework to report the device state in
    // IRP_MN_QUERY_DEVICE_STATE request.
    //
    WDF_DEVICE_STATE_INIT(&deviceState);
    deviceState.DontDisplayInUI = WdfTrue; // Remove Icon from Device manager
    WdfDeviceSetDeviceState(RawPdoDevice, &deviceState);

	//
	// Create 16 interfaces
	//
	for (iInterface=1 ; iInterface <= MAX_N_DEVICES; iInterface++)
	{
		RtlStringCchPrintfW((NTSTRSAFE_PWSTR)RefStr, 20, VJOY_INTERFACE L"%03d", iInterface);
		RtlInitUnicodeString(&RefStr2, (PCWSTR)RefStr);
		status = WdfDeviceCreateDeviceInterface(RawPdoDevice,&GUID_DEVINTERFACE_VJOY,&RefStr2);

		if (!NT_SUCCESS (status)) {
			TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "WdfDeviceCreateDeviceInterface number %d failed 0x%x\n", iInterface, status);
   			LogEventWithStatus(ERRLOG_RAW_DEV_FAILED ,L"WdfDeviceCreateDeviceInterface", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
			goto Cleanup;
		}
	};

	// Mark all interfaces as unused
	pdoData->UsedInterfacesMask=0;


	//
	// Add this device to the FDO's collection of children.
    // After the child device is added to the static collection successfully,
    // driver must call WdfPdoMarkMissing to get the device deleted. It
    // shouldn't delete the child device directly by calling WdfObjectDelete.
    //
    status = WdfFdoAddStaticChild(Device, RawPdoDevice);
    if (!NT_SUCCESS(status)) {
   		LogEventWithStatus(ERRLOG_RAW_DEV_FAILED ,L"WdfFdoAddStaticChild", WdfDriverWdmGetDriverObject(WdfGetDriver()), status);
        goto Cleanup;
    }

    return STATUS_SUCCESS;

Cleanup:

    TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "KbFiltr_CreatePdo failed %x\n", status);
    //
    // Call WdfDeviceInitFree if you encounter an error while initializing
    // a new framework device object. If you call WdfDeviceInitFree,
    // do not call WdfDeviceCreate.
    //
    if (pDeviceInit != NULL) {
        WdfDeviceInitFree(pDeviceInit);
    }

    if(RawPdoDevice) {
        WdfObjectDelete(RawPdoDevice);
    }

    return status;
}


VOID
rawEvtCleanupCallback(
    IN WDFDEVICE Device
    )
{
	UNREFERENCED_PARAMETER(Device);
	PAGED_CODE();
	//NTSTATUS status;

	//status = WdfPdoMarkMissing(Device);
}

// handles operations that must be performed when an application requests access to a device.
VOID vJoy_EvtDeviceFileCreate(
  __in  WDFDEVICE Device,
  __in  WDFREQUEST Request,
  __in  WDFFILEOBJECT FileObject
)
{
		WDFFILEOBJECT FileObj;
		PCUNICODE_STRING pName;
		UNICODE_STRING TmpUstr;
		NTSTATUS status = STATUS_SUCCESS;
		ULONG  id=0;
		WCHAR RefStr[100];
		PFILEOBJECT_EXTENSION pExtension=NULL;
		PDEVICE_EXTENSION    pDevContext = NULL;
		PRPDO_DEVICE_DATA	pPdoData=NULL;
		size_t len;
		DWORD_PTR ProcessId;

	    PAGED_CODE ();
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "Entered vJoy_EvtDeviceFileCreate\n");


		// Get file object then get its filename
		FileObj = WdfRequestGetFileObject(Request);
		if (!FileObj)
			goto going_out;
		pName = WdfFileObjectGetFileName(FileObj);
		if (!pName)
			goto going_out;

		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtDeviceFileCreate: File name=%wZ\n", pName);

		// Extract id from interface number
		status = RtlStringCchLengthW(VJOY_INTERFACE, 100, &len);
		if (!NT_SUCCESS(status))
			goto going_out;
		status = RtlStringCchCopyNW(RefStr, 100, pName->Buffer+len+1,4);	// Copy the numeric part of the string (000)
		if (!NT_SUCCESS(status))
			goto going_out;
		RtlInitUnicodeString(&TmpUstr, RefStr);								// Convert "000" to UNICODE_STRING
		status = RtlUnicodeStringToInteger(&TmpUstr, 10, &id);				// Convert "000" to integer (0)
		if (!NT_SUCCESS(status))
			goto going_out;

		if (id>0)
		{
			// Verify that this interface has a corresponding device
			TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtDeviceFileCreate: ID=%d\n", id);
			pPdoData = PdoGetData(Device);
			pDevContext = GetDeviceContext(pPdoData->hParentDevice);
			if (!pDevContext)
				goto going_out;
			if (!pDevContext->positions[id - 1])
				goto going_out;

			// Get the file object context space
			// Test that this interface is not in use
			// and store there the parent (Raw PDO) context
			pExtension = GetFileObjectContext(FileObject);
			if (!pExtension)
				goto going_out;
			pExtension->pParentRawDeviceContext = pPdoData;
			if (pPdoData->UsedInterfacesMask & (1 << (id - 1)))
			{
				WdfRequestComplete(Request, STATUS_ACCESS_DENIED);
				ProcessId = (DWORD_PTR)PsGetCurrentProcessId();
				TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtDeviceFileCreate: PID=%d Failed (Access Denied)\n", ProcessId);
				return;
			}

			///// This is a successful file creation - Now record the file details

			// vJoy Device ID
			else
				pPdoData->UsedInterfacesMask |= 1 << (id - 1);

			// Put id in file object context space 
			pExtension->id = id; // Update

			// Get the id of the calling process
			ProcessId = (DWORD_PTR)PsGetCurrentProcessId();
			pExtension->CallingProcessId = (DWORD)(ProcessId & 0xFFFFFFFF);
			TraceEvents(TRACE_LEVEL_WARNING, DBG_INIT, "vJoy_EvtDeviceFileCreate: PID=%d\n", pExtension->CallingProcessId);

			// Put the file object in the FDO extension
			pDevContext->DeviceFileObject[id - 1] = FileObject;

			// Activate FFB Queues
			FfbActiveSet(TRUE, id, pDevContext);


			WdfRequestComplete(Request, status);
			return;

		} // if (id>0)

		// Case of General purpose and non device-specific Interface
		else // if (id<1)
		{
			TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtDeviceFileCreate(2nd case): ID=%d\n", id);
#if 0
			pPdoData = PdoGetData(Device);
			pDevContext = GetDeviceContext(pPdoData->hParentDevice);
			if (!pDevContext)
				goto going_out;

			TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtDeviceFileCreate(2nd case): Completing Request\n");

#endif // 0
			WdfRequestComplete(Request, status);
			return;
		}; // if (id<1)

	going_out:
		ProcessId = (DWORD_PTR)PsGetCurrentProcessId();
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtDeviceFileCreate: PID=%d Failed (Invalid Handle)\n", ProcessId);
		WdfRequestComplete(Request, STATUS_INVALID_HANDLE);
}

VOID vJoy_EvtFileCleanup(
  __in  WDFFILEOBJECT FileObject
)
{
		PFILEOBJECT_EXTENSION	pExtension=NULL;
		PRPDO_DEVICE_DATA		pParentRawDeviceContext=NULL;
		PDEVICE_EXTENSION		pDevContext = NULL;

		PAGED_CODE()

		pExtension = GetFileObjectContext(FileObject);
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "vJoy_EvtFileCleanup: DevID=%d PID=%d\n", pExtension->id, pExtension->CallingProcessId);

		// Remove id in file object context space
		if (pExtension && pExtension->id)
			pExtension->pParentRawDeviceContext->UsedInterfacesMask &= ~(1<< (pExtension->id-1));

		// Reset the ID of the calling process
		if (pExtension)
			pExtension->CallingProcessId = 0;

		// Get the parent device context
		if (pExtension)
		{
			pParentRawDeviceContext = pExtension->pParentRawDeviceContext;
			if (!pParentRawDeviceContext)
				return;
		};
		
		if (pParentRawDeviceContext) {
			pDevContext = GetDeviceContext(pParentRawDeviceContext->hParentDevice);
			if (!pDevContext)
				return;
		};
		

		if (pDevContext && pExtension && pExtension->id)
		{
			// set FFB to false
			FfbActiveSet(FALSE, pExtension->id, pDevContext);

			// Remove record of this file object
			pDevContext->DeviceFileObject[pExtension->id - 1] = NULL;  
		};
}

VOID FfbActiveSet(
	BOOLEAN active,
	int id,
	PDEVICE_EXTENSION    pDevContext
	)
{

	int szarry, szelement;
	int sz;

	// Check id
	szarry    = sizeof(pDevContext->FfbEnable);
	szelement = sizeof(pDevContext->FfbEnable[0]);
	sz = szarry/szelement;
	if (id<1 || id>sz)
		return;


	// Has there been a change? if not do nothing
	// NO
	if (active == pDevContext->FfbEnable[id-1])
		return;

	// YES
	if (active)
	{ // FFB Activated
		////////////  ReadQ /////////////////////////////////////////
		// Start queue
		WdfIoQueueStart(pDevContext->FfbReadQ[id - 1]);

		////////////  WriteQ /////////////////////////////////////////
		// Start queue
		WdfIoQueueStart(pDevContext->FfbWriteQ[id - 1]);

		// Mark FFB as active
		pDevContext->FfbEnable[id-1] = TRUE;
	}
	else
	{ // FFB Deactivated
		pDevContext->FfbEnable[id-1] = FALSE;

		// Purge queues
		WdfIoQueuePurge(pDevContext->FfbWriteQ[id - 1], NULL, NULL);
		WdfIoQueuePurge(pDevContext->FfbReadQ[id - 1], NULL, NULL);
	};

}


BOOLEAN FfbActiveGet(
	int    			   	 id,
	PDEVICE_EXTENSION    pDevContext
	)
{
	int szarry, szelement;
	int sz;

	// Check id
	szarry = sizeof(pDevContext->FfbEnable);
	szelement = sizeof(pDevContext->FfbEnable[0]);
	sz = szarry / szelement;
	if (id<1 || id>sz)
		return FALSE;

	return pDevContext->FfbEnable[id - 1];
}


// Exstact the device id of the current device from the request
// Return id
// Returns 0xFFFF if error or id out of range
USHORT GetIdFromRawPdoRequest(
	WDFREQUEST    Request,
	PVOID pExt
	)
{
	WDFFILEOBJECT FileObj = NULL;
	PFILEOBJECT_EXTENSION pExtension = (PFILEOBJECT_EXTENSION)pExt;
	USHORT id = 0xFFFF;

	// Get the context, id and the status
	FileObj = WdfRequestGetFileObject(Request);
	if (FileObj)
	{
		pExtension = GetFileObjectContext(FileObj);
		if (pExtension)
			id = (UCHAR)pExtension->id;
		//else
		//	id = 0;
	}
	//else
	//	id = 0;

	//if (id > MAX_N_DEVICES)
	//	id = 0;

	return id;
}

/*
	If id>0 then resets the values of the device's controls
	If id=0 then resets the values of the controls of all existing devices
*/
NTSTATUS  ResetDevice(USHORT id, PDEVICE_EXTENSION pDevContext)
{
	DEVICE_INIT_VALS data_buf;
	USHORT index;

	// Sanity check
	TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "ResetDevice: DevID=%d\n", id);
	if ((id > MAX_N_DEVICES) || (id < 0))
	{
		TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "ResetDevice: failed - id=%d out of range\n", id);
		return 	STATUS_NO_SUCH_DEVICE;
	};

	// Test the Device context
	if (!pDevContext)
	{
		TraceEvents(TRACE_LEVEL_ERROR, DBG_INIT, "ResetDevice: failed - pDevContext is NULL\n");
		return STATUS_UNSUCCESSFUL;
	};

	// If the device is the hard-coded device then call InitializeDefaultDev(devContext)
	// If this is a user configured se of devices call InitializeDev with a mask
	if (pDevContext->isHardcodedDevice)
	{ 
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "ResetDevice: Initializing hard-coded device\n");
		CalcInitValue(0, &data_buf);
		if (id)
			ResetDeviceControls(id, pDevContext, &data_buf);
		else
			for (index = 0; index<MAX_N_DEVICES; index++)
				ResetDeviceControls(index+1, pDevContext, &data_buf);
	}
	else
	{
		TraceEvents(TRACE_LEVEL_INFORMATION, DBG_INIT, "ResetDevice: Initializing device:%d\n", id);
		if (id)
		{
			CalcInitValue(id, &data_buf);
			ResetDeviceControls(id, pDevContext, &data_buf);
		}
		else
			for (index = 0; index < MAX_N_DEVICES; index++)
			{
				CalcInitValue(index + 1, &data_buf);
				ResetDeviceControls(index + 1, pDevContext, &data_buf);
			}
	};

	return 	STATUS_SUCCESS;
}
