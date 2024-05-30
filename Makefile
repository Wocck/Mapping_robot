APPLICATION = mapbot
BOARD ?= nucleo-l476rg

#RIOTBASE ?= /data/RIOT/RIOT
RIOTBASE ?= $(CURDIR)/../RIOT

FEATURES_REQUIRED += periph_timer
FEATURES_REQUIRED += cpp
FEATURES_REQUIRED += libstdcpp

USEMODULE += periph_gpio
USEMODULE += periph_gpio_irq
USEMODULE += periph_uart
USEMODULE += periph_pwm
USEMODULE += xtimer
USEMODULE += shell


include $(RIOTBASE)/Makefile.include
