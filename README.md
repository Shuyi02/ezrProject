# ezrProject
repository for our group in the lecture Real Time Rendering at University of Koblenz 

# Licence
I have no idea and hope everything is ok this way. I/We do not intend to make any money with it, it's for pure personal usage to program some cool stuff to show our professor.

#### Setup for our group
##### Prerequisites
- MinGW
- Cmake
- Eclipse for C++
- Git
- Eigen Library (e.g. as unpacked from zip)

##### How to (clone and compile)
- clone into folder you desire *myEzr*
   + with git command: **git clone linkToRepository**
   + URL is on the main page on the right side
- make a build folder *buildEzr* (not the same location as the clone!)
- start cmake (gui)
- in 'source code' browse to the folder, where the repository was cloned to *myEzr*
- in 'build the binaries' browse to folder, where the compiled code should be *buildEzr*
- click 'configure'
- select 'Eclipse CDT4 - MinGW Makefiles', rest stays the same 
- click 'finish'
- hope everything will be alright
- click 'generate'
- hope everything will be alright again x____x

##### Put required header library into the right folder
- libraries like Eigen should be put into the folder "dep"
   + pay attention it should look like project/dep/Eigen and in the Eigen folder all the files that belong to that lib

##### Running the project for first time
- if everything worked, open Eclipse
- import 'existing projecct'
- navigate to the build folder *buildEzr*
- build and hope everything will be alright
- if everything worked again, rightclick on the project
- 'Run As'
- click the symbol for 'new'
- click on the lower 'browse button' and navigate to the exe
   + *buildEzr*/exec/bin/hereIsExe.exe

ok?
