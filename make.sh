#!/bin/bash
export NVT_PRJCFG_MODEL_CFG=/home/danny/novatek/u-boot/board/novatek/nvt-na51000/ModelConfig.mk
export ARCH=arm
export CROSS_COMPILE=/opt/arm/arm-ca53-linux-gnueabihf-6.4/usr/bin/arm-ca53-linux-gnueabihf-
make disclean 
make nvt-na51000_nand_defconfig
make -j4
