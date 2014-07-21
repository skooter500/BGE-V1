x86: main.c
	g++ -O2 -m32 -shared -fpic -o example.so $<

x64: main.c
	g++ -O2 -m64 -shared -fpic -o example.so $<

clean:
	rm -f example.so
