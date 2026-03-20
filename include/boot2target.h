/* SPDX-License-Identifier: MIT */

#pragma once

/* UEFI imports */
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/GraphicsOutput.h>

/* Apple SMC protocol */
#include "AppleSmcIo.h"

/* Apple variable protocol */

/* Print support */
#include "tinyprintf.h"
#include "cons.h"


EFI_STATUS TdmIsCableConnected(
        APPLE_SMC_IO_PROTOCOL *SmcIo,
        BOOLEAN *IsConnected
);

EFI_STATUS TdmToggle(
        APPLE_SMC_IO_PROTOCOL *SmcIo,
        BOOLEAN Enable
);

EFI_STATUS TdmResetLcd(
        APPLE_SMC_IO_PROTOCOL *SmcIo
);

BOOLEAN
InitializeConsole(
    VOID
    );