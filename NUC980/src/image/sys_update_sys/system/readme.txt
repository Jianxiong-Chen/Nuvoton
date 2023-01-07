一、PACK wriet
   Image Name       Image Type     Image start offset/Image execute address    点击操作
1、擦除 							        Erase
2、pack.bin           pack					        Burn
二、分散文件烧写
   Image Name                       Image Type          Image start offset/Image execute address    点击操作
1、擦除 									Erase
2、u-boot-spl.bin                   uBoot  	               200  				Burn
3、u-boot.bin                         Data 	               100000  				Burn  
4、env.txt                               Environment  	80000  				Burn 
5、uImage                              Data    		200000  				Burn 
5、rootfs.ubi          	               Data    		1600000  				Burn 
5、nuc980-dev-v1.0.dtb           Data    		  B00000				Burn 