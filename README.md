# Parallel_A_Star
Parallel A* for CMSC498X Group 8's final project

Configuration:
In order to properly compile and run the programs on Deepthought2 you must first load the needed OpenMPI and GCC modules with the below command.

  "module load openmpi/gcc"
  
Compilation:
There are three unique targets for compilation in the Makefile. You may use "make serial", "make decent", or "make centralized" to compile the serial, decentralized or centralized implementations of A* respectively. You may also use "make all" to compile all three implementations at once. You may use "make clean" to delete all executable files at once.

Running the serial program:
To run the serial program after it is complied use the name of the executable "serial" followed by the name of the input file you wish to use followed by the sizes of the X and Y of the graph. An example is below.

  ./serial input/256x256-input.txt 256 256
  
Note that the input files are in the directory named input so you will need to place that infront of the name of the text imput file.

Running the centralized program:
Similar to the serial program we will need to provide a input file and its size however, this program utilizes OpenMP and the number of threads to run the program with needs to be specified. See below for an example.

  export OMP_NUM_THREADS=16
  
  ./centralized input/256x256-input.txt 256 256
  
Running the decentralized program:
Once again similar to the above programs we will need to provide the input file and the size of the graph in that input file. However this program uses MPI so you need to use the "mpirun" command and you will need to specify how many processes you wish to use. Below is an example.

  mpirun -n 16 ./decent input/256x256-input.txt 256 256
  
Output:
When running on deepthought2 each of the programs will output into the slurm file for its job number. The output will show the shortest path from the start node to the goal node by displaying each edge traversed and the weight of each edge. The total of all of these weights will also be displayed. Additionally, the runtime in seconds of the A* algorithm will also be displayed.
  
