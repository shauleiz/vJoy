;/*++ BUILD Version: 0001    // Increment this if a change has global effects
;
;   Copyright (c) Microsoft Corporation
;
;   Module Name:
;       errlog.mc
;
;   Abstract:
;       Constant definitions for the log error code values.
;
;   Revision History:
;--*/
;
;#ifndef _ERRLOGMC_
;#define _ERRLOGMC_
;
;//
;//  Status values are 32 bit values layed out as follows:
;//
;//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
;//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
;//  +---+-+-------------------------+-------------------------------+
;//  |Sev|C|       Facility          |               Code            |
;//  +---+-+-------------------------+-------------------------------+
;//
;//  where
;//
;//      Sev - is the severity code
;//
;//          00 - Success
;//          01 - Informational
;//          10 - Warning
;//          11 - Error
;//
;//      C - is the Customer code flag
;//
;//      Facility - is the facility code
;//
;//      Code - is the facility's status code
;//
;
MessageIdTypedef=NTSTATUS

SeverityNames=(Success=0x0:STATUS_SEVERITY_SUCCESS
               Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
               Warning=0x2:STATUS_SEVERITY_WARNING
               Error=0x3:STATUS_SEVERITY_ERROR
              )

FacilityNames=(System=0x0
               RpcRuntime=0x2:FACILITY_RPC_RUNTIME
               RpcStubs=0x3:FACILITY_RPC_STUBS
               Io=0x4:FACILITY_IO_ERROR_CODE
               ErrLog=0x5:FACILITY_DEVICE_ERROR_CODE
              )

MessageId=0x0001 Facility=ErrLog Severity=Error SymbolicName=ERRLOG_NOT_SUPPORTED
Language=English
The request is not supported.
.

MessageId=0x0002 Facility=ErrLog Severity=Error SymbolicName=ERRLOG_INSUFFICIENT_RESOURCES
Language=English
Failed allocating requested system resources.
.

MessageId=0x0003 Facility=ErrLog Severity=Error SymbolicName=ERRLOG_DEVICE_REMOVED
Language=English
The device has been removed.
.

MessageId=0x0004 Facility=ErrLog Severity=Error SymbolicName=ERRLOG_DEVICE_NOT_STARTED
Language=English
The device has not been started.
.

MessageId=0x0005 Facility=ErrLog Severity=Error SymbolicName=ERRLOG_BUFFER_TOO_SMALL
Language=English
The buffer is too small for the requested data.
.

MessageId=0x0006 Facility=ErrLog Severity=Error SymbolicName=ERRLOG_INVALID_BUFFER_SIZE
Language=English
The size of the buffer is invalid for the specified operation.
.

MessageId=0x0007 Facility=ErrLog Severity=Error SymbolicName=ERRLOG_INVALID_PARAMETER
Language=English
An invalid parameter was passed to a function.
.

MessageId=0x0008 Facility=ErrLog Severity=Error SymbolicName=ERRLOG_LOWERDRV_IRP_FAILED
Language=English
Lower driver has failed the request.
.

MessageId=0x0009 Facility=ErrLog Severity=Error SymbolicName=ERRLOG_OPENREG_FAILED
Language=English
Failed opening registry key.
.

MessageId=0x000a Facility=ErrLog Severity=Error SymbolicName=ERRLOG_READREG_FAILED
Language=English
Failed reading registry value.
.

MessageId=0x000b Facility=ErrLog Severity=Error SymbolicName=ERRLOG_WRITEREG_FAILED
Language=English
Failed writing registry value.
.

MessageId=0x000c Facility=ErrLog Severity=Error SymbolicName=ERRLOG_REGTYPE_MISMATCH
Language=English
Registry data type mismatch.
.

MessageId=0x000d Facility=ErrLog Severity=Error SymbolicName=ERRLOG_MINIDRV_REG_FAILED
Language=English
This minidriver failed to register with HID class driver.
.


MessageId=0x0200 Facility=ErrLog Severity=Informational SymbolicName=ERRLOG_TEST_INFORMATION
Language=English
The driver has reported the following test information: %2.
.

MessageId=0x0201 Facility=ErrLog Severity=Informational SymbolicName=ERRLOG_TEST_INFORMATION2
Language=English
The driver has reported the following test information: %2. Status code = %3.
.

MessageId=0x0202 Facility=ErrLog Severity=Error SymbolicName=ERRLOG_DRIVER_FAILED
Language=English
Failed to install driver. Function %2 failed with status %3.
.

MessageId=0x0203 Facility=ErrLog Severity=Error SymbolicName=ERRLOG_DRIVER_FAILED1
Language=English
Failed to install driver. DeviceCount initialization failed.
.

MessageId=0x0204 Facility=ErrLog Severity=Informational SymbolicName=ERRLOG_DRIVER_INSTALLED
Language=English
Driver Installed.
.

MessageId=0x0205 Facility=ErrLog Severity=Error SymbolicName=ERRLOG_DEVICE_FAILED
Language=English
Failed to install device. Function %2 failed with status %3.
.

MessageId=0x0206 Facility=ErrLog Severity=Informational SymbolicName=ERRLOG_DEVICE_ADDED
Language=English
Device Added.
.

MessageId=0x0207 Facility=ErrLog Severity=Error SymbolicName=ERRLOG_HID_DESC_FAILED
Language=English
Failed to get HID Descriptor. Function %2 failed with status %3.
.

MessageId=0x0208 Facility=ErrLog Severity=Error SymbolicName=ERRLOG_HID_DESC_FAILED1
Language=English
Failed to get HID Descriptor. G_DefaultHidDescriptor is zero.
.

MessageId=0x0209 Facility=ErrLog Severity=Error SymbolicName=ERRLOG_REPORT_DESC_FAILED
Language=English
Failed to get Report Descriptor. Function %2 failed with status %3.
.

MessageId=0x020A Facility=ErrLog Severity=Error SymbolicName=ERRLOG_REPORT_DESC_FAILED1
Language=English
Failed to get Report Descriptor. G_DefaultHidDescriptor is zero.
.

MessageId=0x020B Facility=ErrLog Severity=Error SymbolicName=ERRLOG_DEVICE_ATTR_FAILED
Language=English
Failed to get Device Attributes. Function %2 failed with status %3.
.

MessageId=0x020C Facility=ErrLog Severity=Error SymbolicName=ERRLOG_REP_REG_FAILED
Language=English
Failed to get Report Descriptor from Registry. Function %2 failed with status %3.
.

MessageId=0x020D Facility=ErrLog Severity=Error SymbolicName=ERRLOG_REP_REG_FAILED1
Language=English
Failed to get Report Descriptor from Registry. ReAllocatePoolWithTag failed.
.

MessageId=0x020E Facility=ErrLog Severity=Informational SymbolicName=ERRLOG_DRIVER_REMOVED
Language=English
Driver Removed.
.

MessageId=0x020F Facility=ErrLog Severity=Error SymbolicName=ERRLOG_RAW_DEV_FAILED
Language=English
Failed to create raw device. Function %2 failed with status %3.
.

MessageId=0x0210 Facility=ErrLog Severity=Error SymbolicName=ERRLOG_HID_STRING_FAILED
Language=English
Failed to get HID string. Function %2 failed with status %3.
.

MessageId=0x0211 Facility=ErrLog Severity=Error SymbolicName=ERRLOG_HID_STRING_FAILED1
Language=English
Failed to get HID string. String is missing.
.

;#ifdef DEBUG

MessageId=0x8000 Facility=ErrLog Severity=Informational SymbolicName=ERRLOG_DEBUG_INFORMATION
Language=English
The driver has reported the following debug information: %2.
.

MessageId=0x8001 Facility=ErrLog Severity=Warning SymbolicName=ERRLOG_DEBUG_WARNING
Language=English
The driver has reported the following debug warning: %2.
.

MessageId=0x8002 Facility=ErrLog Severity=Error SymbolicName=ERRLOG_DEBUG_ERROR
Language=English
The driver has reported the following debug error: %2.
.

;#endif

;#endif /* _ERRLOGMC_ */

