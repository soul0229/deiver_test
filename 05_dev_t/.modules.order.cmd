cmd_/home/orangepi/test/dev_t/modules.order := {   echo /home/orangepi/test/dev_t/dev_t.ko; :; } | awk '!x[$$0]++' - > /home/orangepi/test/dev_t/modules.order
