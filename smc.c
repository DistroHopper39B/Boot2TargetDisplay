/* SPDX-License-Identifier: MIT */

#include <boot2target.h>

EFI_GUID gAppleSmcIoProtocolGuid = APPLE_SMC_IO_PROTOCOL_GUID;

#define SMC_KEY_MVHR SMC_MAKE_KEY('M', 'V', 'H', 'R') /* SMC enable key */
#define SMC_KEY_MVMR SMC_MAKE_KEY('M', 'V', 'M', 'R') /* Display reset key */
#define SMC_KEY_MVMS SMC_MAKE_KEY('M', 'V', 'M', 'S') /* Cable connect toggle: 3 = disconnected, 6 or 8 = connected */

BOOLEAN TdmIsCableConnected(
        APPLE_SMC_IO_PROTOCOL *SmcIo
)
{
    EFI_STATUS  Status;
    SMC_DATA    Value;

    Status = SmcIo->SmcReadValue(
            SmcIo,
            SMC_KEY_MVMS,
            sizeof(Value),
            &Value
    );

    if (EFI_ERROR(Status))
    {
        gST->ConOut->OutputString(gST->ConOut, L"Cannot read cable connection value!\r\n");
        return FALSE;
    }

    /* i REFUSE to implement a proper print function. */
    CHAR8 NumChar = (CHAR8) Value + 0x30;
    CHAR16 NumWChar[3];
    NumWChar[0] = NumChar;
    NumWChar[1] = L' ';
    NumWChar[1] = L'\0';
    gST->ConOut->OutputString(gST->ConOut, NumWChar);

    return (Value != 3) ? TRUE : FALSE;
}

EFI_STATUS TdmToggle(
        APPLE_SMC_IO_PROTOCOL *SmcIo,
        BOOLEAN Enable
)
{
    EFI_STATUS  Status;
    SMC_DATA    Value;

    Value = (SMC_DATA) Enable;

    Status = SmcIo->SmcWriteValue(
            SmcIo,
            SMC_KEY_MVHR,
            sizeof(Value),
            &Value
    );

    if (EFI_ERROR(Status))
    {
        gST->ConOut->OutputString(gST->ConOut, L"Cannot send enable/disable to MVHR!\r\n");
        return Status;
    }

    return EFI_SUCCESS;
}

EFI_STATUS TdmResetLcd(
        APPLE_SMC_IO_PROTOCOL *SmcIo
)
{
    EFI_STATUS  Status;
    SMC_DATA    Value;

    Value = 2;
    Status = SmcIo->SmcWriteValue(
            SmcIo,
            SMC_KEY_MVMR,
            sizeof(Value),
            &Value
    );

    if (EFI_ERROR(Status))
    {
        gST->ConOut->OutputString(gST->ConOut, L"Cannot send reset command to MVMR!\r\n");
        return Status;
    }

    return EFI_SUCCESS;
}