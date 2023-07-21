cmd_/home/orangepi/test/a/Module.symvers := sed 's/\.ko$$/\.o/' /home/orangepi/test/a/modules.order | scripts/mod/modpost    -o /home/orangepi/test/a/Module.symvers -e -i Module.symvers   -T -
