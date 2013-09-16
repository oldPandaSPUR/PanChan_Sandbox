all: PanChan_Sandbox.exe

clean:
	rm PanChan_Sandbox.o PanChan_Sandbox.exe

PanChan_Sandbox.exe: PanChan_Sandbox.o
	g++ -g -o PanChan_Sandbox.exe PanChan_Sandbox.o
	
PanChan_Sandbox.o:
	g++ -c -g PanChan_Sandbox.cpp
