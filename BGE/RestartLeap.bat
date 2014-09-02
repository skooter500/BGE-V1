net stop LeapService
ping 127.0.0.1 -n 1 -w 1000 > nul
net start LeapService
