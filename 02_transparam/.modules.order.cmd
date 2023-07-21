cmd_/home/orangepi/test/transparam/modules.order := {   echo /home/orangepi/test/transparam/param.ko; :; } | awk '!x[$$0]++' - > /home/orangepi/test/transparam/modules.order
