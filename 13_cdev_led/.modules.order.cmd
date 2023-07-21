cmd_/home/orangepi/test/13_cdev_led/modules.order := {   echo /home/orangepi/test/13_cdev_led/cdev_led.ko; :; } | awk '!x[$$0]++' - > /home/orangepi/test/13_cdev_led/modules.order
