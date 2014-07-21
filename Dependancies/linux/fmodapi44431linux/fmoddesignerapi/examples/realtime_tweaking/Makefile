x86: main.cpp
	g++ -O2 -m32 -o example -I../../../api/inc $< ../../../api/lib/libfmodex.so ../../api/lib/libfmodeventnet.so -Wl,-rpath ../../../api/lib

x64: main.cpp
	g++ -O2 -m64 -o example -I../../../api/inc $< ../../../api/lib/libfmodex64.so ../../api/lib/libfmodeventnet64.so -Wl,-rpath ../../../api/lib

x86_c: main.c
	g++ -O2 -m32 -o example -I../../../api/inc $< ../../../api/lib/libfmodex.so ../../api/lib/libfmodeventnet.so -Wl,-rpath ../../../api/lib

x64_c: main.c
	g++ -O2 -m64 -o example -I../../../api/inc $< ../../../api/lib/libfmodex64.so ../../api/lib/libfmodeventnet64.so -Wl,-rpath ../../../api/lib

clean:
	rm -f example
