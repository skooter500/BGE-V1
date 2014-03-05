#!/bin/bash

if [ "$UID" -ne 0 ] 
	then
		echo 'Compiling BGE' 
		make 
		make clean 
		export LD_LIBRARY_PATH=../Dependancies/linux/fmodapi44431linux/api/lib:../Dependancies/linux/SDL2_ttf-2.0.12/.libs:../Dependancies/linux/SDL-2.0.2-8236/build/.libs:../Dependancies/linux/bullet-2.81-rev2613/src/.libs:$LD_LIBRARY_PATH 
		./bgedemo 
		echo 'You can run BGE again in the future by cding to this directory and typing ./bgedemo' 
		echo 'If you re-run this script as root, the program will be fully installed on your computer automatically' 
		exit 0 
fi

echo 'Preparing dependencies (this will quite a long time)' 
mkdir /usr/lib/BGEForLinux
here=$(pwd)
cd /usr/lib/BGEForLinux
mkdir bullet-2.81-rev2613
mkdir fmodapi44431linux
mkdir SDL2_ttf-2.0.12
mkdir SDL-2.0.2-8236
cd $here
cp -rv ../Dependancies/linux/bullet-2.81-rev2613/src/.libs/* /usr/lib/BGEForLinux/bullet-2.81-rev2613
cp -rv ../Dependancies/linux/fmodapi44431linux/api/lib/* /usr/lib/BGEForLinux/fmodapi44431linux
cp -rv ../Dependancies/linux/SDL2_ttf-2.0.12/.libs/* /usr/lib/BGEForLinux/SDL2_ttf-2.0.12
cp -rv ../Dependancies/linux/SDL-2.0.2-8236/build/.libs/* /usr/lib/BGEForLinux/SDL-2.0.2-8236
# update /etc/ld.so.conf
echo '/usr/lib/BGEForLinux/bullet-2.81-rev2613' >> '/etc/ld.so.conf'
echo '/usr/lib/BGEForLinux/fmodapi44431linux' >> '/etc/ld.so.conf' 
echo '/usr/lib/BGEForLinux/SDL2_ttf-2.0.12' >> '/etc/ld.so.conf'
echo '/usr/lib/BGEForLinux/SDL2_ttf-2.0.12' >> '/etc/ld.so.conf'
echo '/usr/lib/BGEForLinux/SDL-2.0.2-8236' >> '/etc/ld.so.conf'
ldconfig -v
exit 
echo 'Compiling BGE'
make 
make clean

