#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>

struct imgsc //claas for the seam-carve image
{
    int** image; //pointer for 2D image vector
    int** pem; //pointer for 2D pixel energy matrix vector
    int width;
    int height;
    std::string max; //for maximum greyscale value

   imgsc() = default; //Default constructor

   imgsc(std::string filename) //Constructor with filename
   {
        pullimg(filename);
        pem = NULL;
        pixelmatrix();
   }

   void  pullimg(std::string filename) //retrieves image
   {
      std::string temp;
      std::ifstream infile;
      int steps = 1;
      std::vector<std::string> content; //Content vector

      //Open the file
      infile.open(filename.c_str());
      if(!infile) std::cout << "cannot be opened" << std::endl;

      //Collect the file into a string
      while(getline(infile, temp))
      {
          if(temp[0] == '#') continue; //To account for comments in the file

          else if(steps == 1) steps++;

          else if(steps == 2)
          {
              //Size of the image
              int space = temp.find(" ");
              std::string x = temp.substr(0, space); //x component
              std::string y = temp.substr(space + 1); //y component

              width = atoi(x.c_str());
              height = atoi(y.c_str());

              image = new int*[width];
              for(int i = 0; i < width; i++) image[i] = new int[height];
              steps++;
          }

          else if(steps == 3)
          {
              //Maximum greyscale value
              max = temp;
              steps++;
          }

          else
          {
              //Content of the image
              std::string numstr = "";
              int strlength = temp.length();
              for(int i = 0; i < strlength; i++)
              {
                  if(temp[i] == ' ' || temp[i] == '\t' || temp[i] == '\n')
                  {
                      content.push_back(numstr);
                      numstr = "";
                  }
                  else numstr += temp[i];
              }
              if(numstr != "") content.push_back(numstr);
          }
      }

      infile.close();

      //Fill image with content from content vector
      int vit = 0; //vector iterator
      for(int y = 0; y < height; y++)
      {
          for(int x = 0; x < width; x++)
          {
              std::string curr = content.at(vit); //current content iterator
              image[x][y] = atoi(curr.c_str());
              vit++;
          }
      }
  }


  int  energy(int x, int y) //calculate pixel energy
  {
      int current = image[x][y]; //current pixel
      int up;
      int down;
      int right;
      int left;

      if(x == 0) left = image[x][y];
      else left = image[x-1][y];

      if(y == 0) up = image[x][y];
      else up = image[x][y-1];

      if(x == width - 1) right = image[x][y];
      else right = image[x+1][y];

      if(y == height - 1) down = image[x][y];
      else down = image[x][y+1];

      int result = abs(current - up) + //Split for neatness
                  abs(current - down) +
                  abs(current - right) +
                  abs(current - left);

      return result;
  }

  void  pixelmatrix() //generate the pixel energy matrix for image
  {
      //Delete old
      if(pem != NULL)
      {
          for(int i = 0; i < width - 1; i++) delete [] pem[i];
          delete [] pem;
      }

      //Create new
      pem = new int*[width];
      for(int i = 0; i < width; i++) pem[i] = new int[height];

      for(int y = 0; y < height; y++)
        for(int x = 0; x < width; x++) pem[x][y] = energy(x,y);
  }

  void  carve(int vertical, int horizontal)
  {
      for(int i = 0; i < vertical; i++)
      {
          seamcarve();
          pixelmatrix();
      }

      transpose(); //transpose image for carves in horizontal direction

      for(int i = 0; i < horizontal; i++)
      {
          seamcarve();
          pixelmatrix();
      }

      transpose(); //transpose back to original orientation
  }


  void transpose() //for transposing image's orientation
  {
      for(int i = 0; i < width - 1; i++) delete [] pem[i];

      delete [] pem;
      pem = 0;

      int orgw = width; //original width
      int orgh = height; //original height

      height = orgw;
      width = orgh;

      int** img = image;
      image = new int*[width];
      for(int i = 0; i < width; i++) image[i] = new int[height];

      for(int y = 0; y < height; y++)
          for(int x = 0; x < width; x++) image[x][y] = img[y][x];

      pixelmatrix();

      for(int i = 0; i < height; i++) delete [] img[i];

      delete [] img;
  }

  void  seamcarve() //carve seams in vertical direction
  {
      //for cumulative energy matrix
      int** ce = new int*[width]; //ce == cumulative Energy
      for(int i = 0; i < width; i++) ce[i] = new int[height];

      for(int y = 0; y < height; y++)
      {
          for(int x = 0; x < width; x++)
          {
              if(y == 0) ce[x][y] = pem[x][y];
              else
              {
                  int prevminneighbour; //minimum previous neighbour
                  if(x == 0)
                  {
                      prevminneighbour = std::min(ce[x+1][y-1],
                        ce[x][y-1]); //Split for neatness
                  }
                  else if(x == (width - 1))
                  {
                      prevminneighbour = std::min(ce[x-1][y-1],
                        ce[x][y-1]); //Split for neatness
                  }
                  else
                  {
                      prevminneighbour = std::min(
                        std::min(ce[x-1][y-1],
                          ce[x+1][y-1]),
                          ce[x][y-1]); //Split for neatness
                  }
                  ce[x][y] = pem[x][y] +
                  prevminneighbour; //Split for neatness
              }
          }
      }

      //locate Seam
      int s = 0;

      for(int i = 0; i < width; i++)
          if(ce[i][height-1] < ce[s][height-1]) s = i;

      image[s][height-1] = -1;

      for(int i = 1; i < height; i++)
      {
          int currht = height - 1 - i;
          int ri = s + 1; //right index
          int ci = s; //center index
          int li = s - 1;

          if(ri < 0 || ri >= width) ri = ci;
          if(li < 0 || li >= width) li = ci;

          int smv = std::min(
            std::min(ce[ri][currht],
              ce[li][currht]),
              ce[ci][currht]);//Split for neatness; smv==smallest value


          if (ce[li][currht] == smv) s = li;
          else if (ce[ci][currht] == smv) s = ci;
          else if (ce[ri][currht] == smv) s = ri;

          image[s][currht] = -1;
      }

      int orgw = width; //original width
      width--;
      std::vector<int> content;

      //Collect rest of image
      for(int y = 0; y < height; y++)
      {
          for(int x = 0; x < orgw; x++)
            if(image[x][y] != -1) content.push_back(image[x][y]);
      }

      //Delete old Image
      for(int i = 0; i < width; i++) delete [] image[i];
      delete [] image;

      //Create new Image
      image = new int*[width];

      for(int i = 0; i < width; i++) image[i] = new int[height];

      int vit = 0;
      for(int y = 0; y < height; y++)
      {
          for(int x = 0; x < width; x++)
          {
              image[x][y] = content.at(vit);
              vit++;
          }
      }
  }

  void  outputimage()
  {
      std::ofstream outputfile("image_processed.pgm"); //create output file

      outputfile << "P2" << std::endl
                  << width << " " << height << std::endl
                  << max << std::endl;

      for(int y = 0; y < height; y++)
      {
          for(int x = 0; x < width; x++)
            outputfile << image[x][y] << " ";
      }

      outputfile.close();
  }
};

int main(int argc, char *argv[])
{
    assert(argc == 4);

    std::string filename = argv[1];

    int vert = atoi(argv[2]); //vert == vertical seams
    int horz = atoi(argv[3]); //horz == horizontal seams

    imgsc image(filename);

    image.carve(vert, horz);
    image.outputimage();

    return 0;
}
