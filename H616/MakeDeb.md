# make uboot deb
> ## 1.create dir
```cd uboot```
```
mkdir -p /tmp/tmp.VMG1AHmY6k/linux-u-boot-next-orangepizero2_3.1.0_arm64/usr/lib/u-boot \
		/tmp/tmp.VMG1AHmY6k/linux-u-boot-next-orangepizero2_3.1.0_arm64/usr/lib/linux-u-boot-next-orangepizero2_3.1.0_arm64 \
		/tmp/tmp.VMG1AHmY6k/linux-u-boot-next-orangepizero2_3.1.0_arm64/DEBIAN
```
> ## 2.copy compiled file
```
cp u-boot-sunxi-with-spl.bin \
	/tmp/tmp.VMG1AHmY6k/linux-u-boot-next-orangepizero2_3.1.0_arm64/usr/lib/linux-u-boot-next-orangepizero2_3.1.0_arm64/u-boot-sunxi-with-spl.bin
```
>>#### /tmp/tmp.VMG1AHmY6k/linux-u-boot-next-orangepizero2_3.1.0_arm64/usr/lib/u-boot/platform_install.sh
```
DIR=/usr/lib/linux-u-boot-next-orangepizero2_3.1.0_arm64
write_uboot_platform () 
{ 
    if [[ -f $1/boot0_sdcard.fex ]]; then
        dd if=$1/boot0_sdcard.fex of=$2 bs=8k seek=1 conv=fsync > /dev/null 2>&1;
        dd if=$1/boot_package.fex of=$2 bs=8k seek=2050 conv=fsync > /dev/null 2>&1 || true;
    else
        if [[ -f $1/u-boot-with-dtb.bin ]]; then
            dd if=/dev/zero of=$2 bs=1k count=1023 seek=1 status=noxfer > /dev/null 2>&1;
            dd if=$1/u-boot-with-dtb.bin of=$2 bs=1k seek=8 conv=fsync > /dev/null 2>&1 || true;
        else
            if [[ -f $1/u-boot-sunxi-with-spl.bin ]]; then
                dd if=/dev/zero of=$2 bs=1k count=1023 seek=1 status=noxfer > /dev/null 2>&1;
                dd if=$1/u-boot-sunxi-with-spl.bin of=$2 bs=1k seek=8 conv=fsync > /dev/null 2>&1 || true;
            else
                [[ -f $1/sunxi-spl.bin ]] && dd if=$1/sunxi-spl.bin of=$2 bs=8k seek=1 conv=fsync > /dev/null 2>&1;
                [[ -f $1/u-boot.itb ]] && dd if=$1/u-boot.itb of=$2 bs=8k seek=5 conv=fsync > /dev/null 2>&1 || true;
            fi;
        fi;
    fi
}

setup_write_uboot_platform () 
{ 
    if grep -q "ubootpart" /proc/cmdline; then
        local tmp=$(cat /proc/cmdline);
        tmp="${tmp##*ubootpart=}";
        tmp="${tmp%% *}";
        [[ -n $tmp ]] && local part=$(findfs PARTUUID=$tmp 2>/dev/null);
        [[ -n $part ]] && local dev=$(lsblk -n -o PKNAME $part 2>/dev/null);
        [[ -n $dev ]] && DEVICE="/dev/$dev";
    else
        local tmp=$(cat /proc/cmdline);
        tmp="${tmp##*root=}";
        tmp="${tmp%% *}";
        [[ -n $tmp ]] && local part=$(findfs $tmp 2>/dev/null);
        [[ -n $part ]] && local dev=$(lsblk -n -o PKNAME $part 2>/dev/null);
        [[ -n $dev && $dev == mmcblk* ]] && DEVICE="/dev/$dev";
    fi
}
```
>>#### /tmp/tmp.VMG1AHmY6k/linux-u-boot-next-orangepizero2_3.1.0_arm64/DEBIAN/control
```
Package: linux-u-boot-orangepizero2-next
Version: 3.1.0
Architecture: arm64
Maintainer: Orange Pi <leeboby@aliyun.com>
Installed-Size: 1
Section: kernel
Priority: optional
Provides: orangepi-u-boot
Replaces: orangepi-u-boot
Conflicts: orangepi-u-boot, u-boot-sunxi
Description: Uboot loader 2021.10
```
```cp .config /tmp/tmp.VMG1AHmY6k/linux-u-boot-next-orangepizero2_3.1.0_arm64/usr/lib/u-boot/orangepi_zero2_defconfig```
```cp COPYING /tmp/tmp.VMG1AHmY6k/linux-u-boot-next-orangepizero2_3.1.0_arm64/usr/lib/u-boot/LICENSE```
> ## 3.make deb
```fakeroot dpkg-deb -b -Zxz /tmp/tmp.VMG1AHmY6k/linux-u-boot-next-orangepizero2_3.1.0_arm64 /tmp/tmp.VMG1AHmY6k/linux-u-boot-next-orangepizero2_3.1.0_arm64.deb```
# make kernel deb
```
	cd ${kerneldir}
	cp -p /media/wish/ubuntu/H618/orangepi-build/external/config/kernel/linux-6.1-sun50iw9-next.config .config
	make -j18 ARCH=arm64 CROSS_COMPILE=ccache aarch64-none-linux-gnu- LOCALVERSION=-sun50iw9 Image modules dtbs
	make j18 bindeb-pkg KDEB_PKGVERSION=3.1.0 KDEB_COMPRESS=xz BRANCH=next LOCALVERSION=-sun50iw9 KBUILD_DEBARCH=arm64 ARCH=arm64 DEBFULLNAME=Orange Pi DEBEMAIL=leeboby@aliyun.com CROSS_COMPILE=ccache \
		aarch64-none-linux-gnu-
```
# compile_orangepi-config deb
```
make -p /media/wish/ubuntu/H618/orangepi-build/.tmp/orangepi-config_3.1.0_all/DEBIAN /media/wish/ubuntu/H618/orangepi-build/.tmp/orangepi-config_3.1.0_all/usr/bin/ /media/wish/ubuntu/H618/orangepi-build/.tmp/ \
	orangepi-config_3.1.0_all/usr/sbin/ /media/wish/ubuntu/H618/orangepi-build/.tmp/orangepi-config_3.1.0_all/usr/lib/orangepi-config/
```
>>#### /media/wish/ubuntu/H618/orangepi-build/.tmp/orangepi-config_3.1.0_all/DEBIAN/control
```
Package: orangepi-config
Version: 3.1.0
Architecture: all
Maintainer: Orange Pi <leeboby@aliyun.com>
Replaces: orangepi-bsp
Depends: bash, iperf3, psmisc, curl, bc, expect, dialog, pv, 	debconf-utils, unzip, build-essential, html2text, apt-transport-https, html2text, dirmngr, software-properties-common
Recommends: orangepi-bsp
Suggests: libpam-google-authenticator, qrencode, network-manager, sunxi-tools
Section: utils
Priority: optional
Description: Orange Pi configuration utility
```
```fakeroot dpkg-deb -b -Zxz /media/wish/ubuntu/H618/orangepi-build/.tmp/orangepi-config_3.1.0_all```
# orangepi-zsh deb
```mkdir -p /tmp/tmp.prKZH0v2UW/orangepi-zsh_3.1.0_all/DEBIAN /tmp/tmp.prKZH0v2UW/orangepi-zsh_3.1.0_all/etc/skel/ /tmp/tmp.prKZH0v2UW/orangepi-zsh_3.1.0_all/etc/oh-my-zsh/ /tmp/tmp.prKZH0v2UW/orangepi-zsh_3.1.0_all#etc/skel/.oh-my-zsh/cache```
>>#### /tmp/tmp.prKZH0v2UW/orangepi-zsh_3.1.0_all/DEBIAN/control
```
Package: orangepi-zsh
Version: 3.1.0
Architecture: all
Maintainer: Orange Pi <leeboby@aliyun.com>
Depends: zsh, tmux
Section: utils
Priority: optional
Description: Orange Pi improved ZShell
```
>>#### /tmp/tmp.prKZH0v2UW/orangepi-zsh_3.1.0_all/DEBIAN/postinst
```
#!/bin/sh

# copy cache directory if not there yet
awk -F'[:]' '{if ($3 >= 1000 && $3 != 65534 || $3 == 0) print ""$6"/.oh-my-zsh"}' /etc/passwd | xargs -i sh -c 'test ! -d {} && cp -R --attributes-only /etc/skel/.oh-my-zsh {}'
awk -F'[:]' '{if ($3 >= 1000 && $3 != 65534 || $3 == 0) print ""$6"/.zshrc"}' /etc/passwd | xargs -i sh -c 'test ! -f {} && cp -R /etc/skel/.zshrc {}'

# fix owner permissions in home directory
awk -F'[:]' '{if ($3 >= 1000 && $3 != 65534 || $3 == 0) print ""$1":"$3" "$6"/.oh-my-zsh"}' /etc/passwd | xargs -n2 chown -R
awk -F'[:]' '{if ($3 >= 1000 && $3 != 65534 || $3 == 0) print ""$1":"$3" "$6"/.zshrc"}' /etc/passwd | xargs -n2 chown -R

# add support for bash profile
! grep emulate /etc/zsh/zprofile  >/dev/null && echo "emulate sh -c 'source /etc/profile'" >> /etc/zsh/zprofile
exit 0
```
```
	cp -R /media/wish/ubuntu/H618/orangepi-build/external/cache/sources/oh-my-zsh /tmp/tmp.prKZH0v2UW/orangepi-zsh_3.1.0_all/etc/
	cp -R /media/wish/ubuntu/H618/orangepi-build/external/cache/sources/evalcache /tmp/tmp.prKZH0v2UW/orangepi-zsh_3.1.0_all/etc/oh-my-zsh/plugins
	chmod -R g-w,o-w /tmp/tmp.prKZH0v2UW/orangepi-zsh_3.1.0_all/etc/oh-my-zsh/

	# we have common settings
	sed -i "s/^export ZSH=.*/export ZSH=/etc/oh-my-zsh/" /tmp/tmp.prKZH0v2UW/orangepi-zsh_3.1.0_all/etc/skel/.zshrc

	# user cache
	sed -i "/^export ZSH=.*/a export ZSH_CACHE_DIR=~\/.oh-my-zsh\/cache" /tmp/tmp.prKZH0v2UW/orangepi-zsh_3.1.0_all/etc/skel/.zshrc

	# define theme
	sed -i 's/^ZSH_THEME=.*/ZSH_THEME="mrtazz"/' /tmp/tmp.prKZH0v2UW/orangepi-zsh_3.1.0_all/etc/skel/.zshrc

	# disable prompt while update
	sed -i 's/# DISABLE_UPDATE_PROMPT="true"/DISABLE_UPDATE_PROMPT="true"/g' /tmp/tmp.prKZH0v2UW/orangepi-zsh_3.1.0_all/etc/skel/.zshrc

	# disable auto update since we provide update via package
	sed -i 's/# DISABLE_AUTO_UPDATE="true"/DISABLE_AUTO_UPDATE="true"/g' /tmp/tmp.prKZH0v2UW/orangepi-zsh_3.1.0_all/etc/skel/.zshrc

	# define default plugins
	sed -i 's/^plugins=.*/plugins=(evalcache git git-extras debian tmux screen history extract colorize web-search docker)/' /tmp/tmp.prKZH0v2UW/orangepi-zsh_3.1.0_all/etc/skel/.zshrc

	chmod 755 /tmp/tmp.prKZH0v2UW/orangepi-zsh_3.1.0_all/DEBIAN/postinst

	fakeroot dpkg-deb -b -Zxz /tmp/tmp.prKZH0v2UW/orangepi-zsh_3.1.0_all
```
# plymouth-theme-orangepi deb
```mkdir -p /tmp/tmp.LvSr7DVWVC/orangepi-plymouth-theme_3.1.0_all/DEBIAN /tmp/tmp.LvSr7DVWVC/orangepi-plymouth-theme_3.1.0_all/usr/share/plymouth/themes/orangepi```
>>#### /tmp/tmp.LvSr7DVWVC/orangepi-plymouth-theme_3.1.0_all/DEBIAN/control
```
Package: orangepi-plymouth-theme
Version: 3.1.0
Architecture: all
Maintainer: Orange Pi <leeboby@aliyun.com>
Depends: plymouth, plymouth-themes
Section: universe/x11
Priority: optional
Description: boot animation, logger and I/O multiplexer - orangepi theme
```
```cp /media/wish/ubuntu/H618/orangepi-build/external/packages/plymouth-theme-orangepi/debian/postinst /media/wish/ubuntu/H618/orangepi-build/external/packages/plymouth-theme-orangepi/debian/prerm /media/wish/ubuntu/H618/orangepi-build/external/packages/plymouth-theme-orangepi/debian/postrm /tmp/tmp.LvSr7DVWVC/orangepi-plymouth-theme_3.1.0_all/DEBIAN/```

```chmod 755 /tmp/tmp.LvSr7DVWVC/orangepi-plymouth-theme_3.1.0_all/DEBIAN/{postinst,prerm,postrm}```

```	
	convert -resize 52x52 \
		/media/wish/ubuntu/H618/orangepi-build/external/packages/plymouth-theme-orangepi/spinner.gif \
		/tmp/tmp.LvSr7DVWVC/orangepi-plymouth-theme_3.1.0_all/usr/share/plymouth/themes/orangepi/throbber-%04d.png

	cp /media/wish/ubuntu/H618/orangepi-build/external/packages/plymouth-theme-orangepi/watermark.png \
		/tmp/tmp.LvSr7DVWVC/orangepi-plymouth-theme_3.1.0_all/usr/share/plymouth/themes/orangepi/

	cp /media/wish/ubuntu/H618/orangepi-build/external/packages/plymouth-theme-orangepi/{bullet,capslock,entry,keyboard,keymap-render,lock}.png \
		/tmp/tmp.LvSr7DVWVC/orangepi-plymouth-theme_3.1.0_all/usr/share/plymouth/themes/orangepi/

	cp /media/wish/ubuntu/H618/orangepi-build/external/packages/plymouth-theme-orangepi/orangepi.plymouth \
		/tmp/tmp.LvSr7DVWVC/orangepi-plymouth-theme_3.1.0_all/usr/share/plymouth/themes/orangepi/

	fakeroot dpkg-deb -b -Zxz /tmp/tmp.LvSr7DVWVC/orangepi-plymouth-theme_3.1.0_all
```
# firmware deb

```
	mkdir -p "/tmp/tmp.oCPZc5I7SG/orangepi-firmware/lib/firmware"
	cp -af --reflink=auto /media/wish/ubuntu/H618/orangepi-build/external/cache/sources/orangepi-firmware-git/* "/tmp/tmp.oCPZc5I7SG/orangepi-firmware/lib/firmware/"
	rm -rf /tmp/tmp.oCPZc5I7SG/orangepi-firmware/lib/firmware/{amdgpu,amd-ucode,radeon,nvidia,matrox,.git}
	cd /tmp/tmp.oCPZc5I7SG/orangepi-firmware
	mkdir -p DEBIAN
```
>>#### DEBIAN/control
```
Package: orangepi-firmware
Version: 3.1.0
Architecture: all
Maintainer: Orange Pi <leeboby@aliyun.com>
Installed-Size: 1
Replaces: linux-firmware, firmware-brcm80211, firmware-ralink, firmware-samsung, firmware-realtek, orangepi-firmware-full
Section: kernel
Priority: optional
Description: Linux firmware
```
```
	cd /tmp/tmp.oCPZc5I7SG
	mv orangepi-firmware orangepi-firmware_3.1.0_all
	fakeroot dpkg-deb -b -Zxz orangepi-firmware_3.1.0_all
	mv orangepi-firmware_3.1.0_all orangepi-firmware
```
# create_board_package deb
```
	mkdir -p /tmp/tmp.7rBWxg3Myx/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64/DEBIAN
	cd /tmp/tmp.7rBWxg3Myx/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64
	cp -r /media/wish/ubuntu/H618/orangepi-build/external/config/optional/families/sun50iw9/_packages/bsp-cli/usr /tmp/tmp.7rBWxg3Myx/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64/
	mkdir -p /tmp/tmp.7rBWxg3Myx/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64/usr/share/orangepi/
	cp /media/wish/ubuntu/H618/orangepi-build/external/config/bootscripts/boot-sun50iw9-next.cmd /tmp/tmp.7rBWxg3Myx/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64/usr/share/orangepi/boot.cmd
```
>>#### /tmp/tmp.7rBWxg3Myx/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64/DEBIAN/control
```
Package: orangepi-bsp-cli-orangepizero2
Version: 3.1.0
Architecture: arm64
Maintainer: Orange Pi <leeboby@aliyun.com>
Installed-Size: 1
Section: kernel
Priority: optional
Depends: bash, linux-base, u-boot-tools, initramfs-tools, lsb-release, fping
Provides: linux-jammy-root-legacy-orangepizero2, linux-jammy-root-current-orangepizero2, linux-jammy-root-next-orangepizero2
Suggests: orangepi-config
Replaces: zram-config, base-files, orangepi-tools-jammy, linux-jammy-root-legacy-orangepizero2 (<< 3.1.0~), linux-jammy-root-current-orangepizero2 (<< 3.1.0~), linux-jammy-root-next-orangepizero2 (<< 3.1.0~)
Breaks: linux-jammy-root-legacy-orangepizero2 (<< 3.1.0~), linux-jammy-root-current-orangepizero2 (<< 3.1.0~), linux-jammy-root-next-orangepizero2 (<< 3.1.0~)
Recommends: bsdutils, parted, util-linux, toilet
Description: OrangePi board support files for orangepizero2
```
>>#### /tmp/tmp.7rBWxg3Myx/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64/DEBIAN/preinst
```
#!/bin/sh

# tell people to reboot at next login
[ "$1" = "upgrade" ] && touch /var/run/.reboot_required

# convert link to file
if [ -L "/etc/network/interfaces" ]; then

    cp /etc/network/interfaces /etc/network/interfaces.tmp
    rm /etc/network/interfaces
    mv /etc/network/interfaces.tmp /etc/network/interfaces

fi

# fixing ramdisk corruption when using lz4 compression method
sed -i "s/^COMPRESS=.*/COMPRESS=gzip/" /etc/initramfs-tools/initramfs.conf

# swap
grep -q vm.swappiness /etc/sysctl.conf
case $? in
0)
    sed -i 's/vm\.swappiness.*/vm.swappiness=100/' /etc/sysctl.conf
    ;;
*)
    echo vm.swappiness=100 >>/etc/sysctl.conf
    ;;
esac
sysctl -p >/dev/null 2>&1

# disable deprecated services
[ -f "/etc/profile.d/activate_psd_user.sh" ] && rm /etc/profile.d/activate_psd_user.sh
[ -f "/etc/profile.d/check_first_login.sh" ] && rm /etc/profile.d/check_first_login.sh
[ -f "/etc/profile.d/check_first_login_reboot.sh" ] && rm /etc/profile.d/check_first_login_reboot.sh
[ -f "/etc/profile.d/ssh-title.sh" ] && rm /etc/profile.d/ssh-title.sh
[ -f "/etc/update-motd.d/10-header" ] && rm /etc/update-motd.d/10-header
[ -f "/etc/update-motd.d/30-sysinfo" ] && rm /etc/update-motd.d/30-sysinfo
[ -f "/etc/update-motd.d/35-tips" ] && rm /etc/update-motd.d/35-tips
[ -f "/etc/update-motd.d/40-updates" ] && rm /etc/update-motd.d/40-updates
[ -f "/etc/update-motd.d/98-autoreboot-warn" ] && rm /etc/update-motd.d/98-autoreboot-warn
[ -f "/etc/update-motd.d/99-point-to-faq" ] && rm /etc/update-motd.d/99-point-to-faq
[ -f "/etc/update-motd.d/80-esm" ] && rm /etc/update-motd.d/80-esm
[ -f "/etc/update-motd.d/80-livepatch" ] && rm /etc/update-motd.d/80-livepatch
[ -f "/etc/apt/apt.conf.d/02compress-indexes" ] && rm /etc/apt/apt.conf.d/02compress-indexes
[ -f "/etc/apt/apt.conf.d/02periodic" ] && rm /etc/apt/apt.conf.d/02periodic
[ -f "/etc/apt/apt.conf.d/no-languages" ] && rm /etc/apt/apt.conf.d/no-languages
[ -f "/etc/init.d/armhwinfo" ] && rm /etc/init.d/armhwinfo
[ -f "/etc/logrotate.d/armhwinfo" ] && rm /etc/logrotate.d/armhwinfo
[ -f "/etc/init.d/firstrun" ] && rm /etc/init.d/firstrun
[ -f "/etc/init.d/resize2fs" ] && rm /etc/init.d/resize2fs
[ -f "/lib/systemd/system/firstrun-config.service" ] && rm /lib/systemd/system/firstrun-config.service
[ -f "/lib/systemd/system/firstrun.service" ] && rm /lib/systemd/system/firstrun.service
[ -f "/lib/systemd/system/resize2fs.service" ] && rm /lib/systemd/system/resize2fs.service
[ -f "/usr/lib/orangepi/apt-updates" ] && rm /usr/lib/orangepi/apt-updates
[ -f "/usr/lib/orangepi/firstrun-config.sh" ] && rm /usr/lib/orangepi/firstrun-config.sh
# fix for https:#bugs.launchpad.net/ubuntu/+source/lightdm-gtk-greeter/+bug/1897491
[ -d "/var/lib/lightdm" ] && (chown -R lightdm:lightdm /var/lib/lightdm ; chmod 0750 /var/lib/lightdm)
exit 0
```
```	chmod 755 /tmp/tmp.7rBWxg3Myx/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64/DEBIAN/preinst```
>>#### /tmp/tmp.7rBWxg3Myx/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64/DEBIAN/postrm
```
#!/bin/sh
if [ remove = "$1" ] || [ abort-install = "$1" ]; then

    systemctl disable orangepi-hardware-monitor.service orangepi-hardware-optimize.service >/dev/null 2>&1
    systemctl disable orangepi-zram-config.service orangepi-ramlog.service >/dev/null 2>&1

fi
exit 0
```
```	chmod 755 /tmp/tmp.7rBWxg3Myx/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64/DEBIAN/postrm```
>>#### /tmp/tmp.7rBWxg3Myx/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64/DEBIAN/postinst
```
#!/bin/sh
#
# orangepizero2 BSP post installation script
#

[ -f /etc/lib/systemd/system/orangepi-ramlog.service ] && systemctl --no-reload enable orangepi-ramlog.service

# check if it was disabled in config and disable in new service
if [ -n "$(grep -w '^ENABLED=false' /etc/default/log2ram 2> /dev/null)" ]; then

     sed -i "s/^ENABLED=.*/ENABLED=false/" /etc/default/orangepi-ramlog

fi

# fix boot delay "waiting for suspend/resume device"
if [ -f "/etc/initramfs-tools/initramfs.conf" ]; then

    if ! grep --quiet "RESUME=none" /etc/initramfs-tools/initramfs.conf; then
         echo "RESUME=none" >> /etc/initramfs-tools/initramfs.conf
    fi

fi

if [ ! -f /boot/boot.cmd ]; then

# if boot script does not exits its recreated
    # move bootscript to /usr/share/orangepi
    # create a backup
    [ -f /etc/orangepi-release ] &&  . /etc/orangepi-release
    [ -z ${VERSION} ] && VERSION=`date +%s`
    if [ -f /boot/boot.cmd ]; then
       cp /boot/boot.cmd /usr/share/orangepi/boot.cmd-${VERSION} >/dev/null 2>&1
       echo "NOTE: You can find previous bootscript versions in /usr/share/orangepi !"
    fi

    # cleanup old bootscript backup
    ls /usr/share/orangepi/boot.cmd-* >/dev/null 2>&1 | head -n -5 | xargs rm -f --
    ls /usr/share/orangepi/boot.ini-* >/dev/null 2>&1 | head -n -5 | xargs rm -f --

    echo "Recreating boot script"
    cp /usr/share/orangepi/boot.cmd /boot  >/dev/null 2>&1
    rootdev=$(sed -e 's/^.*root=#' -e 's/ .*$#' < /proc/cmdline)
    rootfstype=$(sed -e 's/^.*rootfstype=#' -e 's/ .*$#' < /proc/cmdline)

    # recreate orangepiEnv.txt if it and extlinux does not exists
    if [ ! -f /boot/orangepiEnv.txt ] && [ ! -f /boot/extlinux/extlinux.conf ]; then
      cp /usr/share/orangepi/orangepiEnv.txt /boot  >/dev/null 2>&1
      echo "rootdev="$rootdev >> /boot/orangepiEnv.txt
      echo "rootfstype="$rootfstype >> /boot/orangepiEnv.txt
    fi

    [ -f /boot/boot.ini ] && sed -i "s/setenv rootdev.*/setenv rootdev \"$rootdev\"/" /boot/boot.ini
    [ -f /boot/boot.ini ] && sed -i "s/setenv rootfstype.*/setenv rootfstype \"$rootfstype\"/" /boot/boot.ini
    [ -f /boot/boot.cmd ] && mkimage -C none -A arm -T script -d /boot/boot.cmd /boot/boot.scr  >/dev/null 2>&1

fi

[ ! -f "/etc/network/interfaces" ] && [ -f "/etc/network/interfaces.default" ] && cp /etc/network/interfaces.default /etc/network/interfaces
ln -sf /var/run/motd /etc/motd
rm -f /etc/update-motd.d/00-header /etc/update-motd.d/10-help-text
if [ -f "/boot/bin/orangepizero2.bin" ] && [ ! -f "/boot/script.bin" ]; then ln -sf bin/orangepizero2.bin /boot/script.bin >/dev/null 2>&1 || cp /boot/bin/orangepizero2.bin /boot/script.bin; fi
if [ ! -f "/etc/default/orangepi-motd" ]; then
mv /etc/default/orangepi-motd.dpkg-dist /etc/default/orangepi-motd
fi
if [ ! -f "/etc/default/orangepi-ramlog" ] && [ -f /etc/default/orangepi-ramlog.dpkg-dist ]; then
mv /etc/default/orangepi-ramlog.dpkg-dist /etc/default/orangepi-ramlog
fi
if [ ! -f "/etc/default/orangepi-zram-config" ] && [ -f /etc/default/orangepi-zram-config.dpkg-dist ]; then
mv /etc/default/orangepi-zram-config.dpkg-dist /etc/default/orangepi-zram-config
fi

if [ -L "/usr/lib/chromium-browser/master_preferences.dpkg-dist" ]; then
mv /usr/lib/chromium-browser/master_preferences.dpkg-dist /usr/lib/chromium-browser/master_preferences
fi

# Read release value
if [ -f /etc/lsb-release ]; then
RELEASE=$(cat /etc/lsb-release | grep CODENAME | cut -d"=" -f2 | sed 's/.*/\u&/')
sed -i "s/^PRETTY_NAME=.*/PRETTY_NAME=\"Orange Pi 3.1.0 "${RELEASE}"\"/" /etc/os-release
echo "Orange Pi 3.1.0 ${RELEASE} \l \n" > /etc/issue
echo "Orange Pi 3.1.0 ${RELEASE}" > /etc/issue.net
fi

# Reload services
systemctl --no-reload enable orangepi-hardware-monitor.service orangepi-hardware-optimize.service orangepi-zram-config.service >/dev/null 2>&1
exit 0

```
```
	chmod 755 /tmp/tmp.7rBWxg3Myx/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64/DEBIAN/postinst
	rsync -a /media/wish/ubuntu/H618/orangepi-build/external/packages/bsp/common/* /tmp/tmp.7rBWxg3Myx/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64
```
>>#### /tmp/tmp.7rBWxg3Myx/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64/DEBIAN/triggers
```
activate update-initramfs
```
>>#### /tmp/tmp.7rBWxg3Myx/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64#etc/orangepi-distribution-status
	done
```
bookworm=supported
buster=supported
focal=supported
stretch=eos
jammy=supported
sid=csc
xenial=eos
bullseye=supported
bionic=supported
raspi=supported
```
>>#### /tmp/tmp.7rBWxg3Myx/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64/etc/orangepi-release
```
# PLEASE DO NOT EDIT THIS FILE
BOARD=orangepizero2
BOARD_NAME="OPI Zero2"
BOARDFAMILY=sun50iw9
BUILD_REPOSITORY_URL=https:#github.com/orangepi-xunlong/orangepi-build.git
BUILD_REPOSITORY_COMMIT=cb65680-dirty
DISTRIBUTION_CODENAME=jammy
DISTRIBUTION_STATUS=supported
VERSION=3.1.0
LINUXFAMILY=sun50iw9
ARCH=arm64
IMAGE_TYPE=user-built
BOARD_TYPE=conf
INITRD_ARCH=arm64
KERNEL_IMAGE_TYPE=Image
BRANCH=next
```
```
	sed -i 's/#no-auto-down/no-auto-down/g' /tmp/tmp.7rBWxg3Myx/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64/etc/network/interfaces.default
	fakeroot dpkg-deb -b -Zxz /tmp/tmp.7rBWxg3Myx/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64 /tmp/tmp.7rBWxg3Myx/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64/.deb"
	mkdir -p /media/wish/ubuntu/H618/orangepi-build/output/debs/jammy/
	rsync --remove-source-files -rq /tmp/tmp.7rBWxg3Myx/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64/.deb /media/wish/ubuntu/H618/orangepi-build/output/debs/jammy/
```
# debootstrap_ng
```
	rm -rf /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-8be688c7-ded2-4c80-9668-088947edc05b /media/wish/ubuntu/H618/orangepi-build/.tmp/mount-8be688c7-ded2-4c80-9668-088947edc05b
	mkdir -p /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-8be688c7-ded2-4c80-9668-088947edc05b /media/wish/ubuntu/H618/orangepi-build/.tmp/mount-8be688c7-ded2-4c80-9668-088947edc05b \
	/media/wish/ubuntu/H618/orangepi-build/output/images /media/wish/ubuntu/H618/orangepi-build/external/cache/rootfs
	mount -t tmpfs -o size=16105M tmpfs /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-8be688c7-ded2-4c80-9668-088947edc05b
	tar ubuntu-22.04.tar.gz -c /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-8be688c7-ded2-4c80-9668-088947edc05b
	mount -t proc chproc /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/proc
	mount -t sysfs chsys /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/sys
	mount -t devtmpfs chdev /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/dev || mount --bind /dev /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/dev
	mount -t devpts chpts /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/dev/pts
	echo "/dev/mmcblk0p1 / ext4 defaults 0 1" >> /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/etc/fstab
	echo "/dev/mmcblk0p2 /usr ext4 defaults 0 2" >> /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/etc/fstab
	tr ' ' '\n' <<< uwe5622_bsp_sdio sprdwl_ng sprdbt_tty > /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/etc/modules
```
>>#### /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/etc/default/cpufrequtils
```
ENABLE=true
MIN_SPEED=480000
MAX_SPEED=1512000
GOVERNOR=ondemand

```

```
	# remove default interfaces file if present
	# before installing board support package
	rm -f /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/etc/network/interfaces

	# disable selinux by default
	mkdir -p /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/selinux
	sed "s/^SELINUX=.*/SELINUX=disabled/" -i /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/etc/selinux/config
	rm /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/etc/legal
	sed -e 's/CHARMAP=".*"/CHARMAP="UTF-8"/g' -i /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/etc/default/console-setup
	echo "HRNGDEVICE=/dev/urandom" >> /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/etc/default/rng-tools

	# ping needs privileged action to be able to create raw network socket
	# this is working properly but not with (at least) Debian Buster
	chroot /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368 /bin/bash -c "chmod u+s /bin/ping"

	# change time zone data
	echo Asia/Shanghai > /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/etc/timezone
	chroot /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368 /bin/bash -c "dpkg-reconfigure -f noninteractive tzdata >/dev/null 2>&1"

	# set root password
	chroot /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368 /bin/bash -c "(echo $ROOTPWD;echo $ROOTPWD;) | passwd root >/dev/null 2>&1"

	# change console welcome text
	echo -e "Orange Pi 3.1.0 Jammy \\l \n" > /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/etc/issue
	echo "Orange Pi 3.1.0 Jammy" > /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/etc/issue.net
	sed -i "s/^PRETTY_NAME=.*/PRETTY_NAME=\"Orange Pi 3.1.0 "Jammy"\"/" /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/etc/os-release

	# enable few bash aliases enabled in Ubuntu by default to make it even
	sed "s/#alias ll='ls -l'/alias ll='ls -l'/" -i /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/etc/skel/.bashrc
	sed "s/#alias la='ls -A'/alias la='ls -A'/" -i /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/etc/skel/.bashrc
	sed "s/#alias l='ls -CF'/alias l='ls -CF'/" -i /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/etc/skel/.bashrc
	# root user is already there. Copy bashrc there as well
	cp /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/etc/skel/.bashrc /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/root

	# display welcome message at first root login
	touch /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/root/.not_logged_in_yet

	# set desktop autologin
	touch /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/root/.desktop_autologin

	cp /media/wish/ubuntu/H618/orangepi-build/external/config/bootscripts/boot-sun50iw9-next.cmd /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/boot/boot.cmd

	cp /media/wish/ubuntu/H618/orangepi-build/external/config/bootenv/sunxi.txt /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/boot/orangepiEnv.txt

	echo "overlay_prefix=$OVERLAY_PREFIX" >> "${SDCARD}"/boot/orangepiEnv.txt
	
	# initial date for fake-hwclock
	date -u '+%Y-%m-%d %H:%M:%S' > "${SDCARD}"/etc/fake-hwclock.data

	echo orangepizero2 > "${SDCARD}"/etc/hostname
```
>>#### "${SDCARD}"/etc/hosts
```
127.0.0.1   localhost
127.0.1.1   orangepizero2
::1         localhost orangepizero2 ip6-localhost ip6-loopback
fe00::0     ip6-localnet
ff00::0     ip6-mcastprefix
ff02::1     ip6-allnodes
ff02::2     ip6-allrouters
```
```
	cd /media/wish/ubuntu/H618/orangepi-build

	# Cleaning package lists
	chroot "${SDCARD}" /bin/bash -c "apt-get clean"

	# Updating package lists
	chroot "${SDCARD}" /bin/bash -c "apt-get update"

	# Temporarily disabling initramfs-tools hook for kernel
	chroot "${SDCARD}" /bin/bash -c "chmod -v -x /etc/kernel/postinst.d/initramfs-tools"

	# install u-boot deb
	cp /media/wish/ubuntu/H618/orangepi-build/output/debs/u-boot/linux-u-boot-next-orangepizero2_3.1.0_arm64.deb /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/root/linux-u-boot-next-orangepizero2_3.1.0_arm64.deb
	chroot "${SDCARD}" /bin/bash -c "DEBIAN_FRONTEND=noninteractive apt-get -yqq --no-install-recommends install /root/linux-u-boot-next-orangepizero2_3.1.0_arm64.deb"

	# install /media/wish/ubuntu/H618/orangepi-build/output/debs/linux-image-next-sun50iw9_3.1.0_arm64.deb
	cp /media/wish/ubuntu/H618/orangepi-build/output/debs/linux-image-next-sun50iw9_3.1.0_arm64.deb /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/root/linux-image-next-sun50iw9_3.1.0_arm64.deb
	chroot "${SDCARD}" /bin/bash -c "DEBIAN_FRONTEND=noninteractive apt-get -yqq --no-install-recommends install /root/linux-image-next-sun50iw9_3.1.0_arm64.deb"

	# install /media/wish/ubuntu/H618/orangepi-build/output/debs/linux-dtb-next-sun50iw9_3.1.0_arm64.deb
	cp /media/wish/ubuntu/H618/orangepi-build/output/debs/linux-dtb-next-sun50iw9_3.1.0_arm64.deb /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/root/linux-dtb-next-sun50iw9_3.1.0_arm64.deb
	chroot "${SDCARD}" /bin/bash -c "DEBIAN_FRONTEND=noninteractive apt-get -yqq --no-install-recommends install /root/linux-dtb-next-sun50iw9_3.1.0_arm64.deb"

	cp /media/wish/ubuntu/H618/orangepi-build/output/debs/linux-headers-next-sun50iw9_3.1.0_arm64.deb /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/opt/

	# install /media/wish/ubuntu/H618/orangepi-build/output/debs/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64.deb
	cp /media/wish/ubuntu/H618/orangepi-build/output/debs/jammy/orangepi-bsp-cli-orangepizero2_3.1.0_arm64.deb /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/root/orangepi-bsp-cli-orangepizero2_3.1.0_arm64.deb
	chroot "${SDCARD}" /bin/bash -c "DEBIAN_FRONTEND=noninteractive apt-get -yqq --no-install-recommends install /root/orangepi-bsp-cli-orangepizero2_3.1.0_arm64.deb"

	# install /media/wish/ubuntu/H618/orangepi-build/output/debs/orangepi-firmware_3.1.0_all.deb
	cp /media/wish/ubuntu/H618/orangepi-build/output/debs/orangepi-firmware_3.1.0_all.deb /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/root/orangepi-firmware_3.1.0_all.deb
	chroot "${SDCARD}" /bin/bash -c "DEBIAN_FRONTEND=noninteractive apt-get -yqq --no-install-recommends install /root/orangepi-firmware_3.1.0_all.deb"

	# install /media/wish/ubuntu/H618/orangepi-build/output/debs/orangepi-config_3.1.0_all.deb
	cp /media/wish/ubuntu/H618/orangepi-build/output/debs/orangepi-config_3.1.0_all.deb /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/root/orangepi-config_3.1.0_all.deb
	chroot "${SDCARD}" /bin/bash -c "DEBIAN_FRONTEND=noninteractive apt-get -yqq --no-install-recommends install /root/orangepi-config_3.1.0_all.deb"

	# install /media/wish/ubuntu/H618/orangepi-build/output/debs/orangepi-zsh_3.1.0_all.deb
	cp /media/wish/ubuntu/H618/orangepi-build/output/debs/orangepi-zsh_3.1.0_all.deb /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/root/orangepi-zsh_3.1.0_all.deb
	chroot "${SDCARD}" /bin/bash -c "DEBIAN_FRONTEND=noninteractive apt-get -yqq --no-install-recommends install /root/orangepi-zsh_3.1.0_all.deb"

	# add orangepi user
	chroot "${SDCARD}" /bin/bash -c "adduser --quiet --disabled-password --shell /bin/bash --home /home/${OPI_USERNAME} --gecos ${OPI_USERNAME} ${OPI_USERNAME}"
	chroot "${SDCARD}" /bin/bash -c "(echo ${OPI_PWD};echo ${OPI_PWD};) | passwd "${OPI_USERNAME}"
	# for additionalgroup in sudo netdev audio video disk tty users games dialout plugdev input bluetooth systemd-journal ssh;
	chroot "${SDCARD}" /bin/bash -c "usermod -aG ${additionalgroup} ${OPI_USERNAME}

	## fix for gksu in Xenial
	touch ${SDCARD}/home/${OPI_USERNAME}/.Xauthority
	chroot "${SDCARD}" /bin/bash -c "chown ${OPI_USERNAME}:${OPI_USERNAME} /home/${OPI_USERNAME}/.Xauthority"
	# set up profile sync daemon on desktop systems
	chroot "${SDCARD}" /bin/bash -c "which psd >/dev/null 2>&1"

	# copy boot splash images
	cp "${EXTER}"/packages/blobs/splash/orangepi-u-boot.bmp "${SDCARD}"/boot/boot.bmp
	cp "${EXTER}"/packages/blobs/splash/logo.bmp "${SDCARD}"/boot/logo.bmp

	# copy audio.wav and mute.wav
	cp "${EXTER}"/packages/blobs/audio_wav/audio.wav "${SDCARD}"/usr/share/sounds/alsa/
	cp "${EXTER}"/packages/blobs/audio_wav/mute.wav "${SDCARD}"/usr/share/sounds/alsa/

	cp "${EXTER}"/packages/blobs/test.mp4 "${SDCARD}"/usr/local/

	# copy watchdog test programm
	cp "${EXTER}"/packages/blobs/watchdog/watchdog_test_${ARCH} "${SDCARD}"/usr/local/bin/watchdog_test

	rsync -a --chown=root:root /media/wish/ubuntu/H618/orangepi-build/external/packages/bsp/overlays_arm64/* /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/
	chroot $SDCARD /bin/bash -c "apt-get -y -qq install rfkill bluetooth bluez bluez-tools ir-keytable tree lirc v4l-utils dnsmasq"

	# install /media/wish/ubuntu/H618/orangepi-build/external/cache/debs/arm64/wiringpi_2.54.deb
	cp /media/wish/ubuntu/H618/orangepi-build/external/cache/debs/arm64/wiringpi_2.54.deb /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/root/wiringpi_2.54.deb
	chroot "${SDCARD}" /bin/bash -c "DEBIAN_FRONTEND=noninteractive apt-get -yqq --no-install-recommends install /root/wiringpi_2.54.deb"
	chroot "${SDCARD}" /bin/bash -c "apt-mark hold wiringpi"
	cp /media/wish/ubuntu/H618/orangepi-build/external/cache/sources/wiringOP/next /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/usr/src/wiringOP -rfa
	cp /media/wish/ubuntu/H618/orangepi-build/external/cache/sources/wiringOP-Python/next /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/usr/src/wiringOP-Python -rfa
	rm $SDCARD/root/*.deb

	# enable additional services
	chroot "${SDCARD}" /bin/bash -c "systemctl --no-reload enable orangepi-firstrun.service >/dev/null 2>&1"
	chroot "${SDCARD}" /bin/bash -c "systemctl --no-reload enable orangepi-firstrun-config.service >/dev/null 2>&1"
	chroot "${SDCARD}" /bin/bash -c "systemctl --no-reload enable orangepi-zram-config.service >/dev/null 2>&1"
	chroot "${SDCARD}" /bin/bash -c "systemctl --no-reload enable orangepi-hardware-optimize.service >/dev/null 2>&1"
	chroot "${SDCARD}" /bin/bash -c "systemctl --no-reload enable orangepi-ramlog.service >/dev/null 2>&1"
	chroot "${SDCARD}" /bin/bash -c "systemctl --no-reload enable orangepi-resize-filesystem.service >/dev/null 2>&1"
	chroot "${SDCARD}" /bin/bash -c "systemctl --no-reload enable orangepi-hardware-monitor.service >/dev/null 2>&1"

	cp /media/wish/ubuntu/H618/orangepi-build/external/packages/bsp/orangepi_first_run.txt.template /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/boot/orangepi_first_run.txt.template

	# Cosmetic fix [FAILED] Failed to start Set console font and keymap at first boot
	sed -i "s/^printf '.*/printf '\%\%G'/g" "${SDCARD}"/etc/console-setup/cached_setup_font.sh
	sed -i "s/^printf '.*/printf '\%\%G'/g" "${SDCARD}"/etc/console-setup/cached_setup_terminal.sh
	sed -i "s/-u/-x'/g" "${SDCARD}"/etc/console-setup/cached_setup_keyboard.sh

	# fix for https://bugs.launchpad.net/ubuntu/+source/blueman/+bug/1542723
	chroot "${SDCARD}" /bin/bash -c "chown root:messagebus /usr/lib/dbus-1.0/dbus-daemon-launch-helper"
	chroot "${SDCARD}" /bin/bash -c "chmod u+s /usr/lib/dbus-1.0/dbus-daemon-launch-helper"

	# disable samba NetBIOS over IP name service requests since it hangs when no network is present at boot
	chroot "${SDCARD}" /bin/bash -c "systemctl --quiet disable nmbd 2> /dev/null"

	# disable low-level kernel messages for non betas
	sed -i "s/^#kernel.printk*/kernel.printk/" "${SDCARD}"/etc/sysctl.conf

	# disable repeated messages due to xconsole not being installed.
	sed '/daemon\.\*\;mail.*/,/xconsole/ s/.*/#&/' -i "${SDCARD}"/etc/rsyslog.d/50-default.conf

	# disable deprecated parameter
	sed '/.*$KLogPermitNonKernelFacility.*/,// s/.*/#&/' -i "${SDCARD}"/etc/rsyslog.conf

	# enable getty on multiple serial consoles
	# and adjust the speed if it is defined and different than 115200
	#
	# example: SERIALCON="ttyS0:15000000,ttyGS1"
	#
	echo 'ttyS0' >>  "${SDCARD}"/etc/securetty
	chroot "${SDCARD}" /bin/bash -c "systemctl daemon-reload"
	chroot "${SDCARD}" /bin/bash -c "systemctl --no-reload enable serial-getty@'ttyS0'.service"

	# install initial asound.state if defined
	mkdir -p "${SDCARD}"/var/lib/alsa/
	cp /media/wish/ubuntu/H618/orangepi-build/external/packages/blobs/asound.state/asound.state.sun50iw9-next /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/var/lib/alsa/asound.state

	# save initial orangepi-release state
	cp "${SDCARD}"/etc/orangepi-release "${SDCARD}"/etc/orangepi-image-release

	# permit root login via SSH for the first boot
	sed -i 's/#\?PermitRootLogin .*/PermitRootLogin yes/' "${SDCARD}"/etc/ssh/sshd_config

	# enable PubkeyAuthentication
	sed -i 's/#\?PubkeyAuthentication .*/PubkeyAuthentication yes/' "${SDCARD}"/etc/ssh/sshd_config

	# configure network manager
	sed "s/managed=\(.*\)/managed=true/g" -i "${SDCARD}"/etc/NetworkManager/NetworkManager.conf

	# remove network manager defaults to handle eth by default
	rm -f "${SDCARD}"/usr/lib/NetworkManager/conf.d/10-globally-managed-devices.conf

	# most likely we don't need to wait for nm to get online
	chroot "${SDCARD}" /bin/bash -c "systemctl disable NetworkManager-wait-online.service"

	# Just regular DNS and maintain /etc/resolv.conf as a file
	sed "/dns/d" -i "${SDCARD}"/etc/NetworkManager/NetworkManager.conf
	sed "s/\[main\]/\[main\]\ndns=default\nrc-manager=file/g" -i "${SDCARD}"/etc/NetworkManager/NetworkManager.conf

	# nsswitch settings for sane DNS behavior: remove resolve, assure libnss-myhostname support
	sed "s/hosts\:.*/hosts:          files mymachines dns myhostname/g" -i "${SDCARD}"/etc/nsswitch.conf
```
>>#### build logo in any case
```
	LOGO=${EXTER}/packages/blobs/splash/logo.png
	LOGO_WIDTH=$(identify $LOGO | cut -d " " -f 3 | cut -d x -f 1)
	LOGO_HEIGHT=$(identify $LOGO | cut -d " " -f 3 | cut -d x -f 2)
	THROBBER=${EXTER}/packages/blobs/splash/spinner.gif
	THROBBER_WIDTH=$(identify $THROBBER | head -1 | cut -d " " -f 3 | cut -d x -f 1)
	THROBBER_HEIGHT=$(identify $THROBBER | head -1 | cut -d " " -f 3 | cut -d x -f 2)

	convert -alpha remove -background "#000000" /media/wish/ubuntu/H618/orangepi-build/external/packages/blobs/splash/logo.png /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/tmp/logo.rgb
	convert -alpha remove -background "#000000" /media/wish/ubuntu/H618/orangepi-build/external/packages/blobs/splash/spinner.gif /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/tmp/throbber%02d.rgb

	${EXTER}/packages/blobs/splash/bootsplash-packer \
	--bg_red 0x00 \
	--bg_green 0x00 \
	--bg_blue 0x00 \
	--frame_ms 48 \
	--picture \
	--pic_width $LOGO_WIDTH \
	--pic_height $LOGO_HEIGHT \
	--pic_position 0 \
	--blob "${SDCARD}"/tmp/logo.rgb \
	--picture \
	--pic_width $THROBBER_WIDTH \
	--pic_height $THROBBER_HEIGHT \
	--pic_position 0x05 \
	--pic_position_offset 200 \
	--pic_anim_type 1 \
	--pic_anim_loop 0 \
	--blob "${SDCARD}"/tmp/throbber00.rgb \
	--blob "${SDCARD}"/tmp/throbber01.rgb \
	--blob "${SDCARD}"/tmp/throbber02.rgb \
	--blob "${SDCARD}"/tmp/throbber03.rgb \
	--blob "${SDCARD}"/tmp/throbber04.rgb \
	--blob "${SDCARD}"/tmp/throbber05.rgb \
	--blob "${SDCARD}"/tmp/throbber06.rgb \
	--blob "${SDCARD}"/tmp/throbber07.rgb \
	--blob "${SDCARD}"/tmp/throbber08.rgb \
	--blob "${SDCARD}"/tmp/throbber09.rgb \
	--blob "${SDCARD}"/tmp/throbber10.rgb \
	--blob "${SDCARD}"/tmp/throbber11.rgb \
	--blob "${SDCARD}"/tmp/throbber12.rgb \
	--blob "${SDCARD}"/tmp/throbber13.rgb \
	--blob "${SDCARD}"/tmp/throbber14.rgb \
	--blob "${SDCARD}"/tmp/throbber15.rgb \
	--blob "${SDCARD}"/tmp/throbber16.rgb \
	--blob "${SDCARD}"/tmp/throbber17.rgb \
	--blob "${SDCARD}"/tmp/throbber18.rgb \
	--blob "${SDCARD}"/tmp/throbber19.rgb \
	--blob "${SDCARD}"/tmp/throbber20.rgb \
	--blob "${SDCARD}"/tmp/throbber21.rgb \
	--blob "${SDCARD}"/tmp/throbber22.rgb \
	--blob "${SDCARD}"/tmp/throbber23.rgb \
	--blob "${SDCARD}"/tmp/throbber24.rgb \
	--blob "${SDCARD}"/tmp/throbber25.rgb \
	--blob "${SDCARD}"/tmp/throbber26.rgb \
	--blob "${SDCARD}"/tmp/throbber27.rgb \
	--blob "${SDCARD}"/tmp/throbber28.rgb \
	--blob "${SDCARD}"/tmp/throbber29.rgb \
	--blob "${SDCARD}"/tmp/throbber30.rgb \
	--blob "${SDCARD}"/tmp/throbber31.rgb \
	--blob "${SDCARD}"/tmp/throbber32.rgb \
	--blob "${SDCARD}"/tmp/throbber33.rgb \
	--blob "${SDCARD}"/tmp/throbber34.rgb \
	--blob "${SDCARD}"/tmp/throbber35.rgb \
	--blob "${SDCARD}"/tmp/throbber36.rgb \
	--blob "${SDCARD}"/tmp/throbber37.rgb \
	--blob "${SDCARD}"/tmp/throbber38.rgb \
	--blob "${SDCARD}"/tmp/throbber39.rgb \
	--blob "${SDCARD}"/tmp/throbber40.rgb \
	--blob "${SDCARD}"/tmp/throbber41.rgb \
	--blob "${SDCARD}"/tmp/throbber42.rgb \
	--blob "${SDCARD}"/tmp/throbber43.rgb \
	--blob "${SDCARD}"/tmp/throbber44.rgb \
	--blob "${SDCARD}"/tmp/throbber45.rgb \
	--blob "${SDCARD}"/tmp/throbber46.rgb \
	--blob "${SDCARD}"/tmp/throbber47.rgb \
	--blob "${SDCARD}"/tmp/throbber48.rgb \
	--blob "${SDCARD}"/tmp/throbber49.rgb \
	--blob "${SDCARD}"/tmp/throbber50.rgb \
	--blob "${SDCARD}"/tmp/throbber51.rgb \
	--blob "${SDCARD}"/tmp/throbber52.rgb \
	--blob "${SDCARD}"/tmp/throbber53.rgb \
	--blob "${SDCARD}"/tmp/throbber54.rgb \
	--blob "${SDCARD}"/tmp/throbber55.rgb \
	--blob "${SDCARD}"/tmp/throbber56.rgb \
	--blob "${SDCARD}"/tmp/throbber57.rgb \
	--blob "${SDCARD}"/tmp/throbber58.rgb \
	--blob "${SDCARD}"/tmp/throbber59.rgb \
	--blob "${SDCARD}"/tmp/throbber60.rgb \
	--blob "${SDCARD}"/tmp/throbber61.rgb \
	--blob "${SDCARD}"/tmp/throbber62.rgb \
	--blob "${SDCARD}"/tmp/throbber63.rgb \
	--blob "${SDCARD}"/tmp/throbber64.rgb \
	--blob "${SDCARD}"/tmp/throbber65.rgb \
	--blob "${SDCARD}"/tmp/throbber66.rgb \
	--blob "${SDCARD}"/tmp/throbber67.rgb \
	--blob "${SDCARD}"/tmp/throbber68.rgb \
	--blob "${SDCARD}"/tmp/throbber69.rgb \
	--blob "${SDCARD}"/tmp/throbber70.rgb \
	--blob "${SDCARD}"/tmp/throbber71.rgb \
	--blob "${SDCARD}"/tmp/throbber72.rgb \
	--blob "${SDCARD}"/tmp/throbber73.rgb \
	--blob "${SDCARD}"/tmp/throbber74.rgb \
	"${SDCARD}"/lib/firmware/bootsplash.orangepi

	# enable additional services
	chroot "${SDCARD}" /bin/bash -c "systemctl --no-reload enable bootsplash-ask-password-console.path"
	chroot "${SDCARD}" /bin/bash -c "systemctl --no-reload enable bootsplash-hide-when-booted.service"
	chroot "${SDCARD}" /bin/bash -c "systemctl --no-reload enable bootsplash-show-on-shutdown.service"
```
```
	# disable MOTD for first boot - we want as clean 1st run as possible
	chmod -x "${SDCARD}"/etc/update-motd.d/*
```
>>#### # install locally built packages or install pre-built packages from orangepi
```
	rm -rf /tmp/aptly-temp/
	mkdir -p /tmp/aptly-temp/
	aptly -config=/media/wish/ubuntu/H618/orangepi-build/external/config/aptly-temp.conf repo create temp

	# NOTE: this works recursively
	aptly -config=/media/wish/ubuntu/H618/orangepi-build/external/config/aptly-temp.conf repo add temp /media/wish/ubuntu/H618/orangepi-build/external/cache/debs/extra/jammy-desktop/
	aptly -config=/media/wish/ubuntu/H618/orangepi-build/external/config/aptly-temp.conf repo add temp /media/wish/ubuntu/H618/orangepi-build/external/cache/debs/extra/jammy-utils/

	# -gpg-key="925644A6"
	aptly -keyring="$EXTER/packages/extras-buildpkgs/buildpkg-public.gpg" -secret-keyring="$EXTER/packages/extras-buildpkgs/buildpkg.gpg" -batch=true -config="${conf}" \
		 -gpg-key="925644A6" -passphrase="testkey1234" -component=temp -distribution=jammy publish repo temp
	aptly -config="${conf}" -listen=":8189" serve
	cp $EXTER/packages/extras-buildpkgs/buildpkg.key "${SDCARD}"/tmp/buildpkg.key
```
>>#### "${SDCARD}"/etc/apt/preferences.d/90-orangepi-temp.pref
```
Package: *
Pin: origin "localhost"
Pin-Priority: 550
```
>>#### "${SDCARD}"/etc/apt/sources.list.d/orangepi-temp.list
```
deb http://localhost:8189/ jammy temp

```
>>#### customize image
```
	cp /media/wish/ubuntu/H618/orangepi-build/userpatches/customize-image.sh /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/tmp/customize-image.sh
	chmod +x "${SDCARD}"/tmp/customize-image.sh
	mkdir -p "${SDCARD}"/tmp/overlay

	# util-linux >= 2.27 required
	mount -o bind,ro /media/wish/ubuntu/H618/orangepi-build/userpatches/overlay "${SDCARD}"/tmp/overlay
	chroot "${SDCARD}" /bin/bash -c "/tmp/customize-image.sh jammy sun50iw9 orangepizero2 no arm64"
	umount -i "${SDCARD}"/tmp/overlay
	mountpoint -q "${SDCARD}"/tmp/overlay || rm -r "${SDCARD}"/tmp/overlay
```
```
	# remove packages that are no longer needed. Since we have intrudoced uninstall feature, we might want to clean things that are no longer needed
	chroot $SDCARD /bin/bash -c "apt-get autoremove -y"

	# create list of installed packages for debug purposes
	chroot $SDCARD /bin/bash -c "dpkg --get-selections" | grep -v deinstall | awk '{print $1}' | cut -f1 -d':'

	# clean up / prepare for making the image
	umount -l --recursive "${SDCARD}"/dev
	umount -l "${SDCARD}"/proc
	umount -l "${SDCARD}"/sys
	sleep 5

	# remove service start blockers and QEMU binary
	rm -f "${SDCARD}"/sbin/initctl "${SDCARD}"/sbin/start-stop-daemon
	chroot "${SDCARD}" /bin/bash -c "dpkg-divert --quiet --local --rename --remove /sbin/initctl"
	chroot "${SDCARD}" /bin/bash -c "dpkg-divert --quiet --local --rename --remove /sbin/start-stop-daemon"
	rm -f "${SDCARD}"/usr/sbin/policy-rc.d "${SDCARD}/usr/bin/${QEMU_BINARY}"
```
>>#### prepare_partitions
```
	dd if=/dev/zero bs=1M status=none count=2052 | pv -p -b -r -s $((2052 * 1024 * 1024)) -N "[ .... ] dd" | dd status=none of=${SDCARD}.raw
	# stage: mount image
	# lock access to loop devices
	exec {FD}> /var/lock/orangepi-debootstrap-losetup
	flock -x $FD # 12
	losetup $LOOP ${SDCARD}.raw

	# loop device was grabbed here, unlock
	flock -u $FD

	partprobe $LOOP

	# stage: create fs, mount partitions, create fstab
	rm -f $SDCARD/etc/fstab
	mkfs.${mkfs[$ROOTFS_TYPE]} ${mkopts[$ROOTFS_TYPE]} ${mkopts_label[$ROOTFS_TYPE]:+${mkopts_label[$ROOTFS_TYPE]}"$ROOT_FS_LABEL"} $rootdevice # mkfs.ext4 -q -m 2 -O ^64bit,^metadata_csum -L opi_root /dev/loop33p1
	tune2fs -o journal_data_writeback $rootdevice
	mount ${fscreateopt} $rootdevice $MOUNT/ # mount /dev/loop33p1 /media/wish/ubuntu/H618/orangepi-build/.tmp/mount-9ccf99d5-a6f1-4722-8354-51c958835368/
	echo "$rootfs / ${mkfs[$ROOTFS_TYPE]} defaults,noatime${mountopts[$ROOTFS_TYPE]} 0 1" >> $SDCARD/etc/fstab # echo "UUID=541d6852-4510-48b3-8a16-54343db5b8e8 / ext4 defaults,noatime,commit=600,errors=remount-ro" >> $SDCARD/etc/fstab

	echo "tmpfs /tmp tmpfs defaults,nosuid 0 0" >> $SDCARD/etc/fstab
	echo "rootdev=$rootfs" >> $SDCARD/boot/orangepiEnv.txt # rootdev=UUID=541d6852-4510-48b3-8a16-54343db5b8e8
	echo "rootfstype=$ROOTFS_TYPE" >> $SDCARD/boot/orangepiEnv.txt # rootfstype=ext4
	mkimage -C none -A arm -T script -d $SDCARD/boot/boot.cmd $SDCARD/boot/$BOOTSCRIPT_OUTPUT # /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/boot/boot.cmd /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368/boot/boot.scr
```
>>#### create image
```
	rm -rf $destimg # $destimg : /media/wish/ubuntu/H618/orangepi-build/output/images/Orangepizero2_3.1.0_ubuntu_jammy_server_linux6.1.31
	mkdir -p $destimg
	rsync -aHWXh \
			  --exclude="/boot/*" \
			  --exclude="/dev/*" \
			  --exclude="/proc/*" \
			  --exclude="/run/*" \
			  --exclude="/tmp/*" \
			  --exclude="/sys/*" \
			  --info=progress0,stats1 $SDCARD/ $MOUNT/
	# $MOUNT/ : /media/wish/ubuntu/H618/orangepi-build/.tmp/mount-9ccf99d5-a6f1-4722-8354-51c958835368/

	cp /usr/bin/$QEMU_BINARY $chroot_target/usr/bin/ # cp /usr/bin/qemu-aarch64-static /media/wish/ubuntu/H618/orangepi-build/.tmp/mount-9ccf99d5-a6f1-4722-8354-51c958835368/usr/bin/

	mount -t proc chproc /media/wish/ubuntu/H618/orangepi-build/.tmp/mount-9ccf99d5-a6f1-4722-8354-51c958835368//proc
	mount -t sysfs chsys /media/wish/ubuntu/H618/orangepi-build/.tmp/mount-9ccf99d5-a6f1-4722-8354-51c958835368//sys
	mount -t devtmpfs chdev /media/wish/ubuntu/H618/orangepi-build/.tmp/mount-9ccf99d5-a6f1-4722-8354-51c958835368//dev || mount --bind /dev /media/wish/ubuntu/H618/orangepi-build/.tmp/mount-9ccf99d5-a6f1-4722-8354-51c958835368//dev
	mount -t devpts chpts /media/wish/ubuntu/H618/orangepi-build/.tmp/mount-9ccf99d5-a6f1-4722-8354-51c958835368//dev/pts

	chroot /media/wish/ubuntu/H618/orangepi-build/.tmp/mount-9ccf99d5-a6f1-4722-8354-51c958835368 /bin/bash -c update-initramfs -uv -k 6.1.31-sun50iw9
	chroot $chroot_target /bin/bash -c "chmod -v +x /etc/kernel/postinst.d/initramfs-tools"

	umount -l --recursive /media/wish/ubuntu/H618/orangepi-build/.tmp/mount-9ccf99d5-a6f1-4722-8354-51c958835368//dev >/dev/null 2>&1
	umount -l /media/wish/ubuntu/H618/orangepi-build/.tmp/mount-9ccf99d5-a6f1-4722-8354-51c958835368//proc >/dev/null 2>&1
	umount -l /media/wish/ubuntu/H618/orangepi-build/.tmp/mount-9ccf99d5-a6f1-4722-8354-51c958835368//sys >/dev/null 2>&1
	sleep 5

	rm $chroot_target/usr/bin/$QEMU_BINARY # rm /media/wish/ubuntu/H618/orangepi-build/.tmp/mount-9ccf99d5-a6f1-4722-8354-51c958835368/usr/bin/qemu-aarch64-static

	dpkg -x "${DEB_STORAGE}/u-boot/${CHOSEN_UBOOT}_${revision}_${ARCH}.deb" ${TEMP_DIR}/ # dpkg -x /media/wish/ubuntu/H618/orangepi-build/output/debs/u-boot/linux-u-boot-next-orangepizero2_3.1.0_arm64.deb /tmp/tmp.XZ7uWR2X44/

	dd if=/dev/zero of=/dev/loop33 bs=1k count=1023 seek=1 status=noxfer
	dd if=/tmp/tmp.XZ7uWR2X44/usr/lib/linux-u-boot-next-orangepizero2_3.1.0_arm64/u-boot-sunxi-with-spl.bin of=/dev/loop33 bs=1k seek=8 conv=fsync
	chmod 755 $MOUNT # $MOUNT : /media/wish/ubuntu/H618/orangepi-build/.tmp/mount-9ccf99d5-a6f1-4722-8354-51c958835368

	# unmount /boot/efi first, then /boot, rootfs third, image file last
	sync
	umount -l $MOUNT

	losetup -d $LOOP # $LOOP : /dev/loop33
	rm -rf --one-file-system $DESTIMG $MOUNT # $DESTIMG $MOUNT : /media/wish/ubuntu/H618/orangepi-build/.tmp/image-9ccf99d5-a6f1-4722-8354-51c958835368 /media/wish/ubuntu/H618/orangepi-build/.tmp/mount-9ccf99d5-a6f1-4722-8354-51c958835368

	mkdir -p $DESTIMG
	mv ${SDCARD}.raw $DESTIMG/${version}.img # mv /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368.raw /media/wish/ubuntu/H618/orangepi-build/.tmp/image-9ccf99d5-a6f1-4722-8354-51c958835368/Orangepizero2_3.1.0_ubuntu_jammy_server_linux6.1.31.img

	cd ${DESTIMG} # cd /media/wish/ubuntu/H618/orangepi-build/.tmp/image-9ccf99d5-a6f1-4722-8354-51c958835368
	sha256sum -b ${version}.img${compression_type} > ${version}.img${compression_type}.sha # sha256sum -b  Orangepizero2_3.1.0_ubuntu_jammy_server_linux6.1.31.img > Orangepizero2_3.1.0_ubuntu_jammy_server_linux6.1.31.img.sha

	cd ${DESTIMG} # cd /media/wish/ubuntu/H618/orangepi-build/.tmp/image-9ccf99d5-a6f1-4722-8354-51c958835368

	mv $DESTIMG/${version}* ${FINALDEST} # mv /media/wish/ubuntu/H618/orangepi-build/.tmp/image-9ccf99d5-a6f1-4722-8354-51c958835368/Orangepizero2_3.1.0_ubuntu_jammy_server_linux6.1.31* /media/wish/ubuntu/H618/orangepi-build/output/images/Orangepizero2_3.1.0_ubuntu_jammy_server_linux6.1.31
	rm -rf $DESTIMG # rm -rf /media/wish/ubuntu/H618/orangepi-build/.tmp/image-9ccf99d5-a6f1-4722-8354-51c958835368

	umount $SDCARD # umount /media/wish/ubuntu/H618/orangepi-build/.tmp/rootfs-9ccf99d5-a6f1-4722-8354-51c958835368
	rm -rf $SDCARD

```