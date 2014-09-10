#include <gishandler.h>
vector<vector<float>> getRawValuesFromFile(string fname)
{
   
   //vector<float> temp = vector<float>()
   GDALDataset *poDataset;
   GDALAllRegister();
   poDataset= (GDALDataset*) GDALOpen(fname.c_str(),GA_ReadOnly);
   if(poDataset == NULL)
   {
      cout << "OUCH!" << endl;
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
   return out;
}

int fromRawCreateMesh(vector<vector<float>>& input,float min, float max,GLuint& vbo_geometry,GLuint& elements)
{
    std::vector<Vertex> vectors = vector<Vertex>();
    vector<int> indexs = vector<int>();
    std::cout << "Triangulating" << max << " " << min << std::endl;
    // Indexs
    for(int i = 0; i < input.size(); i++)
    {
    	for(int j = 0; j < input[i].size(); j++)
    	{   
        float val = (float)(input[i][j]-min)/(float)(max-min);
        //cout << input[i][j] << endl;
        //val += 1;
        if(val < 0)
        {
          val = -1;

          Vertex temp = {{100*(float)(i+1)/(float)input.size(),0,100*(float)(j+1)/(float)input[i].size()},val*glm::vec3(0,0,1)};
          vectors.push_back(temp);
        }
        else
        {
          //if(val > 1)
          //cout << "VALUE: "<< val << endl;
    		  Vertex temp = {{100*(float)(i+1)/(float)input.size(),val,100*(float)(j+1)/(float)input[i].size()},val*glm::vec3(1,0,0)+(1-val)*glm::vec3(0,1,0)};
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