cmd_/home/orangepi/test/12_miscdevice/modules.order := {   echo /home/orangepi/test/12_miscdevice/miscdevice.ko; :; } | awk '!x[$$0]++' - > /home/orangepi/test/12_miscdevice/modules.order
