cmd_/home/orangepi/test/06_dev/modules.order := {   echo /home/orangepi/test/06_dev/cdev.ko; :; } | awk '!x[$$0]++' - > /home/orangepi/test/06_dev/modules.order
