PROJECT = zystm32-a0
BUILD_DIR = bin

SHARED_DIR = ../common
CFILES = \
main.c \
motor.c \
delay.c \
bluetooth.c \
sonar.c \

# CFILES += api.c
# AFILES += api-asm.S

# TODO - you will need to edit these two lines!

DEVICE=stm32f103rct6
# DEVICE=stm32f103c8t6

# OOCD_FILE = board/stm32f4discovery.cfg

# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
INCLUDES += \
-I./include \
-I./src \

OPENCM3_DIR=../libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include ../rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk
