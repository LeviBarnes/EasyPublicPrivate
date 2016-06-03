#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <iostream>

#define MAX_STR_LEN 1000

long int prime(long int);
long int gcd(long int p, long int q);
int publickey(long int p, long int q, long int* exp, long int* mod);
int privatekey(long int p, long int q, long int pubexp, long int* exp, long int* mod);
int encrypt(long int* inmsg, long int, long int, long int* outmsg);
int decrypt(long int* inmsg, long int, long int, long int* outmsg);
int char2long(char* in, long int* out);
int long2char(long int* in, char* out);

int main(void) {

   long int p,q, pube, pubmod, prive, privmod;
   char inmsg[MAX_STR_LEN];
   long int inmsg_l[MAX_STR_LEN];
   char outmsg[MAX_STR_LEN];
   long int outmsg_l[MAX_STR_LEN];
   char decrmsg[MAX_STR_LEN];
   long int decrmsg_l[MAX_STR_LEN];

   //Get inputs
   // - two prime numbers
   // - a message to be encrypted
   std::cout << "ENTER A PRIME NUMBER" << std::endl;
   std::cin >> p;
   if (prime(p)) 
   {
      std::cerr << p << " is not prime." << std::endl;
      return 1;
   }
   std::cout << "ENTER ANOTHER PRIME NUMBER" << std::endl;
   std::cin >> q;
   if (prime(q)) 
   {
      std::cerr << q << " is not prime." << std::endl;
      return 1;
   }

   std::cout << "ENTER A MESSAGE (up to MAX_STR_LEN characters)." << std::endl;
   std::cin.ignore(INT_MAX,'\n');
   std::cin.getline(inmsg, MAX_STR_LEN);
   std:: cout << inmsg << std::endl;

   //Generate public and private keys from p and q
   publickey(p,q,&pube,&pubmod);
   std::cout << "public key: " << pube << ", " << pubmod << std::endl;
   privatekey(p,q,pube,&prive,&privmod);
   std::cout << "private key: " << prive << ", " << privmod << std::endl;

   //Encrypt, then decrypt the message
   std::cout << "Original text: " << inmsg << std::endl;
       //Convert to long ints
   char2long(inmsg, inmsg_l);

       //Encrypt
   encrypt(inmsg_l, pube, pubmod, outmsg_l);
   long2char(outmsg_l, outmsg);
   std::cout << "Encrypted text: " << outmsg << std::endl;


       //Decrypt
   decrypt(outmsg_l, prive, privmod, decrmsg_l);
   long2char(decrmsg_l, decrmsg);
   std::cout << "Decrypted text: " << decrmsg << std::endl;
}


long int prime(long int p) 
//returns zero for prime numbers
{
   long int j = sqrt(p);
   for (long int z=2;z<j;z++) if (0==p%z) return z;
   return 0;
}

int publickey(long int p, long int q, long int *exp, long int *mod)
//Generates a public key pair
//The modulus is given by (p-1)*(q-1)
//The exponent is any integer coprime of the modulus
{

   *mod = (p-1)*(q-1);
   //Choose an integer near sqrt(mod)
   *exp = (int)sqrt(*mod);
   //Find a coprime near that number 
   while (1!=gcd(*exp,*mod))  
   {
      (*exp)++;
   }
   *mod = p*q;
   return 0;
}

int privatekey(long int p, long int q, long int pubexp, long int *exp, long int *mod)
//Generates a private key pair
//The modulus is given by (p-1)*(q-1)
//The exponent is the number, n, which satisfies (n * pubexp) % mod = 1
{
   *mod = (p-1)*(q-1);
   *exp = 1;
   long int tmp=pubexp;
   while(1!=tmp%*mod)
   {
      tmp+=pubexp;
      tmp%=*mod; //We can exploit the fact that (a*b)%c = ((a%c)*b)%c 
                 //   to keep the numbers from getting too large
      (*exp)++;
   }
   *mod = p*q;
   return 0;
}

int encrypt(long int* in, long int exp, long int mod, long int* out)
//Encrypt an array of long ints
//exp and mod should be the public key pair
//Each number, c, is encrypted by 
// c' = (c^exp)%mod
{
   //Note: pc is a pointer
   for (long int *pc = in; *pc != '\0';pc++,out++)
   {
      long int c = *pc;
      for (int z=1;z<exp;z++)
      {
         c *= *pc;
         c %= mod; //We can exploit the fact that (a*b)%c = ((a%c)*b)%c
                   //   to keep the numbers from getting too large
      }
      *out = c; 
   }
   *out='\0'; //Terminate with a zero
   return 0;
}

int decrypt(long int* in, long int exp, long int mod, long int* out)
//Decrypt an array of long ints
//exp and mod should be the private key pair
//Each number, c', is decrypted by 
// c = (c'^exp)%mod
{
   for (long int *pc = in; *pc != '\0';pc++,out++)
   {
      long int c = *pc;
      for (int z=1;z<exp;z++)
      {
         c *= *pc;
         c %= mod; //We can exploit the fact that (a*b)%c = ((a%c)*b)%c
                   //   to keep the numbers from getting too large
      }
      *out = c; 
   }
   *out='\0'; //Terminate with a zero
   return 0;
}



long int gcd(long int p, long int q) 
//greatest common devisor (AKA greatest common factor)
// by Euclid's method
{
   if (p<q) {long int tmp=p;p=q;q=tmp;}
   while (q!=0)
   {
      //In each step the new p is the old q and the new q is p%q
      //     p <- q
      //     q <- p%q
      //The last modular remainder will be 0.
      //The next to last modular remainder is the GCD.
      long int tmp = q;
      q = p%q;
      p = tmp;
   }

   return p;
}

int long2char(long int* in, char* out)
//Converts a list of long ints to char 
//Using automatic type conversion
//Useful for outputting to stdout
{
   while(*in != 0) *out++ = (char)(*in++);
   *out = '\0';
   return 0;
}

int char2long(char* in, long int* out)
//Converts a list of chars to long ints
//Using automatic type conversion
//Useful for converting input from stdin
{
   while(*in != '\0') *out++ = (long int)(*in++);
   *out = 0;
   return 0;
}
