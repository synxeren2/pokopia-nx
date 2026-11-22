# Pokopia-NX minimal libnx prototype
.SUFFIXES:
ifeq ($(strip $(DEVKITPRO)),)
$(error DEVKITPRO must be set to the devkitPro installation directory)
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

TARGET := pokopia-nx
BUILD := build
SOURCES := source
ARCH := -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE
CFLAGS := -g -Wall -O2 -ffunction-sections -fdata-sections $(ARCH) -D__SWITCH__
ASFLAGS := -g $(ARCH)
LDFLAGS := -specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)
LIBS := -lnx
LIBDIRS := $(PORTLIBS) $(LIBNX)
export APP_TITLE := Pokopia-NX Prototype
export APP_AUTHOR := Community Research
export APP_VERSION := 0.0.1

ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUT := $(CURDIR)/$(TARGET)
export TOPDIR := $(CURDIR)
export VPATH := $(CURDIR)/$(SOURCES)
export DEPSDIR := $(CURDIR)/$(BUILD)
CFILES := $(notdir $(wildcard $(SOURCES)/*.c))
export LD := $(CC)
export OFILES := $(CFILES:.c=.o)
export INCLUDE := -I$(CURDIR)/$(BUILD) -I$(LIBNX)/include -I$(PORTLIBS)/include
export LIBPATHS := -L$(LIBNX)/lib -L$(PORTLIBS)/lib
export NROFLAGS += --icon=$(DEVKITPRO)/libnx/default_icon.jpg --nacp=$(CURDIR)/$(TARGET).nacp

.PHONY: all clean $(BUILD)
all: $(BUILD)
$(BUILD):
	@mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
clean:
	@rm -rf $(BUILD) $(TARGET).nro $(TARGET).nacp $(TARGET).elf
else
DEPENDS := $(OFILES:.o=.d)
.PHONY: all
all: $(OUTPUT).nro
$(OUTPUT).nro: $(OUTPUT).elf $(OUTPUT).nacp
$(OUTPUT).elf: $(OFILES)
-include $(DEPENDS)
endif
