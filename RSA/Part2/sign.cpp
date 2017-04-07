//Author: Karthik Venkat
//Program made with assitance from Adam Browne
//Program to Sign and verify documents
//NOTE: Program assumes it is in the same directory as e_n.txt and d_n.txt

#include "sha256.h"
#include "../Part1/BigIntegerLibrary.hh"
#include "../Part1/BigIntegerAlgorithms.hh"
#include "../Part1/BigIntegerUtils.hh"
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>

//Verifies the signature of the encrypted file
bool signverify(std::string& odata, BigInteger& encrypted_hash, BigInteger& n)
{
  try //try block for verification
  {
	  int pos = odata.find_last_of("-");
	  if(std::string::npos == pos) return false;
	  // we get the hash from the file
	  BigInteger dhash = //split for neatness
    BIGINTEGERUTILS_H::stringToBigInteger(odata.substr(pos+1));
	  //we compute the hash from the file
	  std::string hashval = SHA256_H::sha256(odata.substr(0,pos-6));
	  //we create a biginteger for purposes of conversion from bigint library
	  BigInteger hvsize(hashval.size());

	  const BigInteger M = //split into 3 lines for neatness
    BIGINTEGERUTILS_H::dataToBigInteger<const char>
    (hashval.c_str(), hvsize.getLength(), hvsize.getSign());
    const BigInteger P = BIGINTEGERALGORITHMS_H::modexp(dhash,
    encrypted_hash.getMagnitude(), n.getMagnitude());
    return (M == P);
  }
  catch (const char *e) //failure in the try block is exclusive to modified
  {
    return false;
  }
  return true; //Almost never invoked; here as a warning guard
}

//add signature to file
void sign(std::string& file)
{
	BigInteger skey, n;
	std::ifstream readFile(file);

	if(readFile) //if file is found
  {
		std::stringstream buffer; //buffer to read file into a single string
		buffer << readFile.rdbuf();
		std::string odata = buffer.str();
		readFile.close();
		//we get the sha hash for this file's content.
		std::string hashval = SHA256_H::sha256(odata);
		//we then get the keys
		readFile.open("d_n.txt");
		if(readFile) //if d_n.txt is found
    {
			std::string keyString, bPrimeProduct;
			readFile >> keyString >> bPrimeProduct;
			skey = BIGINTEGERUTILS_H::stringToBigInteger(keyString);
			n = BIGINTEGERUTILS_H::stringToBigInteger(bPrimeProduct);
		}

		BigInteger hvsize(hashval.size()); //for the size of the hashvalue
		BigInteger M = BIGINTEGERUTILS_H::dataToBigInteger<const char>
    (hashval.c_str(), hvsize.getLength(), hvsize.getSign());

		const BigInteger dh = //split for neatness
    BIGINTEGERALGORITHMS_H::modexp(M, skey.getMagnitude(), n.getMagnitude());
    file += ".signed";
		std::ofstream signedFile(file);
		signedFile << odata << "-------" << dh;
	}
  //when the file isnt found in the directory
	else std::cout << "Specified file not in directory/could not be opened\n";
}

//checks if the file is modified or original
void fileverify(std::string& file)
{
	BigInteger pkey, n;
	std::ifstream readFile(file);

	if(readFile) //if file is found
  {
		//we read in the data from the file.
		std::stringstream buffer;
		buffer << readFile.rdbuf();
		std::string odata = buffer.str();
		readFile.close();
		readFile.open("e_n.txt");
		if(readFile) //if e_n.txt is found
    {
			std::string keyString, bPrimeProduct;
			readFile >> keyString >> bPrimeProduct;
			pkey = BIGINTEGERUTILS_H::stringToBigInteger(keyString);
			n = BIGINTEGERUTILS_H::stringToBigInteger(bPrimeProduct);
		}
		bool verified = signverify(odata, pkey, n);
		if(verified) std::cout << "Unmodified\n";
		else std::cout << "Modified\n";
	}
}

//Function to convert char arrays to strings
std::string convertchar(char c[])
{
	std::string out = "";
	int size = strlen(c);
  int i = 0;
	while(i < size) out += c[i++];
	return out;
}

//main function for verifying / signing files.
int main(int argc, char *argv[])
{
	std::string fname = convertchar(argv[2]);
	if(*argv[1] == 'v') fileverify(fname);
	else if(*argv[1] == 's') sign(fname);
	else std::cout << "v and s are the only valid arguments\n";

  return 0;
}
