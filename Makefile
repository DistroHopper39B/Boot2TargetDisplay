# SPDX-License-Identifier: GPL-2.0-only

TARGET = Boot2TargetDisplay.efi
TARGET_LIB = Boot2TargetDisplay.lib

VERSION_MAJOR = 0
VERSION_MINOR = 1
VERSION_PATCH = 0

DEFINES = -DVERSION_MAJOR=$(VERSION_MAJOR) -DVERSION_MINOR=$(VERSION_MINOR) -DVERSION_PATCH=$(VERSION_PATCH)

CC = clang
CFLAGS = -Iinclude \
		-Iinclude/uefi \
		-Iinclude/uefi/X64 \
		-target x86_64-unknown-windows \
		-O2 \
		-fno-stack-protector \
		-fshort-wchar \
		-mno-red-zone \
		-Wall \
		$(DEFINES)

LD = lld-link
LDFLAGS = -subsystem:efi_application -nodefaultlib -dll

.PHONY: all clean

OBJS := boot2target.o smc.o cons.o baselibc_string.o tinyprintf.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -entry:EfiMain $^ -out:$@

all: $(TARGET)

clean:
	rm -f $(TARGET) $(TARGET_LIB) $(OBJS)