cc = "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC\14.29.30133\bin\Hostx64\x64\cl.exe"
cflags = /I"C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC\14.29.30133\include" /I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\shared" /I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\ucrt" /I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\um" /I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\winrt" 
ldflags =  /LIBPATH:./ /LIBPATH:"C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC\14.29.30133\lib\x64" /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.20348.0\ucrt\x64" /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.20348.0\um\x64"
libs = Ole32.lib 	User32.lib


dll:
	$(cc) /DBUILDNOTIAPI /std:c++17 $(cflags) /Ot /GL /c Notification.cc /Fo:Notification.obj
	$(cc) /LD Notification.obj /link $(ldflags) $(libs) /OUT:Notification.dll /IMPLIB:Notification.lib

dllcs:
	$(cc) /DBUILDNOTIAPI /D__PowerShell /std:c++17 $(cflags) /Ot /GL /c Notification.cc /Fo:Notification.obj
	$(cc) /LD Notification.obj /link $(ldflags) $(libs) /OUT:Notification.dll /IMPLIB:Notification.lib

cdll:
	$(cc) /DBUILDNOTICAPI /std:c++17 $(cflags) /Ot /GL /c NotificationAPI.cc /Fo:NotificationAPI.obj
	$(cc) /LD NotificationAPI.obj /link $(ldflags) $(libs) Notification.lib /OUT:NotificationAPI.dll /IMPLIB:NotificationAPI.lib