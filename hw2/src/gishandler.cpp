#include <gishandler.h>

bool getRawValuesFromFile(string fname,vector<vector<float>>& vecs)
{
   
   //vector<float> temp = vector<float>()
   GDALDataset *poDataset;
   GDALAllRegister();
   poDataset= (GDALDataset*) GDALOpen(fname.c_str(),GA_ReadOnly);
   if(poDataset == NULL)
   {
      cout << "OUCH!" << endl;
      return false;
   }
   cout << "Data size: " << GDALGetRasterXSize(poDataset) << " " << GDALGetRasterYSize(poDataset) << endl;

   GDALRasterBand  *poBand;
   int             nBlockXSize, nBlockYSize;
   int             bGotMin, bGotMax;
   double          adfMinMax[2];
        
   poBand = poDataset->GetRasterBand( 1 );
   poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );
   printf( "Block=%dx%d Type=%s, ColorInterp=%s\n",
            nBlockXSize, nBlockYSize,
            GDALGetDataTypeName(poBand->GetRasterDataType()),
            GDALGetColorInterpretationName(
            poBand->GetColorInterpretation()) );

   adfMinMax[0] = poBand->GetMinimum( &bGotMin );
   adfMinMax[1] = poBand->GetMaximum( &bGotMax );
   if( ! (bGotMin && bGotMax) )
      GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
   int width = poBand->GetXSize(); 
   int height = poBand->GetYSize();
   int bands = poDataset->GetRasterCount();
   float *pafScanline;
   std::cout << "Before allocation" << adfMinMax[0] << " " << adfMinMax[1] << endl;
   int dsize = 256;
   pafScanline = (float *) CPLMalloc(sizeof(float)*width*height);
   vector<vector<float>> out = vector<vector<float>>(height,vector<float> (width,0));
   poBand->RasterIO(GF_Read,0,0,width,height,pafScanline,width,height,GDT_Float32,0,0);
   cout << "After allocation" << endl;
   for(int i = 0; i < height; i++)
   {
    for(int j = 0; j < width; j++)
    {
      //cout << i << j << endl << pafS;
      out[i][j] = pafScanline[i*width+j];
    }
   }
   CPLFree(pafScanline);
   //for(auto i : out)
   //for(auto j : i)
   //		cout << j << endl;
  cout << "After allocation" << endl;
  vecs = out;
   return true;
}

int fromRawCreateMesh(vector<vector<float>>& input,float min, float max,float width, float height,GLuint& vbo_geometry,GLuint& elements,glm::vec3 color,vector<vector<bool>> activeness)
{
    std::vector<Vertex> vectors = vector<Vertex>();
    vector<int> indexs = vector<int>();
    std::cout << "Triangulating" << max << " " << min << std::endl;
    //exit(0);
    // Indexs
    for(int i = 0; i < input.size(); i++)
    {
    	for(int j = 0; j < input[i].size(); j++)
    	{   
        float val = (float)(input[i][j]-min)/(float)(max-min);
        //cout << "VALUE: "<< val << endl;
        //val += 1;
        if(val < 0)
        {
          val = -1;

          Vertex temp = {{width*(float)(i+1)/(float)input.size(),0,height*(float)(j+1)/(float)input[i].size()},color,{(float)i/(float)(input.size()-1),(float)j/(float)(input[i].size()-1)},activeness[i][j]};
          vectors.push_back(temp);
        }
        else
        {
          //if(val > 1)
          //cout << "VALUE: "<< val << endl;
    		  Vertex temp = {{width*(float)(i+1)/(float)input.size(),val,height*(float)(j+1)/(float)input[i].size()},color,{(float)i/(float)(input.size()-1),(float)j/((float)input[i].size()-1)},activeness[i][j]};
          vectors.push_back(temp);
    		}
    		// point, bottom, right
    		if(i+1<input.size() && j+1 <input[i].size())
    		{
    			indexs.push_back(i*input[i].size()+j);
    			indexs.push_back((i+1)*input[i].size()+j);
    			indexs.push_back((i+1)*input[i].size()+j+1);
    		}
    		// point, left, bottom
    		if(j-1 >= 0 && i+1 <input.size())
    		{
    			indexs.push_back(i*input[i].size()+j);
    			indexs.push_back((i)*input[i].size()+j-1);
    			indexs.push_back((i+1)*input[i].size()+j);
    		}
    	}
    }
    	
    // Generate vbo
    cout << "Allocate VBO" << endl;
    glGenBuffers(1, &vbo_geometry);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vectors.size(), &vectors[0], GL_STATIC_DRAW);

    // Generate element array
    cout << "Allocate Elements" << endl;
    glGenBuffers(1, &elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexs.size() * sizeof(int), &indexs[0], GL_STATIC_DRAW);
	// return 
	//return vectors;
	return indexs.size();
}

void bilinearintertop(vector<vector<float>>& input, int width, int height    )
{
  vector<vector<float>> vecs = vector<vector<float>>(height,vector<float>(width,0));
  for(int i = 0; i < vecs.size(); i++)
  {
    for(int j =0; j < vecs[i].size(); j++)
    {
      float iratio = (float)i/(float)vecs.size();
      float jratio = (float)j/(float)vecs[i].size();
      float i2 =  iratio * input.size();
      float j2 = jratio * input[0].size();
      float ib = floor(i2);
      float it = ceil(i2);
      float jt = floor(j2);
      float jb = ceil(j2);
      float xdelta = jt - jb;
      float ydelta = it - ib;
      // x interpolation - 0,0 1,0
      float x1 = (jt - j2)/ xdelta * input[ib][jb] + (j2 - jb)/xdelta*input[ib][jt];

      // x interpolation - 0,1 1,1
      float x2 = (jt - j2)/ xdelta * input[it][jb] + (j2 - jb)/xdelta*input[it][jt];

      // y interpolation - 0,0 1,0
      float val = (it - i2)/ ydelta * x1 + (i2 - ib)/ydelta*x2;
      vecs[i][j] = val;

    }
  }


}

void generateTexture(string fname, GLuint& tex, int bandnum)
{
   if(bandnum <= 0 )
   {
     bandnum = 1;
   }
   GDALDataset *poDataset;
   GDALAllRegister();
   poDataset= (GDALDataset*) GDALOpen(fname.c_str(),GA_ReadOnly);
   if(poDataset == NULL)
   {
      cout << "OUCH!" << endl;
      //exit(0);
      return;
   }
   cout << "Data size: " << GDALGetRasterXSize(poDataset) << " " << GDALGetRasterYSize(poDataset) << endl;

   GDALRasterBand  *poBand;
   int             nBlockXSize, nBlockYSize;
   int             bGotMin, bGotMax;
   double          adfMinMax[2];
  int bands = poDataset->GetRasterCount();
   bandnum = bandnum % bands + 1;
   if(bandnum > bands)
   {
      bandnum = 1;
   }
   poBand = poDataset->GetRasterBand( bandnum );
   poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );
   printf( "Block=%dx%d Type=%s, ColorInterp=%s\n",
            nBlockXSize, nBlockYSize,
            GDALGetDataTypeName(poBand->GetRasterDataType()),
            GDALGetColorInterpretationName(
            poBand->GetColorInterpretation()) );

   float max = adfMinMax[0] = poBand->GetMinimum( &bGotMin );
   float min = adfMinMax[1] = poBand->GetMaximum( &bGotMax );
   if( ! (bGotMin && bGotMax) )
      GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
   int width = poBand->GetXSize(); 
   int height = poBand->GetYSize();

   float *pafScanline;
   std::cout << "Before allocation" << adfMinMax[0] << " " << adfMinMax[1] << endl;
   min = adfMinMax[0];
   max = adfMinMax[1];
   int dsize = 256;
   pafScanline = (float *) CPLMalloc(sizeof(float)*512*512);
   vector<vector<float>> out = vector<vector<float>>(height,vector<float> (width,0));
   //vector<vector<unsigned char>> texs = vector<vector<unsigned char>>(height,vector<unsigned char> (width,0));
   unsigned char texs[512*512];
   poBand->RasterIO(GF_Read,0,0,width,height,pafScanline,512,512,GDT_Float32,0,0);
   float no = poBand->GetNoDataValue();
   cout << "After allocation" << endl;
   for(int i = 0; i < 512; i++)
   {
    for(int j = 0; j < 512; j++)
    {
      //cout << i << j << endl << pafS;
      if(pafScanline[i*width+j] != no)
      {
        // set tex val
        texs[i*512+j] = (unsigned char)(255*((pafScanline[i*512+j] - min)/(max-min)));
        //if((int)texs[i*width] < 0)
        //cout << (int)texs[i*512 +j] << " " << pafScanline[i*512+j] << " " << no << " " << fname << " " << min << " " << max << endl;
      }
      else
      {
        // Set zero val
        texs[i*512+j] = 0;
        //cout << (int)texs[i*512 +j] << fname << endl;
      }
      //texs[i*512+j] = 255;

      //ut[i][j] = pafScanline[i*width+j];
    }
   }
   CPLFree(pafScanline);
   //exit(0);
   // Create a texture
   glGenTextures(1,&tex);
   glBindTexture(GL_TEXTURE_2D,tex);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512,512, 0, GL_RED, GL_UNSIGNED_BYTE,texs);
   GDALClose( (GDALDatasetH) poDataset);
   return;
} 