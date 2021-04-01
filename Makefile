# name of your application
APPLICATION = safewater

# If no BOARD is found in the environment, use this default:
BOARD ?= nucleo-f401re

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += xtimer
USEMODULE += analog_util
USEMODULE += dht
USEMODULE += fmt
USEMODULE += servo

#MODULE MQTT+ETHOS
# Default to using ethos for providing the uplink when not on native
UPLINK ?= ethos

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif

# Specify the mandatory networking modules for IPv6
USEMODULE += gnrc_ipv6_default

# Include MQTT-S client
USEMODULE += emcute

# Optimize network stack to for use with a single network interface
USEMODULE += gnrc_netif_single

USEMODULE += stdio_ethos
USEMODULE += gnrc_uhcpc


FEATURES_REQUIRED += periph_gpio periph_spi


# Allow for env-var-based override of the nodes name (EMCUTE_ID)
ifneq (,$(EMCUTE_ID))
  CFLAGS += -DEMCUTE_ID=\"$(EMCUTE_ID)\"
endif

# include UHCP client
USE_DHCPV6 ?= 0


# Ethos/native TAP interface and UHCP prefix can be configured from make command
TAP ?= tap0
IPV6_PREFIX ?= fe80:2::/64

# The Broker address, port and the default MQTT topic to subscribe.
SERVER_ADDR = fe80::1
SERVER_PORT = 1885
MQTT_TOPIC = both_directions


CFLAGS += -DSERVER_ADDR='"$(SERVER_ADDR)"'
CFLAGS += -DSERVER_PORT=$(SERVER_PORT)
CFLAGS += -DMQTT_TOPIC='"$(MQTT_TOPIC)"'

ETHOS_BAUDRATE ?= 115200
include $(CURDIR)/Makefile.ethos.conf
include $(RIOTBASE)/Makefile.include

.PHONY: host-tools

host-tools:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)


FEATURES_OPTIONAL += periph_rtc


#to compile use -Wno-unused-variable


