CC = arm-hismall-linux-g++

#QianExe目标文件
Q_O = ./QianExe/yx_DeviceCtrlKaiTian.o 	./QianExe/yx_DriverCtrlKaiTian.o 	./QianExe/yx_IrdModKaiTian.o \
		./QianExe/yx_LedKaiTian.o 			./QianExe/yx_MeterModKaiTian.o 	./QianExe/yx_Blk.o	\
		./QianExe/yx_MonAlert.o			./QianExe/yx_QianStdAfx.o 			./QianExe/yx_Sms.o \
		./QianExe/yx_CarSta.o		  		./QianExe/yx_Help.o 				./QianExe/yx_DrvRecRpt.o \
		./QianExe/yx_Cfg.o 					./QianExe/yx_Queue.o 		  		./QianExe/yx_Sock.o \
		./QianExe/yx_Comu.o 				./QianExe/yx_MeterMod.o 			./QianExe/yx_Diaodu.o \
		./QianExe/yx_AutoRpt.o 				./GlobalShare/TimerMng.o 		 	./QianExe/yx_LedChuangLi.o \
		./QianExe/yx_LedBoHai.o	  		./QianExe/yx_QianMain.o 			./GlobalShare/InnerDataMng.o \
		./QianExe/yx_ComBus.o 				./QianExe/yx_DriveRecord.o 			./QianExe/yx_LinkLst.o	\
		./QianExe/yx_IO.o					./QianExe/yx_SpecCtrl.o 			./QianExe/yx_DriverCtrl.o \
		 ./QianExe/yx_Acdent.o				./QianExe/yx_RealPos.o				./QianExe/yx_Jijia.o \
		./QianExe/yx_Photo.o				./GlobalShare/md5.o		 			./GlobalShare/Md5Decrypt.o	\
		./GlobalShare/GlobFunc.o			./QianExe/yx_PhoneBook.o	./QianExe/yx_Com150TR.o	\
		./QianExe/yx_FlashPart4Mng.o	./QianExe/yx_Oil.o		./QianExe/yx_AutoRptStationNew.o	./QianExe/yx_DownLineFile.o
	
#UpdateExe目标文件
U_O =	./UpdateExe/yx_UpdateMain.o 	./UpdateExe/yx_Sock.o 	./UpdateExe/yx_DownLoad.o 	 ./UpdateExe/yx_UpdateStdAfx.o \
		./GlobalShare/TimerMng.o	 		./GlobalShare/md5.o 			./GlobalShare/Md5Decrypt.o \
		./GlobalShare/InnerDataMng.o 		./GlobalShare/GlobFunc.o

#ComuExe目标文件
C_O =	./ComuExe/ComuExeMain.o 	./ComuExe/ComuStdAfx.o 		./ComuExe/DiaoDu.o 			./ComuExe/GpsSrc.o \
		./ComuExe/PhoneEvdo.o 		./ComuExe/Sms.o 				./GlobalShare/GlobFunc.o 	./GlobalShare/InnerDataMng.o \
		./ComuExe/IoSta.o			./ComuExe/LightCtrl.o		./ComuExe/ComAdjust.o		./ComuExe/Handset.o \
		./ComuExe/HandApp.o		./ComuExe/StackLst.o		./GlobalShare/TimerMng.o	./ComuExe/PhoneGsm.o \
		./ComuExe/ComVir.o			./ComuExe/LightCtrl2.o		./ComuExe/PhoneTD.o		./ComuExe/PhoneWcdma.o

#SockServExe目标文件
S_O =	./SockServExe/SockServMain.o 	./SockServExe/TcpSock.o 			./SockServExe/UdpSock.o \
		./SockServExe/StdAfx.o 			./GlobalShare/InnerDataMng.o 		./GlobalShare/TimerMng.o \
		./GlobalShare/GlobFunc.o

#DvrExe目标文件
D_O = 	./DvrExe/src/yx_osd.o 		./DvrExe/src/yx_disk.o		./DvrExe/src/yx_qian.o 		./DvrExe/src/yx_tts.o  \
		./DvrExe/src/yx_irda.o 			./DvrExe/src/yx_menu.o		./DvrExe/src/yx_photo.o  \
		./DvrExe/src/yx_encode.o 	./DvrExe/src/yx_decode.o 	./DvrExe/src/yx_common.o 	./DvrExe/src/yx_config.o \
		./DvrExe/src/yx_monitor.o 	./DvrExe/src/yx_black.o   ./DvrExe/src/yx_upload.o  \
		./DvrExe/src/yx_main.o  	./GlobalShare/InnerDataMng.o		./GlobalShare/TimerMng.o     ./GlobalShare/GlobFunc.o \
		./DvrExe/src/yx_Sock.o		./DvrExe/src/yx_black_new.o

#IOExe目标文件
IO_O = ./IOExe/IOExe.o		./GlobalShare/GlobFunc.o

#ComuServ目标文件
CS_O = ./GlobalShare/GlobFunc.o 	./ComuServ/Gps.o 		./ComuServ/yx_gpio.o 	./ComuServ/IO.o \
		./ComuServ/CfgMng.o 		./ComuServ/Queue.o 		./ComuServ/MsgQueue.o 	./ComuServ/MsgQueMng.o \
		./ComuServ/ComuServ.o 		./ComuServ/TimeMng.o	./ComuServ/Dog.o
		
#Monitor目标文件
M_O = ./Monitor/Main.o	./Monitor/Dog.o		./Monitor/StdAfx.o

ALL_OBJ = $(Q_O) $(CS_O) $(U_O) $(S_O) $(C_O) $(IO_O) $(D_O) $(M_O)

Q_P = ./QianExe/QianExe
U_P = ./UpdateExe/UpdateExe
S_P = ./SockServExe/SockServExe
C_P = ./ComuExe/ComuExe
D_P = ./DvrExe/DvrExe
IO_P = ./IOExe/IOExe
M_P = ./Monitor/Monitor
CS_P = ./ComuServ/libComuServ.so
CS_P_BKP = ./ComuServ/libComuServBkp.so

ALL_OUTPUT = $(Q_P) $(S_P) $(U_P) $(CS_P) $(CS_P_BKP) $(IO_P) $(C_P) $(D_P) $(M_P)

INFO = =========================================================================================================

###################################################################################################
mg_backup: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3510/  
mg_backup: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3510 -liconv -ldl -lpthread -lComuServBkp -lAiSound5 -lhiapi
mg_backup: clean begin mg_mktype_Bkp

mt_backup: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3510/  
mt_backup: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3510 -liconv -ldl -lpthread -lComuServBkp -lAiSound5 -lhiapi
mt_backup: clean begin mt_mktype_Bkp

mk_backup: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3510/  
mk_backup: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3510 -liconv -ldl -lpthread -lComuServBkp -lAiSound5 -lhiapi
mk_backup: clean begin mk_mktype_Bkp

mw_backup: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3510/  
mw_backup: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3510 -liconv -ldl -lpthread -lComuServBkp -lAiSound5 -lhiapi
mw_backup: clean begin mw_mktype_Bkp

mg_mktype_Bkp:
	./SoftPack/Mktype M BENQ 1
mt_mktype_Bkp:
	./SoftPack/Mktype M LC6311 1
mk_mktype_Bkp:
	./SoftPack/Mktype M MC703OLD 1
mw_mktype_Bkp:
	./SoftPack/Mktype M SIM5218 1


mg: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3510/  
mg: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3510 -liconv -ldl -lpthread -lComuServ -lAiSound5 -lhiapi
mg: begin mg_mktype m_install

mt: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3510/  
mt: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3510 -liconv -ldl -lpthread -lComuServ -lAiSound5 -lhiapi
mt: begin mt_mktype m_install

mk: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3510/  
mk: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3510 -liconv -ldl -lpthread -lComuServ -lAiSound5 -lhiapi
mk: begin mk_mktype m_install

mw: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3510/  
mw: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3510 -liconv -ldl -lpthread -lComuServ -lAiSound5 -lhiapi
mw: begin mw_mktype m_install

mg_rebuild: clean mg
mt_rebuild: clean mt
mk_rebuild: clean mk
mw_rebuild: clean mw

mg_mkpack: m_mkpack
mt_mkpack: m_mkpack
mk_mkpack: m_mkpack
mw_mkpack: m_mkpack

mg_mktype:
	./SoftPack/Mktype M BENQ 0
mt_mktype:
	./SoftPack/Mktype M LC6311 0
mk_mktype:
	./SoftPack/Mktype M MC703OLD 0
mw_mktype:
	./SoftPack/Mktype M SIM5218 0
	
m_install: $(ALL_OUTPUT)
	cp -f $(Q_P) /work/Hi3510_VSSDK_V1.3.6.0_SDRAM/M_rootbox/mnt/Flash/part5/QianExe
	cp -f $(CS_P) /work/Hi3510_VSSDK_V1.3.6.0_SDRAM/M_rootbox/mnt/Flash/part5/libComuServ.so
	cp -f $(U_P) /work/Hi3510_VSSDK_V1.3.6.0_SDRAM/M_rootbox/mnt/Flash/part5/UpdateExe
	cp -f $(S_P) /work/Hi3510_VSSDK_V1.3.6.0_SDRAM/M_rootbox/mnt/Flash/part5/SockServExe
	cp -f $(C_P) /work/Hi3510_VSSDK_V1.3.6.0_SDRAM/M_rootbox/mnt/Flash/part5/ComuExe
	cp -f $(IO_P) /work/Hi3510_VSSDK_V1.3.6.0_SDRAM/M_rootbox/mnt/Flash/part5/IOExe
	cp -f $(D_P) /work/Hi3510_VSSDK_V1.3.6.0_SDRAM/M_rootbox/mnt/Flash/part5/DvrExe
	cp -f $(M_P) /work/Hi3510_VSSDK_V1.3.6.0_SDRAM/M_rootbox/mnt/Flash/part5/Monitor
	@echo $(INFO)
	
m_mkpack:
	mkdir -p ../M_bin
	
	./SoftPack/Mkexe M

	tar -zxvf ../M_bin/M_rootbox.tar.gz -C ../
	mknod  ../M_bin/M_rootbox/dev/console c 5 1
	./SoftPack/Mkroot -d ../M_bin/M_rootbox -l -e 0x20000 -o ../M_bin/M_root.bin
	rm -f -r ../M_bin/M_rootbox

	mkdir -p ../M_bin/M_update
	mkdir -p ../M_bin/M_update/Down
	cp -f ../M_bin/M_exe.bin  ../M_bin/M_update/Down/exe.bin
	./SoftPack/Mkroot -d ../M_bin/M_update -l -e 0x20000 -o ../M_bin/M_update.bin
	rm -f -r ../M_bin/M_update
	
	mkdir -p ../M_bin/M_app
	cp -f $(Q_P)  ../M_bin/M_app/QianExe
	cp -f $(U_P)  ../M_bin/M_app/UpdateExe
	cp -f $(S_P)  ../M_bin/M_app/SockServExe
	cp -f $(C_P)  ../M_bin/M_app/ComuExe
	cp -f $(IO_P) ../M_bin/M_app/IOExe
	cp -f $(D_P)  ../M_bin/M_app/DvrExe
	cp -f $(CS_P) ../M_bin/M_app/libComuServ.so
	./SoftPack/Mkroot -d ../M_bin/M_app -l -e 0x20000 -o ../M_bin/M_app.bin
	rm -f -r ../M_bin/M_app

	./SoftPack/Mkimage M
###################################################################################################
v8g_backup: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3511/ 
v8g_backup: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3511 -liconv -ldl -lpthread -lComuServBkp -lAiSound5 -lmpi -l_VoiceEngine -l_amr_spc -l_amr_fipop -l_aec -l_aacdec -l_aacenc -lloadbmp -ltde 
v8g_backup: clean begin v8g_mktype_Bkp v8_install_Bkp

v8t_backup: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3511/ 
v8t_backup: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3511 -liconv -ldl -lpthread -lComuServBkp -lAiSound5 -lmpi -l_VoiceEngine -l_amr_spc -l_amr_fipop -l_aec -l_aacdec -l_aacenc -lloadbmp -ltde 
v8t_backup: clean begin v8t_mktype_Bkp v8_install_Bkp

v8k_backup: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3511/ 
v8k_backup: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3511 -liconv -ldl -lpthread -lComuServBkp -lAiSound5 -lmpi -l_VoiceEngine -l_amr_spc -l_amr_fipop -l_aec -l_aacdec -l_aacenc -lloadbmp -ltde 
v8k_backup: clean begin v8k_mktype_Bkp v8_install_Bkp

v8w_backup: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3511/ 
v8w_backup: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3511 -liconv -ldl -lpthread -lComuServBkp -lAiSound5 -lmpi -l_VoiceEngine -l_amr_spc -l_amr_fipop -l_aec -l_aacdec -l_aacenc -lloadbmp -ltde 
v8w_backup: clean begin v8w_mktype_Bkp v8_install_Bkp

v8g_mktype_Bkp:
	./SoftPack/Mktype V8 BENQ 1
v8t_mktype_Bkp:
	./SoftPack/Mktype V8 LC6311 1
#	./SoftPack/Mktype V8 SIM4222 1
v8k_mktype_Bkp:
	./SoftPack/Mktype V8 MC703OLD 1
v8w_mktype_Bkp:
	./SoftPack/Mktype V8 SIM5218 1
#	./SoftPack/Mktype V8 MU203 1

v8_install_Bkp: $(ALL_OUTPUT)
	tar -zxvf ../V8_bin/V8_rootbox.tar.gz -C ../
	cp -f ../V8_bin/Version ../V8_bin/V8_rootbox/root/Version
	cp -f $(CS_P_BKP) ../V8_bin/V8_rootbox/libComuServBkp.so
	cp -f $(Q_P)  ../V8_bin/V8_rootbox/QianExe
	cp -f $(U_P) ../V8_bin/V8_rootbox/UpdateExe
	cp -f $(S_P) ../V8_bin/V8_rootbox/SockServExe
	cp -f $(C_P) ../V8_bin/V8_rootbox/ComuExe
	cp -f $(IO_P) ../V8_bin/V8_rootbox/IOExe
	cp -f $(M_P) ../V8_bin/V8_rootbox/Monitor
	tar -zcvf ../V8_bin/V8_rootbox.tar.gz ../V8_bin/V8_rootbox
	rm -f -r ../V8_bin/V8_rootbox


v8g: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3511/ 
v8g: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3511 -liconv -ldl -lpthread -lComuServ -lAiSound5 -lmpi -l_VoiceEngine -l_amr_spc -l_amr_fipop -l_aec -l_aacdec -l_aacenc -lloadbmp -ltde 
v8g: begin v8g_mktype v8_install

v8t: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3511/ 
v8t: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3511 -liconv -ldl -lpthread -lComuServ -lAiSound5 -lmpi -l_VoiceEngine -l_amr_spc -l_amr_fipop -l_aec -l_aacdec -l_aacenc -lloadbmp -ltde 
v8t: begin v8t_mktype v8_install

v8k: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3511/ 
v8k: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3511 -liconv -ldl -lpthread -lComuServ -lAiSound5 -lmpi -l_VoiceEngine -l_amr_spc -l_amr_fipop -l_aec -l_aacdec -l_aacenc -lloadbmp -ltde 
v8k: begin v8k_mktype v8_install

v8w: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3511/ 
v8w: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3511 -liconv -ldl -lpthread -lComuServ -lAiSound5 -lmpi -l_VoiceEngine -l_amr_spc -l_amr_fipop -l_aec -l_aacdec -l_aacenc -lloadbmp -ltde 
v8w: begin v8w_mktype v8_install

v8g_rebuild: clean v8g
v8t_rebuild: clean v8t
v8k_rebuild: clean v8k
v8w_rebuild: clean v8w

v8g_mkpack: v8_mkpack
v8t_mkpack: v8_mkpack
v8k_mkpack: v8_mkpack
v8w_mkpack: v8_mkpack

v8g_mktype:
	./SoftPack/Mktype V8 BENQ 0
v8t_mktype:
	./SoftPack/Mktype V8 LC6311 0
#	./SoftPack/Mktype V8 SIM4222 0
v8k_mktype:
	./SoftPack/Mktype V8 MC703OLD 0
v8w_mktype:
	./SoftPack/Mktype V8 SIM5218 0
#	./SoftPack/Mktype V8 MU203 0

v8_install: $(ALL_OUTPUT)
	cp -f $(Q_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/mnt/Flash/part5/QianExe
	cp -f $(CS_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/mnt/Flash/part5/libComuServ.so
	cp -f $(U_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/mnt/Flash/part5/UpdateExe
	cp -f $(S_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/mnt/Flash/part5/SockServExe
	cp -f $(C_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/mnt/Flash/part5/ComuExe
	cp -f $(IO_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/mnt/Flash/part5/IOExe
	cp -f $(D_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/mnt/Flash/part5/DvrExe
	cp -f $(M_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/mnt/Flash/part5/Monitor
	
	cp -f $(Q_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/QianExe
	cp -f $(CS_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/libComuServ.so
	cp -f $(U_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/UpdateExe
	cp -f $(S_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/SockServExe
	cp -f $(C_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/ComuExe
	cp -f $(IO_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/IOExe
	cp -f $(D_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/DvrExe
	cp -f $(M_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/Monitor
	
	@echo $(INFO)

v8_mkpack:
	mkdir -p ../V8_bin

	./SoftPack/Mkexe V8

	tar -zxvf ../V8_bin/V8_rootbox.tar.gz -C ../
	mknod ../V8_bin/V8_rootbox/dev/console c 5 1
	./SoftPack/Mkroot -d ../V8_bin/V8_rootbox -l -e 0x20000 -o ../V8_bin/V8_root.bin
	rm -f -r ../V8_bin/V8_rootbox

	mkdir -p ../V8_bin/V8_update
	mkdir -p ../V8_bin/V8_update/Down
	cp -f ../V8_bin/V8_exe.bin  ../V8_bin/V8_update/Down/exe.bin
	./SoftPack/Mkroot -d ../V8_bin/V8_update -l -e 0x20000 -o ../V8_bin/V8_update.bin
	rm -f -r ../V8_bin/V8_update

	mkdir -p ../V8_bin/V8_app
	cp -f $(Q_P)  ../V8_bin/V8_app/QianExe
	cp -f $(U_P)  ../V8_bin/V8_app/UpdateExe
	cp -f $(S_P)  ../V8_bin/V8_app/SockServExe
	cp -f $(C_P)  ../V8_bin/V8_app/ComuExe
	cp -f $(IO_P) ../V8_bin/V8_app/IOExe
	cp -f $(D_P)  ../V8_bin/V8_app/DvrExe
	cp -f $(CS_P) ../V8_bin/V8_app/libComuServ.so
	./SoftPack/Mkroot -d ../V8_bin/V8_app -l -e 0x20000 -o ../V8_bin/V8_app.bin
	rm -f -r ../V8_bin/V8_app

	./SoftPack/Mkimage V8
###################################################################################################
m2g_backup: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3511/ 
m2g_backup: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3511 -liconv -ldl -lpthread -lComuServBkp -lAiSound5 -lmpi -l_VoiceEngine -l_amr_spc -l_amr_fipop -l_aec -l_aacdec -l_aacenc -lloadbmp -ltde 
m2g_backup: clean begin m2g_mktype_Bkp v8_install_Bkp

m2t_backup: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3511/ 
m2t_backup: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3511 -liconv -ldl -lpthread -lComuServBkp -lAiSound5 -lmpi -l_VoiceEngine -l_amr_spc -l_amr_fipop -l_aec -l_aacdec -l_aacenc -lloadbmp -ltde 
m2t_backup: clean begin m2t_mktype_Bkp v8_install_Bkp

m2k_backup: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3511/ 
m2k_backup: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3511 -liconv -ldl -lpthread -lComuServBkp -lAiSound5 -lmpi -l_VoiceEngine -l_amr_spc -l_amr_fipop -l_aec -l_aacdec -l_aacenc -lloadbmp -ltde 
m2k_backup: clean begin m2k_mktype_Bkp v8_install_Bkp

m2w_backup: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3511/ 
m2w_backup: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3511 -liconv -ldl -lpthread -lComuServBkp -lAiSound5 -lmpi -l_VoiceEngine -l_amr_spc -l_amr_fipop -l_aec -l_aacdec -l_aacenc -lloadbmp -ltde 
m2w_backup: clean begin m2w_mktype_Bkp v8_install_Bkp

m2g_mktype_Bkp:
	./SoftPack/Mktype M2 BENQ 1
m2t_mktype_Bkp:
	./SoftPack/Mktype M2 LC6311 1
#	./SoftPack/Mktype M2 SIM4222 1
m2k_mktype_Bkp:
	./SoftPack/Mktype M2 MC703OLD 1
m2w_mktype_Bkp:
	./SoftPack/Mktype M2 SIM5218 1
#	./SoftPack/Mktype M2 MU203 1


m2g: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3511/ 
m2g: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3511 -liconv -ldl -lpthread -lComuServ -lAiSound5 -lmpi -l_VoiceEngine -l_amr_spc -l_amr_fipop -l_aec -l_aacdec -l_aacenc -lloadbmp -ltde 
m2g: begin m2g_mktype m2_install

m2t: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3511/ 
m2t: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3511 -liconv -ldl -lpthread -lComuServ -lAiSound5 -lmpi -l_VoiceEngine -l_amr_spc -l_amr_fipop -l_aec -l_aacdec -l_aacenc -lloadbmp -ltde 
m2t: begin m2t_mktype m2_install

m2k: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3511/ 
m2k: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3511 -liconv -ldl -lpthread -lComuServ -lAiSound5 -lmpi -l_VoiceEngine -l_amr_spc -l_amr_fipop -l_aec -l_aacdec -l_aacenc -lloadbmp -ltde 
m2k: begin m2k_mktype m2_install

m2w: INC = -I. -I./GlobalShare -I./DvrExe/inc/hi3511/ 
m2w: LIB = -D _REENTRANT -L./ComuServ -L./DvrExe/lib/hi3511 -liconv -ldl -lpthread -lComuServ -lAiSound5 -lmpi -l_VoiceEngine -l_amr_spc -l_amr_fipop -l_aec -l_aacdec -l_aacenc -lloadbmp -ltde 
m2w: begin m2w_mktype m2_install

m2g_rebuild: clean m2g
m2t_rebuild: clean m2t
m2k_rebuild: clean m2k
m2w_rebuild: clean m2w

m2g_mktype:
	./SoftPack/Mktype M2 BENQ 0
m2t_mktype:
	./SoftPack/Mktype M2 LC6311 0
#	./SoftPack/Mktype M2 SIM4222 0
m2k_mktype:
	./SoftPack/Mktype M2 MC703OLD 0
m2w_mktype:
	./SoftPack/Mktype M2 SIM5218 0
#	./SoftPack/Mktype M2 MU203 0

m2_install: $(ALL_OUTPUT)
	cp -f $(Q_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/mnt/Flash/part5/QianExe
	cp -f $(CS_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/mnt/Flash/part5/libComuServ.so
	cp -f $(U_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/mnt/Flash/part5/UpdateExe
	cp -f $(S_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/mnt/Flash/part5/SockServExe
	cp -f $(C_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/mnt/Flash/part5/ComuExe
	cp -f $(IO_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/mnt/Flash/part5/IOExe
	cp -f $(D_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/mnt/Flash/part5/DvrExe
	cp -f $(M_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/mnt/Flash/part5/Monitor	
	cp -f $(Q_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/QianExe
	cp -f $(CS_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/libComuServ.so
	cp -f $(U_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/UpdateExe
	cp -f $(S_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/SockServExe
	cp -f $(C_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/ComuExe
	cp -f $(IO_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/IOExe
	cp -f $(D_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/DvrExe
	cp -f $(M_P) /work/Hi3511_VSSDK_V1.1.2.3/V8_rootbox/Monitor
	
	@echo $(INFO)
###################################################################################################

begin: 
	clear
	@echo $(INFO)

clean: 
	-rm $(Q_O) $(Q_P) $(CS_O) $(CS_P) $(CS_P_BKP) $(U_O) $(U_P) $(S_O) $(S_P) $(C_O) $(C_P) $(D_O) $(D_P) $(IO_O) $(IO_P) $(M_O) $(M_P) 

$(Q_P): $(CS_P) $(CS_P_BKP) $(Q_O)
	$(CC)  -o $(Q_P) $(Q_O) $(LIB)
	@echo  $(INFO)     

$(U_P):  $(CS_P) $(CS_P_BKP) $(U_O)
	$(CC)  -o $(U_P) $(U_O) $(LIB)
	@echo  $(INFO)

$(S_P):  $(CS_P) $(CS_P_BKP) $(S_O)
	$(CC)  -o $(S_P) $(S_O) $(LIB)
	@echo  $(INFO)

$(C_P):  $(CS_P) $(CS_P_BKP) $(C_O)
	$(CC)  -o $(C_P) $(C_O) $(LIB)
	@echo  $(INFO)

$(D_P):  $(CS_P) $(CS_P_BKP) $(D_O)
	$(CC)  -o $(D_P) $(D_O) $(LIB)
	@echo  $(INFO)

$(IO_P):  $(CS_P) $(CS_P_BKP) $(IO_O)
	$(CC)  -o $(IO_P) $(IO_O) $(LIB)
	@echo  $(INFO)
	
$(M_P):  $(CS_P) $(CS_P_BKP) $(M_O)
	$(CC)  -o $(M_P) $(M_O) -ldl -lpthread
	@echo  $(INFO)

$(CS_P): $(CS_O)
	$(CC) -shared -o $(CS_P) $(CS_O)
	@echo $(INFO)

$(CS_P_BKP): $(CS_O)
	$(CC) -shared -o $(CS_P_BKP) $(CS_O)
	@echo $(INFO)
	

$(Q_O): %.o : %.cpp
	$(CC) -c  $< -o $@ 

$(U_O): %.o : %.cpp
	$(CC) -c  $< -o $@

$(S_O): %.o : %.cpp
	$(CC) -c  $< -o $@

$(C_O): %.o : %.cpp
	$(CC) -c  $< -o $@

$(D_O): %.o : %.cpp
	$(CC) -c  $< -o $@ $(INC)

$(IO_O): %.o : %.cpp
	$(CC) -c  $< -o $@
	
$(M_O): %.o : %.cpp
	$(CC) -c  $< -o $@

$(CS_O): %.o : %.cpp
	$(CC) -c -fPIC -Wall $< -o $@
	
