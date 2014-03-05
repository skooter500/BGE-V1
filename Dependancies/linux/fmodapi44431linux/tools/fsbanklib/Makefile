x86: example.cpp
	g++ -O2 -m32 -o example $< libfsbankex.a ../../api/lib/libfmodex.so

x64: example.cpp
	g++ -O2 -m64 -o example $< libfsbankex64.a ../../api/lib/libfmodex64.so

clean:
	rm -f example
