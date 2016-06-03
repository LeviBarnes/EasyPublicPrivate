# EasyPublicPrivate
Simple RSA encryption using public and private keys. Carefully commented for learners.

Build:
   g++ -o PublicPrivate RSA.cpp
   
Run:
   ./PublicPrivate < in.txt
Decryption can take a very long time with this simple implementation. You can run without taking input from in.txt. In that case, you will be prompted to enter two prime numbers and a message to be encrypted.


