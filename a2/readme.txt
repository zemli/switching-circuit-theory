Implementation  

Run Solution Package
To run the program, download the zip file and unzip it to a destination folder. 
Find the assign2.sln file in the directory and open it with Microsoft Visual Studio.  
Edit solution property and input test BLIF file and operation command (i.e. apply, build or sifting)  in ¡°command arguments window¡±
Lastly, run the program by  clicking Load Windows Debugger. 


Run executable file
An executable file (assign2.exe) was generated and can be directly run in command prompt.  
a) Download the file assign2 to a folder of choice and put test node files (.blif) under the same directory. To test build and sifting function, the test node file must contain one function. To test apply function, the test node file must contain two functions.  
b) Rename file name to assign2.exe  
c) Open command prompt and use change directory command (cd) to the destination folder. The command prompt will run assign1.exe and the result will be printed in the same command window screen. 
d) Testing
Test Build function£º Input command ¡°assign2.exe <source BLIF file> build¡± 
(e.g. assign1.exe node1.blif build) to execute assign2 with input data in node1.blif
Test Sifting function£º Input command ¡°assign2.exe <source BLIF file> sifting¡± 
(e.g. assign1.exe node1.blif sifting) to execute assign2 with input data in node1.blif
Test Apply function£º Input command ¡°assign2.exe <source BLIF file> apply¡±  
(e.g. assign1.exe node12.blif apply) to execute assign2 with input data in node12.blif (there are two functions in node12.blif)

CUDD Implementation 
Download CUDD package and  unzip it to a destination folder. We implemented apply algorithm based on the main function of nanotrav package in CUDD. Compiling this package follows the same method as compiling nanotrav package. Specifically, refer to  the following commands. 
Change directory to  folder cudd_3.0.0/nanotrav 
and input the following command:  ¡°execute ./nanotrav <file1.blif> -second <file2.blif> -apply <operation>¡±
(e.g.: ./nanotrav a.blif -second b.blif -apply xnor) to execute and operation for two logic functions.
