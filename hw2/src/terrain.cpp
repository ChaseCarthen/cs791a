#include <terrain.h>
bool valid(int i, int j, int width, int height)
{
  return i >= 0 && i < width && j >= 0 && j < height; 
};
terrain::terrain()
{
 height = 10;
 width = 40;
 length = 40;
 h = 40;
 w = 40;
 selected = false;
}
bool terrain::loadTerrain(std::string fname)
{
  mh = 10;
  GDALAllRegister();
  ds= ((GDALDataset*) GDALOpen(fname.c_str(),GA_ReadOnly));
  if(ds == NULL)
  {
    return false;
  }

  GDALRasterBand  *poBand;
  int             nBlockXSize, nBlockYSize;
  int             bGotMin, bGotMax;
  double          adfMinMax[2];
        
  poBand = ds->GetRasterBand( 1 );
  poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );

  adfMinMax[0] = poBand->GetMinimum( &bGotMin );
  adfMinMax[1] = poBand->GetMaximum( &bGotMax );
  if( ! (bGotMin && bGotMax) )
    GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
  min = adfMinMax[0];
  max = adfMinMax[1];
  width = poBand->GetXSize(); 
  height = poBand->GetYSize();
  int bands = ds->GetRasterCount();

  float *pafScanline;
  std::cout << "Before allocation" << adfMinMax[0] << " " << adfMinMax[1] << endl;
  int dsize = 256;
  pafScanline = (float *) CPLMalloc(sizeof(float)*width*height);
  vecs = vector<vector<float>>(height,vector<float> (width,0));
  active = vector<vector<bool>>(height,vector<bool> (width,true));
  poBand->RasterIO(GF_Read,0,0,width,height,pafScanline,width,height,GDT_Float32,0,0);
  cout << "After allocation" << endl;
  double geot[6];
  ds->GetGeoTransform(geot);

  //ds->GetProjection();
  xres = geot[1];
  yres = geot[5];
  string proj;
  proj = string(ds->GetProjectionRef());

  //OGRSpatialReference sr2;
  char* test = &proj[0];
  sr.importFromWkt(&test);
  startx = geot[0];
  starty = geot[3];
  th = height;
  tw = width;
  h = height*yres;
  w = width*xres;
  h = 200;
  w = 200;
  for(int i = 0; i < height; i++)
  {
    for(int j = 0; j < width; j++)
    {
      //cout << i << j << endl << pafS;
      vecs[i][j] = pafScanline[((int)i)*(int)width+((int)width-j)];

    }
   }
   CPLFree(pafScanline);
   return true;
   bandnum = 0;
   raster = 0;
   mask = 0;
}

bool terrain::createMesh(glm::vec3 color,float wd,float ht)
{
  rend.init();
  if(wd==1 && ht==1)
  {
    wd = width;
    ht = height;
  }
  GLuint vertex_shader2;
  GLuint frag_shader2;
  vertex_shader2 = glCreateShader(GL_VERTEX_SHADER);

  frag_shader2 = glCreateShader(GL_FRAGMENT_SHADER);
  if(!rend.addShader(vertex_shader2,"../shaders/vert.shader"))
  {
    cout << "Failed to add shader" << endl;
    return false;
  }
  if(!rend.addShader(frag_shader2,"../shaders/frag.shader"))
  {
    cout << "Failed to add shader" << endl;
    return false;
  }

  if(!rend.compile())
  {
    cout << "Failed to compile shaders" << endl;
    return false;
  }
  if(!rend.link())
  {
    cout << "Failed to link shaders" << endl;
    return false;
  }
  
  for(int i =0; i < vecs.size(); i++)
    for(int j = 0; j < vecs[i].size(); j++)
    {
      //cout << "LOOP" << j << " " << floatarr[i].size() << endl;
      float v = vecs[i][j];
        
      if(v < 0)
      {
        vecs[i][j] = 0;
      }
    } 
    h = h*getWorldHeight()/ht;
    w =w*getWorldWidth()/wd;
    cout << "SCALES: " << w << h << endl; 
    veccount = fromRawCreateMesh(vecs, min, max, w,h,vbo_geometry, elements,color,active);
    return veccount != 0;
}

void terrain::render(glm::mat4& projection,glm::mat4& view,GLuint& texture)
{
    glm::mat4 mvp;
    glDepthMask( GL_TRUE );
    mvp = projection * view * model;

    rend.useProgram();

    rend.setUniformMatrix4x4("mvpMatrix",mvp);

    rend.setUniformFloat("h",mh);
    rend.setUniformInteger("que",selected);
    rend.enableVertexAttribPointer("v_position");
    rend.enableVertexAttribPointer("v_color");
    rend.enableVertexAttribPointer("v_uv");
    rend.enableVertexAttribPointer("active");
    if(selectedMask != NULL)
    {
      rend.setTexture("mask",GL_TEXTURE_2D,GL_TEXTURE0,selectedMask);
      
    }
    if(selectedRaster != NULL)
    rend.setTexture("raster",GL_TEXTURE_2D,GL_TEXTURE0+1,selectedRaster);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);

    rend.setGLVertexAttribPointer("v_position",3, GL_FLOAT, GL_FALSE,sizeof(Vertex),0);
    rend.setGLVertexAttribPointer("v_color",3, GL_FLOAT, GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,color));
    rend.setGLVertexAttribPointer("v_uv",2, GL_FLOAT, GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,uv));
    rend.setGLVertexAttribPointer("active",1, GL_INT, GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,keep));
    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements);

    //cout << "COUNT: " << veccount << endl;
    rend.render(veccount);
    rend.disableVertexAttribPointer("v_color");
    rend.disableVertexAttribPointer("v_position");
    rend.disableVertexAttribPointer("v_uv");
    rend.disableVertexAttribPointer("active");  
}
void terrain::update()
{

}
void terrain::setHeight(float val)
{
	height = val;
}
void terrain::setWidth(float val)
{
	width = val;
}
void terrain::setLength(float val)
{
	height = val;
}

void terrain::cleanup()
{
  rend.cleanup();
  glDeleteBuffers(1, &vbo_geometry);
  glDeleteBuffers(1,&elements);
}

vector<std::vector<float> > terrain::getData()
{
  return vecs;
}

float terrain::getWidth()
{
  return width;
}
float terrain::getHeight()
{
  return height;
}
float terrain::getXRes()
{
  return xres;
}
float terrain::getYRes()
{
  return yres;
}

glm::vec2 terrain::getWorldPoint(int i, int j)
{
  glm::vec2 temp;
  temp.x = getStartX() + xres*i;
  temp.y = getStartY() + yres*j;

  //cout << "WORLD: " << xres << " " << yres << endl;
  return temp;
}

vector<glm::vec2> terrain::getNearestWorldPoint(glm::vec2 point)
{
  int i = 0;
  int j = 0;
  double x = getStartX();
  double y = getStartY();
  i = floor(abs((point.x - x)/xres)+.5);
  j = floor(abs((point.y - y)/yres)+.5);
  glm::vec2  temp = getWorldPoint(i,j);
  if(xres == 2.5 && i < 1000)
  {
    //cout << "HERE" << i << " " << j << " " << width << " "<< height << " " << x << " " << y<< endl;
  }
  vector<glm::vec2> vects=vector<glm::vec2>();
  if(valid(i+1,j,width,height))
    vects.push_back(getWorldPoint(i+1,j));
  if(valid(i-1,j,width,height))
    vects.push_back(getWorldPoint(i-1,j));
  if(valid(i,j+1,width,height))
    vects.push_back(getWorldPoint(i,j+1));
  if(valid(i,j-1,width,height))
    vects.push_back(getWorldPoint(i,j-1));
  if(valid(i,j-1,width,height))
    vects.push_back(getWorldPoint(i,j-1));
  if(valid(i-1,j+1,width,height))
    vects.push_back(getWorldPoint(i-1,j+1));
  return vects;

}

float terrain::getStartX()
{
  double x = startx;
  double y = starty;
  if(transform != NULL)
  {

    transform->Transform (1, &x, &y);
    //cout << "Transform: " << x << " " << y << endl;
  }
  return x;
}

float terrain::getStartY()
{
  double y = starty;
  double x = startx;
  if(transform != NULL)
  {

    transform->Transform (1, &x, &y);
    //cout << "Transform: " << x << " " << y << endl;
  }
  return y;
}

void terrain::setTransformation(OGRSpatialReference& a)
{
  transform = OGRCreateCoordinateTransformation( &sr, &a);
}

void terrain::getNearestIndex(glm::vec2 point, float& i, float& j)
{
  double x = getStartX();
  double y = getStartY();
  j = (point.x - x)/xres;
  i = (y - point.y)/-yres;
}

float terrain::getWorldWidth()
{
  return xres*width;
}
float terrain::getWorldHeight()
{
  return abs(yres*height);
}

void terrain::translate(glm::vec3 position)
{
   //cout << "Position X:" << position.x << " " << position.y << " " << position.z << endl;
   //cout << "NO DATA VALUE: "<< ds->GetRasterBand(1)->GetNoDataValue() << endl;
   model = glm::translate( glm::mat4(1), position);
}

float terrain::getXScale()
{
  //cout << w << endl;
  return w/getWorldWidth();
}

float terrain::getYScale()
{
  return h/getWorldHeight();
}

float terrain::getMin()
{
  return min;
}
float terrain::getMax()
{
  return max;
}
void terrain::setMin(float& m)
{
  min = m;
}
void terrain::setMax(float& m)
{
  max = m;
}

float terrain::setMaxH(float m)
{
  mh = m;
}

void terrain::setactive(int i , int j, bool val)
{
  active[i][j] = val;
}

bool terrain::addMask(std::string fname)
{
  GDALDataset* ts = ((GDALDataset*) GDALOpen(fname.c_str(),GA_ReadOnly));
  if(!ts)
    return false;
  masks.push_back(fname);
  GDALClose( (GDALDatasetH) ts );
  return true;
}
bool terrain::addRaster(std::string fname)
{
  GDALDataset* ts = ((GDALDataset*) GDALOpen(fname.c_str(),GA_ReadOnly));
  if(!ts)
    return false;
  GDALClose( (GDALDatasetH) ts );
  rasterfs.push_back(fname);
  return true;
}

float terrain::interpolateHeight(glm::vec2 wp)
{
  float y,x;
  getNearestIndex(wp,y,x);
  int i = (int)y;
  int j = (int) x;
  float iratio = (float)i/(float)vecs.size();
  float jratio = (float)j/(float)vecs[i].size();
  float i2 =  y;
  float j2 = width-1-x;
  float ib = floor(i2);
  float it = ceil(i2);
  float jt = floor(j2);
  float jb = ceil(j2);
  float xdelta = jt - jb;
  float ydelta = it - ib;
  // x interpolation - 0,0 1,0
  float x1 = (jt - j2)/ xdelta * vecs[ib][jb] + (j2 - jb)/xdelta*vecs[ib][jt];

  // x interpolation - 0,1 1,1
  float x2 = (jt - j2)/ xdelta * vecs[it][jb] + (j2 - jb)/xdelta*vecs[it][jt];

  // y interpolation - 0,0 1,0
  float val = (it - i2)/ ydelta * x1 + (i2 - ib)/ydelta*x2;
  return (val-min)/(max-min)*mh;
}

void terrain::selectTexture(int i,int bn)
{
  generateTexture(masks[0],selectedMask,0);
  cout << "Selected: " << i << endl;
  generateTexture(rasterfs[i],selectedRaster,bn);
}