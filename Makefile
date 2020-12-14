serial: run_serial.cpp serial_a_star.cpp
	g++ -o serial run_serial.cpp serial_a_star.cpp
	
decent: run_decent.cpp decent_a_star.cpp
	mpicxx -o decent run_decent.cpp decent_a_star.cpp

centralized: run_centralized.cpp centralized_a_star.cpp
	g++ -fopenmp -o centralized run_centralized.cpp centralized_a_star.cpp

clean:
	rm -f decent serial centralized
    
