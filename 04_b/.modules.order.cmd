cmd_/home/orangepi/test/b/modules.order := {   echo /home/orangepi/test/b/b.ko; :; } | awk '!x[$$0]++' - > /home/orangepi/test/b/modules.order
