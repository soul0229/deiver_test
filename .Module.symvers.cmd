cmd_/home/orangepi/test/Module.symvers := sed 's/\.ko$$/\.o/' /home/orangepi/test/modules.order | scripts/mod/modpost    -o /home/orangepi/test/Module.symvers -e -i Module.symvers   -T -
