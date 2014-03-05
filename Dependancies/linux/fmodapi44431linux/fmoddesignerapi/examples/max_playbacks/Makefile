x86: main.cpp
	g++ -O2 -m32 -o example -I../../../api/inc $< ../../../api/lib/libfmodex.so ../../api/lib/libfmodevent.so -Wl,-rpath ../../../api/lib

x64: main.cpp
	g++ -O2 -m64 -o example -I../../../api/inc $< ../../../api/lib/libfmodex64.so ../../api/lib/libfmodevent64.so -Wl,-rpath ../../../api/lib

x86_c: main.c
	g++ -O2 -m32 -o example -I../../../api/inc $< ../../../api/lib/libfmodex.so ../../api/lib/libfmodevent.so -Wl,-rpath ../../../api/lib

x64_c: main.c
	g++ -O2 -m64 -o example -I../../../api/inc $< ../../../api/lib/libfmodex64.so ../../api/lib/libfmodevent64.so -Wl,-rpath ../../../api/lib

clean:
	rm -f example
