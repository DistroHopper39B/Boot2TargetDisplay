/* SPDX-License-Identifier: MIT */

#pragma once

/* UEFI imports */
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/GraphicsOutput.h>

/* Apple SMC protocol */
#include "smc_io.h"

BOOLEAN TdmIsCableConnected(
        APPLE_SMC_IO_PROTOCOL *SmcIo
);

EFI_STATUS TdmToggle(
        APPLE_SMC_IO_PROTOCOL *SmcIo,
        BOOLEAN Enable
);

EFI_STATUS TdmResetLcd(
        APPLE_SMC_IO_PROTOCOL *SmcIo
);