cmd_/home/orangepi/test/07_dev_ops/modules.order := {   echo /home/orangepi/test/07_dev_ops/file.ko; :; } | awk '!x[$$0]++' - > /home/orangepi/test/07_dev_ops/modules.order
