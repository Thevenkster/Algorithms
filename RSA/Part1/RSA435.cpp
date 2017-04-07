//Author: Karthik Venkat
//Program for part1 of the RSA project
//Program generates n, p, e and d values and writes them to the respective files

#include <string>
#include <iostream>
#include <fstream>
#include <ctime>

// `BigIntegerLibrary.hh' includes all of the library headers.
#include "BigIntegerLibrary.hh"
// 'BigIntegerAlgorithms.hh' includes modexp
#include "BigIntegerAlgorithms.hh"

//Function to perform fermat's test 3 times on a randomly genrated bigint
//using the modexp function provided in the library
bool fermat(BigInteger &p)
{
    for (int i =0; i < 3; i++)
    {
      BigInteger base = BigInteger(rand()) % (p - 1) + 1;
      BigInteger value = BIGINTEGERALGORITHMS_H::modexp(base, //split: neatness
                         p.getMagnitude()-1, p.getMagnitude());
      if(value != 1)
         return false;
   }
   return true;
}

//function that genrates a very large integer and tests for primality with
//about 156 digits, roughly 512 bits in size
BigInteger generate()
{
  BigInteger a = 1;
  for (int i = 1; i < 155; i++) a = a * 10 + (rand() % 10);
  while (fermat(a) == false) a = generate();
  return a;
}

//Function to generate the e value; since all prime numbers are inherently
//coprime, we simply generate another very large prime number for the e value
BigInteger ecompute(BigInteger &n)
{
   BigInteger e = generate();
   while(e < 0 || e > n) e = generate();
   return e;
}

//function that computes d using the modinv function provided in the library
//which performs the extended euclidian algorithm, also included in the library
BigInteger dcompute(BigInteger &phi, BigInteger &e)
{
  BigInteger d = BIGINTEGERALGORITHMS_H::modinv(e.getMagnitude(),//neatness
                 phi.getMagnitude());
  if (d < 0) d += phi;
  return d;
}

int main()
{
    //seed for rand()
    srand(time(0));

    BigInteger p = generate();
    BigInteger q = generate();

    //compute n
    BigInteger n = p*q;
    BigInteger phi = (p-1) * (q-1);

    //generate and compute our d and e values
    BigInteger e = ecompute(n);
    BigInteger d = dcompute(phi, e);

    //streams to open, write to and close our files
    std::ofstream textfile;
    textfile.open("p_q.txt");
    textfile << p << std::endl;
    textfile << q << std::endl;
    std::cout << p << std::endl;
    std::cout << q << std::endl;
    textfile.close();

    textfile.open("e_n.txt");
    textfile << e << std::endl;
    textfile << n << std::endl;
    textfile.close();

    textfile.open("d_n.txt");
    textfile << d << std::endl;
    textfile << n << std::endl;
    textfile.close();

  	return 0;
  }
