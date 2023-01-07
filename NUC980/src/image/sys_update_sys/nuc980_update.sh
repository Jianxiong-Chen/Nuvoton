#! /bin/sh
#NUC980 NandFlash启动系统烧写脚本


pwd_dir=$PWD
#pwd_dir=$PWD/run_script/sys_update
echo "当前目录： $pwd_dir"
if [ ! -b /dev/mtdblock0 ]; then
   echo "错误: 你的设备不存在NandFlash"
   exit 1
fi



if [ ! -d $pwd_dir ]; then
   echo "错误: $sdkdir目录不存在"
   exit 1
fi



if [ ! -d $pwd_dir/system ]; then
	echo "下找不到system 不需要更新系统"
	#exit 1
else
	echo "开始更新系统"
	if [ ! -f /usr/sbin/nandwrite ]; then
	   echo "/usr/sbin/nandwrite不存在 开始添加nandwrite命令"
	   mv  $pwd_dir/system/nandwrite /usr/sbin/
	   chmod +x   /usr/sbin/nandwrite
	   echo "添加nandwrite命令完成"
	   sync
	   #exit 1
	fi

	if [ ! -f /usr/sbin/flash_erase ]; then
	   echo "/usr/sbin/flash_erase不存在 开始添加flash_erase"
	   mv $pwd_dir/system/flash_erase /usr/sbin/
	   chmod +x   /usr/sbin/flash_erase
	   echo "添加flash_erase命令完成"
	   sync
	   #exit 1
	fi 


	if [ ! -f $pwd_dir/system/u-boot-spl.bin ]; then
		echo "错误: $pwd_dir/u-boot-spl.bin 不存在不进行更新"
		#exit 1
	else
		if [ ! -f $pwd_dir/system/NUC980DF61YC.bin ]; then
			echo "$pwd_dir/NUC980DF61YC.bin 不存在不进行更新"
			#exit 1
		else
			echo "正在合并NUC980DF61YC.bin文件和u-boot-spl.bin文件到NUC980DF61YC_u-boot-spl.bin中"
			cat $pwd_dir/system/NUC980DF61YC.bin $pwd_dir/system/u-boot-spl.bin > $pwd_dir/system/NUC980DF61YC_u-boot-spl.bin
			echo "正在擦除u-boot分区...0x0-0x80000  4块数据 ，128K/块"
			flash_erase /dev/mtd0 0x0 4
			
			echo "正在烧写NUC980DF61YC_u-boot-spl.bin...0x00000000"
			nandwrite -p -s 0x00000000 			/dev/mtd0 $pwd_dir/system/NUC980DF61YC_u-boot-spl.bin
			
			
			echo "正在烧写NUC980DF61YC_u-boot-spl.bin...0x00020000"
			nandwrite -p -s 0x00020000 			/dev/mtd0 $pwd_dir/system/NUC980DF61YC_u-boot-spl.bin
			
			echo "正在烧写NUC980DF61YC_u-boot-spl.bin...0x00040000"
			nandwrite -p -s 0x00040000 			/dev/mtd0 $pwd_dir/system/NUC980DF61YC_u-boot-spl.bin
			
			
			echo "正在烧写NUC980DF61YC_u-boot-spl.bin...0x00060000"
			nandwrite -p -s 0x00060000 			/dev/mtd0 $pwd_dir/system/NUC980DF61YC_u-boot-spl.bin
			sleep 1
			rm $pwd_dir/system/NUC980DF61YC_u-boot-spl.bin
			sync
			echo "烧写u-boot-spl.bin完成"
			echo "              "
		fi             
	fi


	if [ ! -f $pwd_dir/system/env.txt ]; then                                    
		echo "错误: $pwd_dir/环境变量env.txt文件不存在不进行更新"                              
		#exit 1
	else
		echo "正在擦除u-boot分区...0x80000-0x100000  1块数据 ，128K/块"
		#flash_erase /dev/mtd0 0x80000 1
		echo "正在烧写env.txt"
		#nandwrite -p -s 0x80000 	/dev/mtd0 $pwd_dir/system/env.txt
		sync
		echo "烧写env.txt完成"
		echo "              "
	fi 



	if [ ! -f $pwd_dir/system/u-boot.bin ]; then
		echo "错误: $pwd_dir/u-boot.bin文件不存在不进行更新"
		#exit 1
	else
		echo "正在擦除u-boot分区...0x100000-0x200000  8块数据 ，128K/块"
		flash_erase /dev/mtd0 0x100000 8
		echo "正在烧写u-boot.bin..."
		nandwrite -p -s 0x100000 /dev/mtd0 $pwd_dir/system/u-boot.bin
		sync
		echo "烧写u-boot.bin完成"
		echo "              "
	fi


	if [ ! -f $pwd_dir/system/zImage ]; then
		echo "错误: $pwd_dir/uImage文件不存在不进行更新"
		#exit 1
	else
		echo "正在擦除kernel分区...0x200000-0xB00000  72块数据 ，128K/块"
		flash_erase /dev/mtd1 0x0 72
		echo "正在烧写kernel..."
		nandwrite -p -s 0x0 /dev/mtd1 $pwd_dir/system/zImage
		sync
		echo "烧写kernel..zImage 完成"
		echo "              "
	fi

	if [ ! -f $pwd_dir/system/*.dtb ]; then
		echo "错误: $pwd_dir/设备树不存在不进行更新"
		#exit 1
	else
		echo "正在擦除kernel分区...0xB00000-0x1400000  72块数据 ，128K/块"
		flash_erase /dev/mtd1 0x900000 72
		echo "正在烧写设备树..."
		nandwrite -p -s 0x900000 /dev/mtd1 $pwd_dir/system/nuc980-cjx.dtb
		sync
		echo "kernel分区设备树烧写完成..."
		echo "              "
	fi
	#rm -rf system
fi



if [ ! -f $pwd_dir/rootfs/rootfs.ubi ]; then
	echo "错误: $pwd_dir/rootfs/ubi.img下找不到文件系统压缩包"
	#exit 1
else
	flash_erase /dev/mtd2 0x0 0
	nandwrite -p -s 0x0 /dev/mtd2 $pwd_dir/rootfs/rootfs.ubi
	echo "NUC980 NandFlash系统烧写完成！"
	exit 1
	
	mv  $pwd_dir/system/ubi[adfm]*   /usr/sbin/
    	chmod +x   /usr/sbin/*
    	echo "添加ubi工具命令完成"
    	sync
	
	echo "正在擦除文件系统分区，请稍候..."
	flash_erase /dev/mtd2 0x0 0
	ubiformat /dev/mtd2
	ubiattach /dev/ubi_ctrl -m 5
	ubimkvol /dev/ubi0 -Nrootfs -m
	mkdir -p /mnt/mtd2
	mount -t ubifs ubi0:rootfs /mnt/mtd2
	echo "正在解压文件系统到mtd2分区，请稍候..."
	tar jxfm $pwd_dir/filesystem/*.tar.* -C /mnt/mtd5
	sleep 1
	sync
	umount /mnt/mtd2
	rm -rf /mnt/mtd2
	ubidetach -p /dev/mtd2
	echo "文件系统更新完成"
	echo "NUC980 NandFlash系统烧写完成！"
	
	
fi

if [ ! -d $pwd_dir/rootfs ]; then
	echo "下找不到rootfs 不需要更新文件系统"
	#exit 1
else
	echo "开始更新文件系统"
	cp -r $pwd_dir/rootfs/*     /
	sleep 3
	#rm -rf rootfs
	sync
	echo "文件系统更新完成"
	echo "              "
fi



echo "NUC980 NandFlash系统烧写完成！"
exit 1


