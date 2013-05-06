#include <windows.h>
#include <gl/gl.h>
#include <stdio.h>


struct CTGAHeader_ForSaving
{
  char IDCount;
  char ColorMap;
  char ImageCode;
  unsigned short CMOrigin;
  char CMLength;
  char CMBitDepth;
  unsigned short IMXOrigin;
  unsigned short IMYOrigin;
  unsigned short IMWidth;
  unsigned short IMHeight;
  char IMBitDepth;
  char IMDescByte;
};

/*
  Takes a screenshot of the current opengl buffer.  Be sure to include the
  extension in the filename, i.e. 'shot.tga' NOT 'shot'.
*/
void TakeScreenshot(const char* filename)
{
  /* grab OpenGL buffer */
  
  int VPort[4],FSize,PackStore;
  unsigned char *PStore;

  // get viewport dims (x,y,w,h)
  glGetIntegerv(GL_VIEWPORT,VPort);
  
  // allocate space for framebuffer in rgb format
  FSize = VPort[2]*VPort[3]*3;
  PStore = new unsigned char[FSize];

  // store unpack settings
  glGetIntegerv(GL_PACK_ALIGNMENT, &PackStore);

  // setup unpack settings
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  // this actually gets the buffer pixels
  glReadPixels(VPort[0],VPort[1],VPort[2],VPort[3],GL_RGB,GL_UNSIGNED_BYTE,PStore);

  // restore unpack settings
  glPixelStorei(GL_PACK_ALIGNMENT, PackStore);   
  
   
  /* dump data to disk */ 
  
  CTGAHeader_ForSaving Header;  
  int loop;
  FILE *pFile;
  unsigned char SwapByte;
  
  // setup header
  Header.IDCount    = 0;
  Header.ColorMap   = 0;
  Header.ImageCode  = 2;  // uncompressed rgb image
  Header.CMOrigin   = 0;
  Header.CMLength   = 0;
  Header.CMBitDepth = 0;
  Header.IMXOrigin  = 0;
  Header.IMYOrigin  = 0;
  Header.IMWidth    = VPort[2];
  Header.IMHeight   = VPort[3];
  Header.IMBitDepth = 24; // rgb with no alpha
  Header.IMDescByte = 0;
  
  // swap red and blue
  for (loop=0;loop<FSize;loop+=3)
  {
    SwapByte       = PStore[loop];
    PStore[loop]   = PStore[loop+2];
    PStore[loop+2] = SwapByte;    
  }
  
  // open file
  pFile = fopen( filename,"wb" );
  if (pFile==NULL)
  {
    delete[] PStore;
    return;
  }  
     
  // write to disk
  fwrite( &Header,sizeof(CTGAHeader_ForSaving),1,pFile ); // header
  fwrite( PStore,FSize,1,pFile ); // image data
  
  // close file
  fclose( pFile );
  
  // free memory
  delete[] PStore;
}