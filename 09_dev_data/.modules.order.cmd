cmd_/home/orangepi/test/09_dev_data/modules.order := {   echo /home/orangepi/test/09_dev_data/dev_data.ko; :; } | awk '!x[$$0]++' - > /home/orangepi/test/09_dev_data/modules.order
