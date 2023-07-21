cmd_/home/orangepi/test/10_private_data/modules.order := {   echo /home/orangepi/test/10_private_data/private_data.ko; :; } | awk '!x[$$0]++' - > /home/orangepi/test/10_private_data/modules.order
