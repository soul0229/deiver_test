cmd_/home/orangepi/test/modules.order := {   echo /home/orangepi/test/helloworld.ko; :; } | awk '!x[$$0]++' - > /home/orangepi/test/modules.order
