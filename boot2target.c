/* SPDX-License-Identifier: MIT */

#include <boot2target.h>

EFI_HANDLE          gImageHandle;
EFI_SYSTEM_TABLE    *gST;
EFI_BOOT_SERVICES   *gBS;

static NORETURN
VOID
Reboot(VOID)
{
    while (1)
    {
        gST->RuntimeServices->ResetSystem(
            EfiResetCold,
            EFI_SUCCESS,
            0,
            NULL);
    }
}

static NORETURN
VOID
Halt(VOID)
{
    while (1)
    {
        gST->RuntimeServices->ResetSystem(
            EfiResetShutdown,
            EFI_SUCCESS,
            0,
            NULL);
    }
}

EFI_STATUS
EFIAPI
EfiMain(
        IN EFI_HANDLE ImageHandle,
        IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS              Status;
    APPLE_SMC_IO_PROTOCOL   *SmcIo = NULL;
    BOOLEAN                 TargetDisplayEnabled = FALSE;
    BOOLEAN                 CableConnected;

    gST = SystemTable;
    gBS = SystemTable->BootServices;
    gImageHandle = ImageHandle;

    /* Disable watchdog timer so we don't auto-reboot after 5 minutes */
    gBS->SetWatchdogTimer(0, 0, 0, NULL);

    /* Initialize console */
    if (!InitializeConsole())
    {
        // Reboot without question if this fails for some reason
        Reboot();
    }

    /* Find Apple SMC I/O protocol */
    Status = gBS->LocateProtocol(
            &gAppleSmcIoProtocolGuid,
            NULL,
            (VOID **) &SmcIo
            );

    if (EFI_ERROR(Status))
    {
        printf("Cannot find SMC I/O protocol!\n");
        goto Fail;
    }

    printf("Waiting for cable connection...\n");

    /* FIXME: Use timers */
    while (1)
    {
        Status = TdmIsCableConnected(SmcIo, &CableConnected);
        if (EFI_ERROR(Status))
        {
            printf("Unsupported device!\n");
            break;
        }

        if (CableConnected)
        {
            if (!TargetDisplayEnabled)
            {
                Status = TdmToggle(SmcIo, TRUE);
                if (EFI_ERROR(Status))
                {
                    printf("Cannot enable Target Display Mode!\n");
                    break;
                }

                gBS->Stall(250000);

                Status = TdmResetLcd(SmcIo);
                if (EFI_ERROR(Status))
                {
                    printf("Cannot reset display!\n");
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
                gBS->Stall(1000000);

                Status = TdmToggle(SmcIo, FALSE);
                if (EFI_ERROR(Status))
                {
                    printf("Cannot disable Target Display Mode!\n");
                    break;
                }

                gBS->Stall(500000);

                Status = TdmResetLcd(SmcIo);
                if (EFI_ERROR(Status))
                {
                    printf("Cannot reset display!\n");
                    break;
                }

                TargetDisplayEnabled = FALSE;
            }
        }
        gBS->Stall(500000);
    }

    /* We broke out of the loop, error out */
Fail:
    ConsChangeBgColor(COLOR_BLACK);
    ConsChangeFgColor(COLOR_YELLOW);

    printf("Something went wrong (see above)\n");
    printf("Shutting down in 15 seconds...\n");

    gBS->Stall(15000000);
    Halt();
}
