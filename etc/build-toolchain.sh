#!/bin/bash -xv
#===================================================================
# Build Toolchain
#===================================================================
#
# License     :	Public Domain
# Author      :	Robert Umbehant <rumbehant@wheresjames.com>
# References  :	
#              	http://frank.harvard.edu/~coldwell/toolchain/
#              	http://sung2ne.tistory.com/entry/Building-a-GNULinux-ARM-Toolchain-from-scratch
#              	http://wiki.davincidsp.com/index.php?title=Linux_Toolchain
#				http://cross-lfs.org/view/clfs-sysroot/arm/
#			   	http://ftp.snapgear.org/pub/snapgear/tools/arm-linux/build-arm-linux-3.4.4
#				http://gmplib.org/manual/Build-Options.html
#				http://aakash-bapna.blogspot.com/2007/10/iphone-non-official-sdk-aka-toolchain.html
#				http://code.google.com/p/iphone-dev/wiki/Building
#
#				http://armel.applieddata.net/developers/linux/eabi/
#				http://ixp2xxx.sourceforge.net/toolchain/toolchain.html
#				http://www.linuxfromscratch.org/clfs/view/clfs-2.0/arm/part3.html
#
#     DON'T expect this script to work perfectly for you.
#     DO Expect to tweak things a bit to get a build on your box.
#
# If you use this script correctly, it will install your patches
# and linux config.  
#    Setup you're directory to match what you see below.  Set 
# CFG_LINUX to the name of your config file.  For the example below, 
# CFG_LINUX=<my_linux_config>  The actual file must begin with 
# linux-cfg-  It will search for and apply all patches matching the 
# version striong.  If VER_GCC=gcc-3.4.4, all patches matching 
# gcc-3.4.4-<whatev>.patch will be applied.
#
# +---- [downloads]  (Downloaded files will be copied here)
# |
# +---- [tools]      (Toolchain will be built here)
# |
# +---- [etc]        (Put the script and patches here, the directory name is not important)
#       |
#       +---- build-toolchain.sh
#       |
#       +---- [patches]
#             |
#             +---- <lib>-my_patch.patch
#             |
#             +---- linux-cfg-<my_linux_config>
#

#-------------------------------------------------------------------
# Setup
#-------------------------------------------------------------------

#************************************************
# An old version 
#************************************************
#TARGET=arm-unknown-linux-gnu
#VER_BINUTILS=binutils-2.16
#VER_GCC=gcc-3.4.4
#VER_GLIBC=glibc-2.3.5
#VER_GLIBC_THREADS=glibc-linuxthreads-2.3.5
#VER_LINUX=linux-2.6.10
#DIR_LINUX=v2.6
#CFG_LINUX=custom_davinci_all_defconfig
#ENABLE_ADD_ONS=linuxthreads

#************************************************
# Recent version 
#************************************************
#TARGET=arm-none-linux-gnueabi
#VER_BINUTILS=binutils-2.19
#VER_GCC=gcc-4.2.4
#VER_GLIBC=glibc-2.7
#VER_GLIBC_PORTS=glibc-ports-2.7
#VER_GLIBC_THREADS=glibc-linuxthreads-2.5
#VER_LINUX=linux-2.6.28
#DIR_LINUX=v2.6
#CFG_LINUX=custom_davinci_all_defconfig
#ENABLE_ADD_ONS=nptl


#************************************************
# iPhone build
#************************************************
#arm-apple-darwin

#************************************************
# MontaVista build
#************************************************
# --enable-kernel=2.6.10
#
# armv5tl-montavista-linux-gnueabi - gcc-4.2.0 - glibc-2.6
# -mabi=aapcs-linux -mfloat-abi=soft -meabi=4
#
# linuxthreads = --with-tls --without-__thread
#

# [Works] - Old mainline
#TARGET=arm-none-linux-gnu
#VER_BINUTILS=binutils-2.16
#VER_GCC=gcc-3.4.3
#VER_GLIBC=glibc-2.3.5
#VER_GLIBC_THREADS=glibc-linuxthreads-2.3.5
#VER_LINUX=linux-2.6.10
#DIR_LINUX=v2.6
#ENABLE_ADD_ONS_S1=linuxthreads
#ENABLE_ADD_ONS_S2=linuxthreads

# [Works] - Montavista proprietary
#TARGET=arm-none-linux-gnu
#VER_BINUTILS=binutils-2.16
#VER_GCC=gcc-3.4.3
#VER_GLIBC=glibc-2.3.5
#VER_GLIBC_THREADS=glibc-linuxthreads-2.3.5
#VER_LINUX=linux-2.6.10-davinci
#ENABLE_ADD_ONS_S1=linuxthreads
#ENABLE_ADD_ONS_S2=linuxthreads
#MACH=davinci
#TOOL_ALIAS=arm_v5t_le

# [] - Montavista proprietary with eabi
#TARGET=armv5tl-montavista-linuxeabi
TARGET=arm-unknown-linux-gnueabi
VER_BINUTILS=binutils-2.19
VER_GCC=gcc-4.2.4
VER_GMP=gmp-4.2.4
VER_GLIBC=glibc-2.7
VER_GLIBC_PORTS=glibc-ports-2.7
#VER_GLIBC_THREADS=glibc-linuxthreads-2.5
#VER_LINUX=linux-2.6.10-davinci
VER_LINUX=linux-2.6.25-davinci
GLIBC_EXTRA_S1= --with-tls --with-__thread
ENABLE_ADD_ONS_S1=ports,nptl
GLIBC_EXTRA_S2= --with-tls --with-__thread
ENABLE_ADD_ONS_S2=ports,nptl
MACH=davinci
TOOL_ALIAS=arm_v5t_le
#CFG_LINUX=custom_davinci_all_defconfig
#CFG_LINUX=davinci_evm_dm644x_defconfig

# [] Mainline
#TARGET=arm-none-linux-gnueabi
#VER_BINUTILS=binutils-2.19
#VER_GCC=gcc-4.2.4
#VER_GMP=gmp-4.2.4
#VER_GLIBC=glibc-2.7
#VER_GLIBC_PORTS=glibc-ports-2.7
#VER_GLIBC_THREADS=glibc-linuxthreads-2.5
#VER_LINUX=linux-2.6.28
#DIR_LINUX=v2.6
#GLIBC_EXTRA= --with-tls --disable-sanity-checks
#CFG_LINUX=custom_davinci_all_defconfig

# [] Montavista Open Source
#TARGET=arm-none-linux-gnueabi
#VER_BINUTILS=binutils-2.17
#VER_GCC=gcc-4.1.0
#VER_GCC=gcc-4.1.1
#VER_GLIBC=glibc-2.5
#VER_GLIBC_PORTS=glibc-ports-2.5
#VER_GLIBC_THREADS=glibc-linuxthreads-2.5
#GLIBC_EXTRA= --with-tls --disable-sanity-checks
#VER_LINUX=linux-davinci-2.6
#DIR_LINUX=v2.6
#CFG_LINUX=davinci_all_defconfig
#CFG_LINUX=custom_eabi_defconfig
#GIT_REPOS="git://source.mvista.com/git/linux-davinci-2.6.git"
#GIT_CHECKOUT=v2.6.28-davinci1
#MACH=davinci
#GLIBC_EXTRA_S1= --without-tls --without-__thread --disable-sanity-checks
#ENABLE_ADD_ONS_S1=ports,linuxthreads
#GLIBC_EXTRA_S@= --with-tls --with-__thread
#ENABLE_ADD_ONS_S2=ports,nptl


#-------------------------------------------------------------------

BUILD=i686-pc-linux-gnu
HOST=i686-pc-linux-gnu
#BUILD=i486-linux-gnu
#HOST=i486-linux-gnu
export ARCH=arm
SCRIPT_DIR=$PWD
PATCHES=$PWD/patches
PREFIX=$PWD/../../../tools/${TARGET}
DOWNLOADS=$PWD/../../../downloads
SYSROOT=${PREFIX}/sysroot
BUILDROOT=${PREFIX}/src
#export CROSS_COMPILE=arm_v5t_le-
export CROSS_COMPILE=${PREFIX}/bin/${TARGET}-
export PATH=$PATH:${PREFIX}/bin
mkdir -p ${BUILDROOT}
mkdir -p ${BUILDROOT}/logs
mkdir -p ${DOWNLOADS}
mkdir -p ${SYSROOT}/usr/include
mkdir -p ${SYSROOT}/usr/include/asm

# Set to 1 to verify signatures, (you must have the public keys)
VERIFY_SIGS=0

#-------------------------------------------------------------------
# Get the sources
#-------------------------------------------------------------------

cd ${BUILDROOT}
for URL in \
	"http://ftp.gnu.org/gnu/binutils/${VER_BINUTILS}.tar.bz2" \
	"http://ftp.gnu.org/gnu/gcc/${VER_GCC}/${VER_GCC}.tar.bz2" \
	"http://ftp.gnu.org/gnu/gmp/${VER_GMP}.tar.bz2" \
	"http://ftp.gnu.org/gnu/glibc/${VER_GLIBC}.tar.bz2" \
	"http://ftp.gnu.org/gnu/glibc/${VER_GLIBC_PORTS}.tar.bz2" \
	"http://ftp.gnu.org/gnu/glibc/${VER_GLIBC_THREADS}.tar.bz2" \
	"http://ftp.kernel.org/pub/linux/kernel/${DIR_LINUX}/${VER_LINUX}.tar.bz2"
do
    FILE=${URL##*/}
    FILE=${DOWNLOADS}/${FILE%%\?*}
    echo Downloading ${FILE}
    [ -f ${FILE} ] || wget -O ${FILE} ${URL}
    [ -s ${FILE} ] || rm ${FILE}
    
    if [ ${VERIFY_SIGS} -gt 0 ]; then
		[ -s ${FILE}.sig ] || wget -O ${FILE}.sig ${URL}.sig
		[ -s ${FILE}.sig ] || wget -O ${FILE}.sig ${URL}.sign
		gpg --verify ${FILE}.sig ${FILE}
		if [ $? -ne 0 ]; then
			exit 1
		fi    
	fi
done

for URL in ${GIT_REPOS}
do
    FILE=${URL##*/}
    FILE=${FILE%.*}
    FILE=${FILE%%\?*}
    echo Getting ${FILE}
    if [ ! -d ${BUILDROOT}/${FILE} ]; then        
		[ -d ${DOWNLOADS}/${FILE} ] || git-clone ${URL} ${DOWNLOADS}/${FILE}
		cp -r ${DOWNLOADS}/${FILE} ${BUILDROOT}/${FILE}		
	fi
done

#-------------------------------------------------------------------
# 1. GNU binutils
#-------------------------------------------------------------------
cd ${BUILDROOT}
if [ ! -d ${VER_BINUTILS} ]; then 
	bunzip2 -c ${DOWNLOADS}/${VER_BINUTILS}.tar.bz2 | tar xvf -
	find ${PATCHES} -name ${VER_BINUTILS}-*.patch | xargs -rtI {} cat {} | patch -d ${VER_BINUTILS} -p1
	mkdir -p BUILD/${VER_BINUTILS}
	cd BUILD/${VER_BINUTILS}
	../../${VER_BINUTILS}/configure --prefix=${PREFIX} \
		                            --target=${TARGET} \
		                            --disable-nls \
		                            --enable-shared \
		                            --disable-multilib \
	                                --with-sysroot=${SYSROOT} \
	                                2>&1 | tee -a ${BUILDROOT}/logs/10.${VER_BINUTILS}.configure.log
	make configure-host 2>&1 | tee -a ${BUILDROOT}/logs/11.${VER_BINUTILS}.make.configure-host.log
	make 2>&1 | tee -a ${BUILDROOT}/logs/12.${VER_BINUTILS}.make.log
	make install 2>&1 | tee -a ${BUILDROOT}/logs/12.${VER_BINUTILS}.make.install.log
	cp -v include/libiberty.h ${SYSROOT}/usr/include
	[ -z "${TOOL_ALIAS}" ] || sh ${SCRIPT_DIR}/link-tools.sh ${PREFIX}/bin/${TARGET} ${PREFIX}/bin/${TOOL_ALIAS} 2>&1 | tee -a ${BUILDROOT}/logs/13.${VER_BINUTILS}.link-gcc.sh.log
fi

#-------------------------------------------------------------------
# 2. Linux Kernel Headers
#-------------------------------------------------------------------
cd ${BUILDROOT}
[ ! -d ${DOWNLOADS}/${VER_LINUX} ] || cp -r ${DOWNLOADS}/${VER_LINUX} ${VER_LINUX}
[ -d ${VER_LINUX} ] || bunzip2 -c ${DOWNLOADS}/${VER_LINUX}.tar.bz2 | tar xvf -
ln -s ${VER_LINUX} linux
cd ${BUILDROOT}/linux
[ -z "${GIT_CHECKOUT}" ] || git checkout ${GIT_CHECKOUT}
find ${PATCHES} -name ${VER_LINUX}-*.patch | xargs -rtI {} cat {} | patch -d ${BUILDROOT}/linux -p1
[ -f ${PATCHES}/linux-cfg-${CFG_LINUX} ] && cp -a ${PATCHES}/linux-cfg-${CFG_LINUX} ${BUILDROOT}/linux/arch/${ARCH}/configs/${CFG_LINUX}
[ -z "${CFG_LINUX}" ] || make ${CFG_LINUX} 2>&1 | tee -a ${BUILDROOT}/logs/20.${VER_LINUX}.make.${CFG_LINUX}.log
#make dep 2>&1 | tee -a ${BUILDROOT}/logs/21.${VER_LINUX}.make.dep.log
make include/linux/version.h 2>&1 | tee -a ${BUILDROOT}/logs/22.${VER_LINUX}.make.version.h.log
#make symlinks 2>&1 | tee -a ${BUILDROOT}/logs/23.${VER_LINUX}.make.symlinks.log
install -dv ${SYSROOT}/usr/include 2>&1 | tee -a ${BUILDROOT}/logs/24.${VER_LINUX}.install.sysroot.log

#cp -av include/{asm-generic,linux,mtd,scsi,sound} ${SYSROOT}/usr/include
#cp -av include/asm-${ARCH} ${SYSROOT}/usr/include/asm


cp -a ${PREFIX}/src/linux/include/linux ${SYSROOT}/usr/include/linux
#cp -a ${PREFIX}/src/linux/include/asm ${SYSROOT}/usr/include/asm

cp -a ${PREFIX}/src/linux/include/asm-generic ${SYSROOT}/usr/include/asm-generic
cp -a ${PREFIX}/src/linux/include/asm-${ARCH}/* ${SYSROOT}/usr/include/asm

cp -af ${PREFIX}/src/linux/arch/${ARCH}/include/asm/* ${SYSROOT}/usr/include/asm
[ ${MACH} = "" ] || cp -af ${PREFIX}/src/linux/arch/${ARCH}/mach-${MACH}/include/mach/* ${SYSROOT}/usr/include/asm

touch ${SYSROOT}/usr/include/linux/autoconf.h

#-------------------------------------------------------------------
# 3. Glibc headers
#-------------------------------------------------------------------
cd ${BUILDROOT}

# Extract GLIBC
[ -d ${VER_GLIBC} ] || bunzip2 -c ${DOWNLOADS}/${VER_GLIBC}.tar.bz2 | tar xvf -
find ${PATCHES} -name ${VER_GLIBC}-*.patch | xargs -rtI {} cat {} | patch -d ${VER_GLIBC} -p1
cd ${VER_GLIBC}

if [ ${VER_GLIBC_PORTS} != "" ]; then
	bunzip2 -c ${DOWNLOADS}/${VER_GLIBC_PORTS}.tar.bz2 | tar xvf -
	ln -s ${VER_GLIBC_PORTS} ports
fi

if [ ${VER_GLIBC_THREADS} != "" ]; then
	if [ ! -d ${VER_GLIBC_THREADS} ]; then
		bunzip2 -c ${DOWNLOADS}/${VER_GLIBC_THREADS}.tar.bz2 | tar xvf -
		find ${PATCHES} -name ${VER_GLIBC_THREADS}-*.patch | xargs -rtI {} cat {} | patch -p0
	fi
fi

cd ..
mkdir BUILD/${VER_GLIBC}-headers
cd BUILD/${VER_GLIBC}-headers
echo "libc_cv_forced_unwind=yes" > config.cache
echo "libc_cv_c_cleanup=yes" >> config.cache
echo "libc_cv_arm_tls=yes" >> config.cache
echo "install_root=${SYSROOT}" > configparms
CC=gcc ../../${VER_GLIBC}/configure \
							 --prefix=/usr \
							 --host=${TARGET} \
							 --with-tls --with-__thread --disable-sanity-checks \
							 --enable-add-ons=${ENABLE_ADD_ONS_S1} \
							 --with-headers=${SYSROOT}/usr/include \
							 --cache-file=config.cache \
							 2>&1 | tee ${BUILDROOT}/logs/30.${VER_GLIBC}.configure.headers.log
make cross-compiling=yes install_root=${SYSROOT} install-headers 2>&1 | tee ${BUILDROOT}/logs/31.${VER_GLIBC}.make.headers.log

install -dv ${SYSROOT}/usr/include/bits
cp -v bits/stdio_lim.h ${SYSROOT}/usr/include/bits

touch ${SYSROOT}/usr/include/gnu/stubs.h
#touch ${SYSROOT}/usr/include/bits/stdio_lim.h

cp -v ../../${VER_GLIBC}/ports/sysdeps/unix/sysv/linux/arm/nptl/bits/pthreadtypes.h \
    ${SYSROOT}/usr/include/bits

#-------------------------------------------------------------------
# 4. Stage 1 gcc
#-------------------------------------------------------------------
cd ${BUILDROOT}

#GMP
if [ ${VER_GMP} != "" ]; then
	bunzip2 -c ${DOWNLOADS}/${VER_GMP}.tar.bz2 | tar xvf -
	find ${PATCHES} -name ${VER_GMP}-*.patch | xargs -rtI {} cat {} | patch -d ${VER_GMP} -p1	
	cd ${VER_GMP}
CC=${CROSS_COMPILE}gcc \
	AR=${CROSS_COMPILE}ar \
	RANLIB=${CROSS_COMPILE}ranlib \
	AS=${CROSS_COMPILE}as \
	LD=${CROSS_COMPILE}ld \
	./configure	--prefix=${SYSROOT}/usr/local \
				--build=${BUILD} \
				--host=${TARGET} \
				--enable-cxx \
				--enable-mpbsd \
				2>&1 | tee ${BUILDROOT}/logs/40.01.${VER_GMP}.configure.stage1.log
	make 2>&1 | tee ${BUILDROOT}/logs/40.02.${VER_GMP}.make.stage1.log
	make install 2>&1 | tee -a ${BUILDROOT}/logs/40.03.${VER_GMP}.make.stage1.install.log
	cd ${BUILDROOT}
fi

[ -d ${VER_GCC} ] || bunzip2 -c ${DOWNLOADS}/${VER_GCC}.tar.bz2 | tar xvf -
find ${PATCHES} -name ${VER_GCC}-*.patch | xargs -rtI {} cat {} | patch -d ${VER_GCC} -p1
mkdir -p BUILD/${VER_GCC}-stage1
cd BUILD/${VER_GCC}-stage1

#						   --disable-shared \
#						   --disable-threads \
#						   --disable-nls \

../../${VER_GCC}/configure --prefix=${PREFIX} \
						   --target=${TARGET} \
						   --enable-languages=c \
						   --with-sysroot=${SYSROOT} \
						   --with-gmp=${SYSROOT}/usr/local \
						   2>&1 | tee ${BUILDROOT}/logs/40.${VER_GCC}.configure.stage1.log
make 2>&1 | tee ${BUILDROOT}/logs/41.${VER_GCC}.make.stage1.log
make install 2>&1 | tee -a ${BUILDROOT}/logs/42.${VER_GCC}.make.stage1.install.log
[ -z "${TOOL_ALIAS}" ] || sh ${SCRIPT_DIR}/link-tools.sh ${PREFIX}/bin/${TARGET} ${PREFIX}/bin/${TOOL_ALIAS} 2>&1 | tee -a ${BUILDROOT}/logs/43.${VER_GCC}.link-gcc.sh.log

#-------------------------------------------------------------------
# 5. Build glibc
#-------------------------------------------------------------------
cd ${BUILDROOT}
mkdir BUILD/${VER_GLIBC}
cd BUILD/${VER_GLIBC}
BUILD_CC=gcc \
	CC=${CROSS_COMPILE}gcc \
	AR=${CROSS_COMPILE}ar \
	RANLIB=${CROSS_COMPILE}ranlib \
	AS=${CROSS_COMPILE}as \
	LD=${CROSS_COMPILE}ld \
	../../${VER_GLIBC}/configure --prefix=/usr \
								 --build=${BUILD} \
								 --host=${TARGET} \
								 ${GLIBC_EXTRA_S2} \
								 --enable-add-ons=${ENABLE_ADD_ONS_S2} \
								 --with-headers=${SYSROOT}/usr/include \
								 2>&1 | tee ${BUILDROOT}/logs/50.${VER_GLIBC}.configure.log
make 2>&1 | tee ${BUILDROOT}/logs/51.${VER_GLIBC}.make.log
make install_root=${SYSROOT} install | tee -a ${BUILDROOT}/logs/52.${VER_GLIBC}.make.install.log

#-------------------------------------------------------------------
# 6. Stage 2 gcc
#-------------------------------------------------------------------
cd ${BUILDROOT}
mkdir BUILD/${VER_GCC}
cd BUILD/${VER_GCC}
../../${VER_GCC}/configure --prefix=${PREFIX} \
						   --target=${TARGET} \
						   --enable-languages=c,c++ \
						   --with-sysroot=${SYSROOT} \
						   2>&1 | tee ${BUILDROOT}/logs/60.${VER_GCC}.configure.stage2.log
make 2>&1 | tee ${BUILDROOT}/logs/61.${VER_GCC}.make.stage2.log
make install 2>&1 | tee -a ${BUILDROOT}/logs/62.${VER_GCC}.make.stage2.install.log
[ -z "${TOOL_ALIAS}" ] || sh ${SCRIPT_DIR}/link-tools.sh ${PREFIX}/bin/${TARGET} ${PREFIX}/bin/${TOOL_ALIAS} 2>&1 | tee -a ${BUILDROOT}/logs/63.${VER_GCC}.link-gcc.sh.log

#-------------------------------------------------------------------
# 7. Build Linux kernel
#-------------------------------------------------------------------
cd ${BUILDROOT}/linux
make zImage 2>&1 | tee -a ${BUILDROOT}/logs/70.${VER_LINUX}.make.zImage.log
make modules 2>&1 | tee -a ${BUILDROOT}/logs/71.${VER_LINUX}.make.modules.log
make INSTALL_MOD_PATH=${SYSROOT} modules_install 2>&1 | tee -a ${BUILDROOT}/logs/72.${VER_LINUX}.make.modules.install.log


