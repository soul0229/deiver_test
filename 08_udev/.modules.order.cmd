cmd_/home/orangepi/test/08_udev/modules.order := {   echo /home/orangepi/test/08_udev/udev.ko; :; } | awk '!x[$$0]++' - > /home/orangepi/test/08_udev/modules.order
