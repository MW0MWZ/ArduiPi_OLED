#*********************************************************************
# This is the makefile for the ArduiPi OLED library driver
#
#	02/18/2013 	Charles-Henri Hallard (http://hallard.me)
#							Modified for compiling and use on Raspberry ArduiPi Board
#							LCD size and connection are now passed as arguments on 
#							the command line (no more #define on compilation needed)
#							ArduiPi project documentation http://hallard.me/arduipi
# 
# 07/26/2013	Charles-Henri Hallard
#							modified name for generic library using different OLED type
#
# 08/26/2015	Lorenzo Delana (lorenzo.delana@gmail.com)
#							added bananapi specific CCFLAGS and conditional macro BANANPI
#
# 31-Aug-2025	Andy Taylor (MW0MWZ)
#							Updated Makefile to work with aarch64 / armv7 / armhf
#							Re-worked to work on Alpine Linux / Musl
#
# *********************************************************************

#*********************************************************************
# ArduiPi OLED library driver — portable Makefile (RasPiOS & Alpine)
#*********************************************************************

# --- Makefile location & platform metadata --------------------------
ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
HWPLAT   := $(shell cat $(ROOT_DIR)/hwplatform 2>/dev/null || echo "")
ARCH     := $(shell uname -m)

# Detect Alpine and musl
IS_ALPINE := $(shell [ -f /etc/alpine-release ] && echo yes || echo no)
IS_MUSL   := $(shell ldd --version 2>&1 | grep -iq musl && echo yes || echo no)

# --- Install prefix defaults ----------------------------------------
# On Alpine/musl, install under /usr so the dynamic loader finds libs without rpath.
ifndef PREFIX
  ifeq ($(IS_ALPINE),yes)
    PREFIX := /usr
  else
    PREFIX := /usr/local
  endif
endif

# --- Library parameters ---------------------------------------------
LIBDIR  := $(PREFIX)/lib
LIB     := libArduiPi_OLED
LIBNAME := $(LIB).so.1.0

# Compilers
CXX := g++
CC  := gcc

# --- Architecture-tuned CFLAGS --------------------------------------
# Set CCFLAGS depending on hardware platform / architecture
ifeq ($(HWPLAT),BananaPI)
  CCFLAGS = -Wall -Ofast -mfpu=vfpv4 -mfloat-abi=hard -march=armv7 -mtune=cortex-a7 -DBANANAPI
else
  ifeq ($(ARCH),armv7l)        # 32-bit ARMv7 (e.g. Raspberry Pi 2)
    CCFLAGS = -Ofast -mfpu=vfpv4 -mfloat-abi=hard -march=armv7-a -mtune=cortex-a7
  else ifeq ($(ARCH),armv6l)   # 32-bit ARMv6 (Pi 1/Zero)
    CCFLAGS = -Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s
  else ifeq ($(ARCH),aarch64)  # 64-bit ARM (Pi 3/4/5, ARM servers)
    CCFLAGS = -Ofast -march=armv8-a -mtune=cortex-a53
  else
    $(warning Unknown architecture $(ARCH), using generic -Ofast flags)
    CCFLAGS = -Ofast
  endif
endif

# --- I2C detection (prefer pkg-config, fallback to -li2c) -----------
PKGCONFIG ?= pkg-config
HAVE_I2C_PC := $(shell $(PKGCONFIG) --exists i2c && echo yes || echo no)
ifeq ($(HAVE_I2C_PC),yes)
  I2C_CFLAGS  := $(shell $(PKGCONFIG) --cflags i2c)
  I2C_LDFLAGS := $(shell $(PKGCONFIG) --libs   i2c)
else
  # Fallback: check if libi2c.so exists in standard locations
  HAVE_I2C_LIB := $(shell [ -f /usr/lib/libi2c.so ] || [ -f /usr/local/lib/libi2c.so ] || [ -f /lib/libi2c.so ] && echo yes || echo no)
  ifeq ($(HAVE_I2C_LIB),yes)
    I2C_CFLAGS  :=
    I2C_LDFLAGS := -li2c
  else
    # On Alpine/musl, always try -li2c even if we can't detect the lib file
    ifeq ($(IS_ALPINE),yes)
      $(info Alpine detected: forcing I2C linking with -li2c)
      I2C_CFLAGS  :=
      I2C_LDFLAGS := -li2c
    else
      $(warning No I2C library found - SMBus functions may not be available)
      I2C_CFLAGS  :=
      I2C_LDFLAGS :=
    endif
  endif
endif

# Final flags
CFLAGS  := $(CCFLAGS) $(I2C_CFLAGS)
LDFLAGS := $(LDFLAGS) $(I2C_LDFLAGS)

# If using musl and installing to a non-standard loader path, inject rpath automatically
# But skip rpath if this is a temporary build directory (contains 'src' or 'tmp')
ifeq ($(IS_MUSL),yes)
  ifneq ($(PREFIX),/usr)
    ifeq ($(findstring src,$(PREFIX)),)
      ifeq ($(findstring tmp,$(PREFIX)),)
        LDFLAGS += -Wl,-rpath,$(LIBDIR)
      endif
    endif
  endif
endif

# --- Build targets ---------------------------------------------------
# Reinstall the library after each recompilation
all: ArduiPi_OLED install

# Shared library link
ArduiPi_OLED: ArduiPi_OLED.o Adafruit_GFX.o bcm2835.o Wrapper.o
	$(CXX) -shared -Wl,-soname,$(LIB).so.1 $(CFLAGS) $(LDFLAGS) -o $(LIBNAME) $^

# Objects (use -fno-rtti to avoid link issues some setups have)
ArduiPi_OLED.o: ArduiPi_OLED.cpp
	$(CXX) -Wall -fPIC -fno-rtti $(CFLAGS) -c $^

Adafruit_GFX.o: Adafruit_GFX.cpp
	$(CXX) -Wall -fPIC -fno-rtti $(CFLAGS) -c $^

bcm2835.o: bcm2835.c
	$(CC)  -Wall -fPIC $(CFLAGS) -c $^

Wrapper.o: Wrapper.cpp
	$(CXX) -Wall -fPIC $(CFLAGS) -c $^

# --- Install ---------------------------------------------------------
install:
	@echo "[Install Library]"
	@mkdir -p $(LIBDIR)
	@install -m 0755 $(LIBNAME) $(LIBDIR)
	@ln -sf $(LIBDIR)/$(LIBNAME) $(LIBDIR)/$(LIB).so.1
	@ln -sf $(LIBDIR)/$(LIBNAME) $(LIBDIR)/$(LIB).so
# ldconfig is glibc-specific; guard so Alpine/musl won't error
	@command -v ldconfig >/dev/null 2>&1 && ldconfig || true
	@rm -f $(LIB).*

	@echo "[Install Headers]"
	@mkdir -p $(PREFIX)/include
	@cp -f Adafruit_*.h $(PREFIX)/include
	@cp -f ArduiPi_*.h $(PREFIX)/include
	@cp -f bcm2835.h    $(PREFIX)/include

# --- Uninstall / Clean ----------------------------------------------
uninstall:
	@echo "[Uninstall Library]"
	@rm -f $(LIBDIR)/$(LIB).so $(LIBDIR)/$(LIB).so.1 $(LIBDIR)/$(LIBNAME)
	@echo "[Uninstall Headers]"
	@rm -f $(PREFIX)/include/ArduiPi_OLED* $(PREFIX)/include/bcm2835.h

clean:
	rm -f *.o $(LIB).* $(LIBDIR)/$(LIB).*