/* SPDX-License-Identifier: MIT */

#include <boot2target.h>

EFI_HANDLE          gImageHandle;
EFI_SYSTEM_TABLE    *gST;
EFI_BOOT_SERVICES   *gBS;

EFI_STATUS EFIAPI EfiMain(
        IN EFI_HANDLE ImageHandle,
        IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS              Status;
    APPLE_SMC_IO_PROTOCOL   *SmcIo = NULL;
    BOOLEAN                 TargetDisplayEnabled = FALSE;

    gST = SystemTable;
    gBS = SystemTable->BootServices;
    gImageHandle = ImageHandle;

    /* Disable watchdog timer so we don't auto-reboot after 5 minutes */
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
        return Status;
    }

    /* FIXME: Use timers */
    while (1)
    {
        if (TdmIsCableConnected(SmcIo))
        {
            if (!TargetDisplayEnabled)
            {
                Status = TdmToggle(SmcIo, TRUE);
                if (EFI_ERROR(Status))
                {
                    gST->ConOut->OutputString(gST->ConOut, L"Cannot enable Target Display Mode!\r\n");
                    break;
                }

                gBS->Stall(250000);

                Status = TdmResetLcd(SmcIo);
                if (EFI_ERROR(Status))
                {
                    gST->ConOut->OutputString(gST->ConOut, L"Cannot reset display!\r\n");
                    break;
                }

                TargetDisplayEnabled = TRUE;
            }
        }
        else
        {
            /* Cable no longer connected */
            if (TargetDisplayEnabled)
            {
                /* This code doesn't seem to reset the screen properly, might be impossible while in EFI */
                /*
                gBS->Stall(1000000);

                Status = TdmToggle(SmcIo, FALSE);
                if (EFI_ERROR(Status))
                {
                    gST->ConOut->OutputString(gST->ConOut, L"Cannot disable Target Display Mode!\r\n");
                    break;
                }

                gBS->Stall(500000);

                Status = TdmResetLcd(SmcIo);
                if (EFI_ERROR(Status))
                {
                    gST->ConOut->OutputString(gST->ConOut, L"Cannot reset display!\r\n");
                    break;
                }


                TargetDisplayEnabled = FALSE;
                */

                gST->RuntimeServices->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
            }

            gST->ConOut->OutputString(gST->ConOut, L"No signal\r\n");
        }

        gBS->Stall(500000);
    }

    /* We broke out of the loop, error out */
    gST->ConOut->OutputString(gST->ConOut, L"Something went wrong (see above)\r\n");
    return EFI_INVALID_PARAMETER;
}
