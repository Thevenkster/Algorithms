//Karthik Venkat-kv39@zips.uakron.edu
//Description: Modified LZW for variable bit compression and decompression(pt2)


//Done with assistance from Adam Browne & Sam Borick
#include <sstream>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <sys/stat.h>

//count the number of the bits
int bitcount(int value)
{
  int count;
  for(count = 0; value > 0; value >>= 1) ++count;
  return count;
}

//from original
std::string int2BinaryString(int c)
{
      int cl = 9;
      if(c >= 512)
        cl = bitcount(c);
      std::string p = ""; //a binary code string with code length = cl
      while (c>0)
      {
		   if (c%2==0) p="0"+p;
         else p="1"+p;
         c=c>>1;
      }
      int zeros = cl-p.size();
      if (zeros<0)
      {
         std::cout << "\nWarning: Overflow. code is too big to be coded by "
                   << cl <<" bits!\n";
         p = p.substr(p.size()-cl);
      }
      else
      {
        for (int i=0; i<zeros; i++)//pad 0s to left of the binary code if needed
            p = "0" + p;
      }
      return p;
}

//from original
std::string int2BinaryString(int c, int cl)
{
      std::string p = ""; //a binary code string with code length = cl
      while (c>0)
      {
       if (c%2==0) p="0"+p;
       else p="1"+p;
       c=c>>1;
      }
      int zeros = cl-p.size();
      if (zeros<0)
      {
         std::cout << "\nWarning: Overflow. code is too big to be coded by "
                   << cl <<" bits!\n";
         p = p.substr(p.size()-cl);
      }
      else {
        for (int i=0; i<zeros; i++)//pad 0s to left of the binary code if needed
            p = "0" + p;
      }
      return p;
}

//from original
int binaryString2Int(std::string p)
{
   int code = 0;
   if (p.size()>0)
   {
      if (p.at(0)=='1') code = 1;
      p = p.substr(1);
      while (p.size()>0)
      {
         code = code << 1;
		   if (p.at(0)=='1') code++;
         p = p.substr(1);
      }
   }
   return code;
}

//Extracts string content for compression
std::string databuffer(std::ifstream& fileReader)
{
	std::stringstream buffer;
	buffer << fileReader.rdbuf();
	return buffer.str();
}

// Compress a string to a list of output symbols.
// The result will be written to the output iterator
// starting at "result"; the final iterator is returned.
std::string compress(std::string filename)
{
  std::ifstream fileReader(filename);
  const std::string uncompressed = databuffer(fileReader);
  std::string output = "";

  // Build the dictionary.
  int dictSize = 256;
  std::map<std::string,int> dictionary;
  for (int i = 0; i < 256; i++)
    dictionary[std::string(1, i)] = i;

  std::string w;
  for (std::string::const_iterator it = uncompressed.begin();
       it != uncompressed.end(); ++it) {
    char c = *it;
    std::string wc = w + c;
    if (dictionary.count(wc))
      w = wc;
    else {
      output += int2BinaryString(dictionary[w], bitcount(dictSize));
      if(dictSize < 65536) {
        dictionary[wc] = dictSize++;
      }
      w = std::string(1, c);
    }
  }

  // Output the code for w.
  if (!w.empty()) output += int2BinaryString(dictionary[w],bitcount(dictSize));
  return output;
}

// Decompress a list of output ks to a string.
// "begin" and "end" must form a valid range of ints
std::string decompress(std::string& encoded)
{
  // Build the dictionary.
  int dictSize = 256;
  std::map<int,std::string> dictionary;
  for (int i = 0; i < 256; i++) dictionary[i] = std::string(1, i);

  //requirements for variable bitlengths [9,16]
  int index = 0, curBit = 9, curNum;
  std::string entry, curSequence;
  //we get the first letter, as is custom, so not dealt with further:
  std::string w(1, binaryString2Int(encoded.substr(index,curBit)));
  std::string result = w;
  index += curBit;
  while(index < encoded.size())
  {
    curSequence = encoded.substr(index, curBit);
    curNum = binaryString2Int(curSequence);

    if (dictionary.count(curNum)) entry = dictionary[curNum];
    else if (curNum == dictSize) entry = w + w[0];
    else throw "Bad compressed k";

    result += entry;
    if(dictSize < 65536)
    {
      // Add w+entry[0] to the dictionary.
      dictionary[dictSize++] = w + entry[0];
      curBit = bitcount(dictSize);
    }

    w = entry;
    index += curBit;
  }
  return result;
}

//converts strings to ints
std::vector<int> binaryToInts(std::string& bstring)
{
  std::vector<int> output;
  int i = 0;
  while(i <= bstring.length())
  {
    output.push_back(binaryString2Int(bstring.substr(i,9)));
    i += 9;
  }
  //for trailing newlines
  i = output.size() - 1;
  while(output[i--] == 0) output.pop_back();
  return output;
}

//creates the file with binaries
void binarywriter(std::string& filename, std::string& bin_string)
{
  std::ofstream write;
  write.open(filename.c_str(), std::ios::binary);
  std::string zeros = "00000000";

  //to find out what the padding is
  int padsize = 8 - bin_string.size() % 8;
  std::string pad = int2BinaryString(padsize, 8);
  bin_string = pad + zeros.substr(0, padsize) + bin_string;
  int b;
  for(int i=0; i<bin_string.size(); i+=8)
  {
    b = 1;
    for(int j=0;j<8;++j)
    {
      b = b<<1;
      if(bin_string.at(i+j) == '1') ++b;
    }
    char c = (char) (b & 255);
    write.write(&c, 1);
  }
  write.close();
}

//modifies decompressed file name to distingiush it from the original
std::string filedistinguisher(std::string& filename)
{
  int period = filename.find("."); //locates first period in the file
  std::string file = "";
  for(int i=0; i< period; ++i) file += filename[i];
  file += "2M.", ++period;
  while(period < filename.size() && filename[period] != '.')
    file += filename[period++];
	return file;
}

//reads contents of file, compresses it and creates compressed file
void filecompress(std::string& filename)
{
  std::string inbinary = compress(filename);
  std::string extension = filename + ".lzw2";
  binarywriter(extension, inbinary);
}

void fileexpand(std::string& filename)
{
  std::ifstream filestream(filename, std::ios::binary);

  struct stat status;
  stat(filename.c_str(), &status);
  long size = status.st_size; //size in bytes

  char c2[size];
  filestream.read(c2, size);
  long count = 0;

  std::string zeros = "00000000";
  std::string str = "";
  while(count < size)
  {
    unsigned char charshift = (unsigned char) c2[count++];
    std::string p = "";
    for(int j=0; j<8 && charshift>0; ++j)
    {
      if (charshift%2==0) p="0"+p;
      else p="1"+p;
      charshift=charshift>>1;
    }
    p = zeros.substr(0,8-p.size()) + p;
    str += p;
  }

  int pad = binaryString2Int(str.substr(0,8));
  str = str.substr(8+pad);
  std::string decompressed = decompress(str);
  std::ofstream write(filedistinguisher(filename));
  write << decompressed;
}

//helper for parsing filename in main
std::string stringhelper(char arr[])
{
	std::string output = "";
	int size = strlen(arr), i = 0;
	while(i < size) output += *(arr + i++);
	return output;
}

int main(int argc, char *argv[])
{
	std::string filename = stringhelper(argv[2]);
	if (*argv[1] == 'c') filecompress(filename);
	else if (*argv[1] == 'e') fileexpand(filename);
  else std::cout << "Re-enter arguments with c to compress and e to expand\n";

  return 0;
}
