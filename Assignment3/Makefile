# name of your application
APPLICATION = lorawan

# If no BOARD is found in the environment, use this default:
BOARD ?= b-l072z-lrwan1

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= ../../RIOT

# Modules to include:
USEMODULE += xtimer

# The Semtech LoRa radio device (SX1276)
USEMODULE += sx1276
USEMODULE += semtech_loramac_rx

# The Semtech Loramac package
USEPKG += semtech-loramac

# Default region is Europe and default band is 868MHz
LORA_REGION ?= EU868

# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += ps


ifneq (,$(TTN_DEV_ID))
  CFLAGS += -DTTN_DEV_ID=\"$(TTN_DEV_ID)\"
endif

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include

