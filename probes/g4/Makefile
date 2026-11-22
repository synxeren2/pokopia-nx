# Pokopia-NX G4: isolated C++20 build using actual lifter output
.SUFFIXES:
ifeq ($(strip $(DEVKITPRO)),)
$(error DEVKITPRO is required)
endif
include $(DEVKITPRO)/libnx/switch_rules
TARGET := pokopia-nx-g4
BUILD := build
SOURCES := source
ARCH := -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE
CFLAGS := -g -Wall -O2 -ffunction-sections -fdata-sections $(ARCH) $(INCLUDE) -D__SWITCH__
CXXFLAGS := $(CFLAGS) -std=gnu++20 -fno-exceptions -fno-rtti -fno-strict-aliasing -ffp-contract=off
ASFLAGS := -g $(ARCH)
LDFLAGS := -specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,--gc-sections,-Map,$(notdir $*.map) $(LIBPATHS)
LIBS := -lnx
LIBDIRS := $(PORTLIBS) $(LIBNX)
export APP_TITLE := Pokopia-NX G4
export APP_AUTHOR := Community Research
export APP_VERSION := 0.0.1
ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUT := $(CURDIR)/$(TARGET)
export TOPDIR := $(CURDIR)
export VPATH := $(CURDIR)/$(SOURCES)
export DEPSDIR := $(CURDIR)/$(BUILD)
CPPFILES := $(notdir $(wildcard $(SOURCES)/*.cpp))
export LD := $(CXX)
export OFILES := $(CPPFILES:.cpp=.o)
export INCLUDE := -I$(CURDIR)/$(BUILD) -I$(CURDIR)/include -I$(CURDIR)/source -I$(LIBNX)/include -I$(PORTLIBS)/include
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
