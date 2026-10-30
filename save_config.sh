#!/bin/bash
make -C buildroot BR2_EXTERNAL=../base_external savedefconfig
# Alternatively, if savedefconfig defaults to buildroot/configs:
cp buildroot/.config base_external/configs/aesd_qemu_defconfig
echo "Configuration saved successfully!"
