# EasyPublicPrivate
Simple RSA encryption using public and private keys. Carefully commented 
for learners. RSA.cpp is the simplest code. RSA_omp.cpp has a few changes
to facilitate OpenMP parallelization and includes pragma directives for
OpenMP.

**Build**
   make PublicPrivate 
   (Uses g++)
To build with OpenMP parallelization, add OMP=1 to the make line
   
   
   
**Run**
   ./PublicPrivate < in.txt
Decryption can take a very long time with this simple implementation. You can run without taking input from in.txt. In that case, you will be prompted to enter two prime numbers and a message to be encrypted.

**Multiple Versions**
Multiple versions of the code are provided to give insight into speeding up encryption/decryption. Below are short descriptions of each version
RSA.cpp i         - Simplest implementation
RSA_omp.cpp       - Adds OpenMP directives to use all processor cores.
                    Set OMP_NUM_THREADS appropriately or leave it undefined
                    to use all cores
RSA_fastexp.cpp   - Faster exponetiation for vastly improved performance.
                    The change is described in the comment preceeding the
                    fastexp() function.
