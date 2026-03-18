# SPDX-License-Identifier: GPL-2.0-only

TARGET = Boot2TargetDisplay.efi
CC = clang
CFLAGS = -Iinclude \
		-Iinclude/uefi \
		-Iinclude/uefi/X64 \
		-target x86_64-unknown-windows \
		-O2 \
		-fno-stack-protector \
		-fshort-wchar \
		-mno-red-zone \
		-Wall
LD = lld-link
LDFLAGS = -subsystem:efi_application -nodefaultlib -dll

.PHONY: all clean

OBJS := boot2target.o smc.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -entry:EfiMain $^ -out:$@

all: $(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)