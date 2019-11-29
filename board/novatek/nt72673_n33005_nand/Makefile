#
# (C) Copyright 2000-2004
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# SPDX-License-Identifier:	GPL-2.0+
#
KBUILD_CFLAGS := -fno-builtin -Werror -Iboard/$(BOARDDIR)/../common/include

obj-y	+= board.o update_cmds.o n33005_utils.o
obj-y	+= ../common/crypto_core.o
obj-$(CONFIG_NVT_SCRIPT_UPDATE_BLINK_LED)	+= n330xx_led_stbc.o
obj-$(CONFIG_FRC_UTILS)	+= frc_utils.o
