cmd_/home/orangepi/test/a/modules.order := {   echo /home/orangepi/test/a/a.ko; :; } | awk '!x[$$0]++' - > /home/orangepi/test/a/modules.order
