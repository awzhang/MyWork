<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: ComuExe - Win32 (WCE MIPSII_FP) Debug--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\ADMINI~1\LOCALS~1\Temp\RSP554.tmp" with contents
[
/nologo /W3 /Zi /Od /D "DEBUG" /D "MIPS" /D "_MIPS_" /D "R4000" /D "MIPSII" /D "MIPSII_FP" /D UNDER_CE=420 /D _WIN32_WCE=420 /D "WCE_PLATFORM_STANDARDSDK" /D "UNICODE" /D "_UNICODE" /Fp"MIPSII_FPDbg/ComuExe.pch" /YX /Fo"MIPSII_FPDbg/" /Fd"MIPSII_FPDbg/" /QMmips2 /QMFPE- /MC /c 
"E:\车台源码\Linux车台\DVR车台通用版\ComuExe\ComAdjust.cpp"
"E:\车台源码\Linux车台\DVR车台通用版\ComuExe\ComuExeMain.cpp"
"E:\车台源码\Linux车台\DVR车台通用版\ComuExe\ComuStdAfx.cpp"
"E:\车台源码\Linux车台\DVR车台通用版\ComuExe\ComVir.cpp"
"E:\车台源码\Linux车台\DVR车台通用版\ComuExe\DiaoDu.cpp"
"E:\车台源码\Linux车台\DVR车台通用版\ComuExe\GpsSrc.cpp"
"E:\车台源码\Linux车台\DVR车台通用版\ComuExe\HandApp.cpp"
"E:\车台源码\Linux车台\DVR车台通用版\ComuExe\Handset.cpp"
"E:\车台源码\Linux车台\DVR车台通用版\ComuExe\IoSta.cpp"
"E:\车台源码\Linux车台\DVR车台通用版\ComuExe\LightCtrl2.cpp"
"E:\车台源码\Linux车台\DVR车台通用版\ComuExe\LightCtrl.cpp"
"E:\车台源码\Linux车台\DVR车台通用版\ComuExe\PhoneEvdo.cpp"
"E:\车台源码\Linux车台\DVR车台通用版\ComuExe\PhoneGsm.cpp"
"E:\车台源码\Linux车台\DVR车台通用版\ComuExe\PhoneTD.cpp"
"E:\车台源码\Linux车台\DVR车台通用版\ComuExe\PhoneWcdma.cpp"
"E:\车台源码\Linux车台\DVR车台通用版\ComuExe\Sms.cpp"
"E:\车台源码\Linux车台\DVR车台通用版\ComuExe\StackLst.cpp"
]
Creating command line "clmips.exe @C:\DOCUME~1\ADMINI~1\LOCALS~1\Temp\RSP554.tmp" 
Creating temporary file "C:\DOCUME~1\ADMINI~1\LOCALS~1\Temp\RSP555.tmp" with contents
[
commctrl.lib coredll.lib  /nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"WinMainCRTStartup" /incremental:yes /pdb:"MIPSII_FPDbg/ComuExe.pdb" /debug /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib" /out:"MIPSII_FPDbg/ComuExe.exe" /subsystem:windowsce,4.20 /MACHINE:MIPS  
".\MIPSII_FPDbg\ComAdjust.obj"
".\MIPSII_FPDbg\ComuExeMain.obj"
".\MIPSII_FPDbg\ComuStdAfx.obj"
".\MIPSII_FPDbg\ComVir.obj"
".\MIPSII_FPDbg\DiaoDu.obj"
".\MIPSII_FPDbg\GpsSrc.obj"
".\MIPSII_FPDbg\HandApp.obj"
".\MIPSII_FPDbg\Handset.obj"
".\MIPSII_FPDbg\IoSta.obj"
".\MIPSII_FPDbg\LightCtrl2.obj"
".\MIPSII_FPDbg\LightCtrl.obj"
".\MIPSII_FPDbg\PhoneEvdo.obj"
".\MIPSII_FPDbg\PhoneGsm.obj"
".\MIPSII_FPDbg\PhoneTD.obj"
".\MIPSII_FPDbg\PhoneWcdma.obj"
".\MIPSII_FPDbg\Sms.obj"
".\MIPSII_FPDbg\StackLst.obj"
]
Creating command line "link.exe @C:\DOCUME~1\ADMINI~1\LOCALS~1\Temp\RSP555.tmp"
<h3>Output Window</h3>
Compiling...
ComAdjust.cpp
e:\车台源码\linux车台\dvr车台通用版\comuexe\comustdafx.h(4) : fatal error C1083: Cannot open include file: 'pthread.h': No such file or directory
ComuExeMain.cpp
e:\车台源码\linux车台\dvr车台通用版\comuexe\comustdafx.h(4) : fatal error C1083: Cannot open include file: 'pthread.h': No such file or directory
ComuStdAfx.cpp
e:\车台源码\linux车台\dvr车台通用版\comuexe\comustdafx.h(4) : fatal error C1083: Cannot open include file: 'pthread.h': No such file or directory
ComVir.cpp
e:\车台源码\linux车台\dvr车台通用版\comuexe\comustdafx.h(4) : fatal error C1083: Cannot open include file: 'pthread.h': No such file or directory
DiaoDu.cpp
e:\车台源码\linux车台\dvr车台通用版\comuexe\comustdafx.h(4) : fatal error C1083: Cannot open include file: 'pthread.h': No such file or directory
GpsSrc.cpp
e:\车台源码\linux车台\dvr车台通用版\comuexe\comustdafx.h(4) : fatal error C1083: Cannot open include file: 'pthread.h': No such file or directory
HandApp.cpp
e:\车台源码\linux车台\dvr车台通用版\comuexe\comustdafx.h(4) : fatal error C1083: Cannot open include file: 'pthread.h': No such file or directory
Handset.cpp
e:\车台源码\linux车台\dvr车台通用版\comuexe\comustdafx.h(4) : fatal error C1083: Cannot open include file: 'pthread.h': No such file or directory
IoSta.cpp
e:\车台源码\linux车台\dvr车台通用版\comuexe\comustdafx.h(4) : fatal error C1083: Cannot open include file: 'pthread.h': No such file or directory
LightCtrl2.cpp
e:\车台源码\linux车台\dvr车台通用版\comuexe\comustdafx.h(4) : fatal error C1083: Cannot open include file: 'pthread.h': No such file or directory
LightCtrl.cpp
e:\车台源码\linux车台\dvr车台通用版\comuexe\comustdafx.h(4) : fatal error C1083: Cannot open include file: 'pthread.h': No such file or directory
PhoneEvdo.cpp
e:\车台源码\linux车台\dvr车台通用版\comuexe\comustdafx.h(4) : fatal error C1083: Cannot open include file: 'pthread.h': No such file or directory
PhoneGsm.cpp
e:\车台源码\linux车台\dvr车台通用版\comuexe\comustdafx.h(4) : fatal error C1083: Cannot open include file: 'pthread.h': No such file or directory
PhoneTD.cpp
e:\车台源码\linux车台\dvr车台通用版\comuexe\comustdafx.h(4) : fatal error C1083: Cannot open include file: 'pthread.h': No such file or directory
PhoneWcdma.cpp
e:\车台源码\linux车台\dvr车台通用版\comuexe\comustdafx.h(4) : fatal error C1083: Cannot open include file: 'pthread.h': No such file or directory
Sms.cpp
e:\车台源码\linux车台\dvr车台通用版\comuexe\comustdafx.h(4) : fatal error C1083: Cannot open include file: 'pthread.h': No such file or directory
StackLst.cpp
e:\车台源码\linux车台\dvr车台通用版\comuexe\comustdafx.h(4) : fatal error C1083: Cannot open include file: 'pthread.h': No such file or directory
Error executing clmips.exe.



<h3>Results</h3>
ComuExe.exe - 17 error(s), 0 warning(s)
</pre>
</body>
</html>
