test: test.cpp serial_a_star.cpp
	g++ -o test test.cpp serial_a_star.cpp

serial: serial_a_star.cpp
	g++ -o serial serial_a_star.cpp

clean:
	rm -f test.exe serial.exe
    