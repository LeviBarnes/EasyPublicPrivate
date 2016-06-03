#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <iostream>
#include <string.h>

#define MAX_STR_LEN 1000

long int prime(long int);
long int gcd(long int p, long int q);
int publickey(long int p, long int q, long int* exp, long int* mod);
int privatekey(long int p, long int q, long int pubexp, long int* exp, long int* mod);
int encrypt(long int* inmsg, long int, long int, long int* outmsg, size_t len);
int decrypt(long int* inmsg, long int, long int, long int* outmsg, size_t len);
int char2long(char* in, long int* out, bool random_salt=false);
int long2char(long int* in, char* out, bool subtract_pairs=false);

long int fastexp(long int base, long int exp, long int mod);

int main(void) {

   long int p,q, pube, pubmod, prive, privmod;
   char inmsg[MAX_STR_LEN];
   long int inmsg_l[MAX_STR_LEN];
   char outmsg[MAX_STR_LEN];
   long int outmsg_l[MAX_STR_LEN];
   char decrmsg[MAX_STR_LEN];
   long int decrmsg_l[MAX_STR_LEN];

   size_t len;

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
   len = strlen(inmsg);

   //Generate public and private keys from p and q
   publickey(p,q,&pube,&pubmod);
   std::cout << "public key: " << pube << ", " << pubmod << std::endl;
   privatekey(p,q,pube,&prive,&privmod);
   std::cout << "private key: " << prive << ", " << privmod << std::endl;

   //Encrypt, then decrypt the message
   std::cout << "Original text: " << inmsg << std::endl;
       //Convert to long ints
   char2long(inmsg, inmsg_l,true);

       //Encrypt
   
   encrypt(inmsg_l, pube, pubmod, outmsg_l, len*2);
   long2char(outmsg_l, outmsg,true);
   std::cout << "Encrypted text: " << outmsg << std::endl;


       //Decrypt
   decrypt(outmsg_l, prive, privmod, decrmsg_l, len*2);
   long2char(decrmsg_l, decrmsg,true);
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

int encrypt(long int* in, long int exp, long int mod, long int* out, size_t len)
//Encrypt an array of long ints
//exp and mod should be the public key pair
//Each number, c, is encrypted by 
// c' = (c^exp)%mod
{
   #pragma acc parallel loop
   #pragma omp parallel for
   for (int i=0; i < len; i++)
   {
      long int c = in[i];
      #if 1
      out[i] = fastexp(c, exp, mod);
      #else
      //This is the slow way to do exponentiation
      for (int z=1;z<exp;z++)
      {
         c *= in[i];
         c %= mod; //We can exploit the fact that (a*b)%c = ((a%c)*b)%c
                   //   to keep the numbers from getting too large
      }
      out[i] = c; 
      #endif
   }
   out[len]='\0'; //Terminate with a zero
   return 0;
}

int decrypt(long int* in, long int exp, long int mod, long int* out, size_t len)
//Decrypt an array of long ints
//exp and mod should be the private key pair
//Each number, c', is decrypted by 
// c = (c'^exp)%mod
{
   #pragma acc parallel loop
   #pragma omp parallel for
   for (int i=0; i < len; i++)
   {
      long int c = in[i];
      #if 1
      out[i] = fastexp(c, exp, mod);
      #else
      //This is the slow way to do exponentiation
      for (int z=1;z<exp;z++)
      {
         c *= in[i];
         c %= mod; //We can exploit the fact that (a*b)%c = ((a%c)*b)%c
                   //   to keep the numbers from getting too large
      }
      out[i] = c; 
      #endif
   }
   out[len]='\0'; //Terminate with a zero
   return 0;
}

long int fastexp(long int base, long int exp, long int mod)
//Faster modular exponetiation
//Essentially, we express the exponent as a binary number, 
//  then move one bit at a time multiplying output by
//  base^(2^b) in the case of a 1 bit. b, here, is the bit
//  position. Rather than recompute base^(2^b) each time
//  we just generate base^(2^b) by squaring base^(2^(b-1))
//  which is the number used for the last bit.
//  All multiplications are modular with the given
//  modulus, mod.

//Note that dividing by 2 is the same as a bit shift by one.
{
   long int out = 1;
   while(exp>0)
   {
      if(1==exp%2) 
      {
         out*=base;
         out%=mod;
      }  
      base=base*base;
      base%=mod;
      exp/=2;
   }
   return out;
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

int long2char(long int* in, char* out, bool subtract_pairs)
//Converts a list of long ints to char 
//Using automatic type conversion
//Useful for outputting to stdout
{
   while(*in != 0 || *(in+1) != 0) 
   {
      long int r = 0;
      if (subtract_pairs)
      {
         r = *in++;
      }
      *out++ = (char)(*in++)-r;
   }
   *out = '\0';
   return 0;
}

int char2long(char* in, long int* out, bool random_salt)
//Converts a list of chars to long ints
//Using automatic type conversion
//Useful for converting input from stdin
{
   while(*in != '\0') 
   {
      long int r = 0;
      if (random_salt)
      {
         r = rand()%INT_MAX - INT_MAX/2;
         *out++ = r;
      }
      *out++ = (long int)(*in++) + r;
   }
   *out++ = 0;
   *out = 0;
   return 0;
}
