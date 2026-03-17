/* SPDX-License-Identifier: MIT */

#include <boot2target.h>

EFI_HANDLE          gImageHandle;
EFI_SYSTEM_TABLE    *gST;
EFI_BOOT_SERVICES   *gBS;

EFI_GUID gAppleSmcIoProtocolGuid = APPLE_SMC_IO_PROTOCOL_GUID;

#define SMC_KEY_MVHR SMC_MAKE_KEY('M', 'V', 'H', 'R')
#define SMC_KEY_MVMR SMC_MAKE_KEY('M', 'V', 'M', 'R')

// FIXME: This will pin the CPU, we should use timers or something
void Sleep(UINT32 Seconds)
{
    gBS->Stall(Seconds * 1000000);
}

EFI_STATUS EFIAPI EfiMain(
        IN EFI_HANDLE ImageHandle,
        IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS              Status;
    APPLE_SMC_IO_PROTOCOL   *SmcIo = NULL;
    SMC_DATA                Value;

    gST = SystemTable;
    gBS = SystemTable->BootServices;
    gImageHandle = ImageHandle;

    /* Disable watchdog timer */
    gBS->SetWatchdogTimer(0, 0, 0, NULL);

    /* Find Apple SMC I/O protocol */
    Status = gBS->LocateProtocol(
            &gAppleSmcIoProtocolGuid,
            NULL,
            (VOID **) &SmcIo
            );

    if (EFI_ERROR(Status))
    {
        gST->ConOut->OutputString(gST->ConOut, L"Cannot find SMC I/O protocol!\r\n");
        while (1);
    }

    /*
     * Enable TDM: 1
     * Disable TDM: 0
     */
    Value = 1;

    Status = SmcIo->SmcWriteValue(
            SmcIo,
            SMC_KEY_MVHR,
            sizeof(Value),
            &Value
            );

    if (EFI_ERROR(Status))
    {
        gST->ConOut->OutputString(gST->ConOut, L"Cannot write first value to SMC!\r\n");
        while (1);
    }

    Sleep(1);

    /*
     * Reset screen?
     */
    Value = 2;

    Status = SmcIo->SmcWriteValue(
            SmcIo,
            SMC_KEY_MVMR,
            sizeof(Value),
            &Value
            );

    if (EFI_ERROR(Status))
    {
        gST->ConOut->OutputString(gST->ConOut, L"Cannot write second value to SMC!\r\n");
        while (1);
    }

    gST->ConOut->OutputString(gST->ConOut, L"Ideally this should be invisible.\r\n");

    /* FIXME: Use timers */
    while (1);
}
