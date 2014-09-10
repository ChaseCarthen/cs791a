#include <terrain.h>

terrain::terrain()
{
 height = 10;
 width = 40;
 length = 40;
}
bool terrain::loadTerrain(std::string filename)
{
   rend.init();
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
   vecs = getRawValuesFromFile(filename);
   float min=100000000,max=-100000;
   for(int i =0; i < vecs.size(); i++)
      for(int j = 0; j < vecs[i].size(); j++)
      {
        //cout << "LOOP" << j << " " << floatarr[i].size() << endl;
        float v = vecs[i][j];
        
        if(v > max)
        {
          max = v;
        }
        if(v < min && v > 0)
        {
          min = v;
        }
        if(v < 0)
        {
          vecs[i][j] = 0;
        }
      } 
      veccount = fromRawCreateMesh(vecs, min, max, vbo_geometry, elements);
      return true;

}
void terrain::render(glm::mat4& projection,glm::mat4& view,GLuint& texture)
{
	glm::mat4 mvp;

    mvp = projection * view * model;

    //enable the shader program
    //glUseProgram(program);
    rend.useProgram();

    //upload the matrix to the shader
    //glUniformMatrix4fv(loc_mvpmat, 1, GL_FALSE, glm::value_ptr(mvp));
    rend.setUniformMatrix4x4("mvpMatrix",mvp);

    //glUniform1f(loc_height,max_height);
    rend.setUniformFloat("h",height);

    //set up the Vertex Buffer Object so it can be drawn
    //glEnableVertexAttribArray(loc_position);
    //glEnableVertexAttribArray(loc_color);
    rend.enableVertexAttribPointer("v_position");
    rend.enableVertexAttribPointer("v_color");

    //glEnable(GL_TEXTURE_1D);
    //glUniform1i(colorm, 0);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_1D, texture);
    rend.setTexture("tex",texture);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    //set pointers into the vbo for each of the attributes(position and color)
    /*glVertexAttribPointer( loc_position,//location of attribute
                           3,//number of elements
                           GL_FLOAT,//type
                           GL_FALSE,//normalized?
                           sizeof(Vertex),//stride
                           0);//offset

    glVertexAttribPointer( loc_color,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof(Vertex),
                           (void*)offsetof(Vertex,color));*/
    rend.setGLVertexAttribPointer("v_position",3, GL_FLOAT, GL_FALSE,sizeof(Vertex),0);
    rend.setGLVertexAttribPointer("v_color",3, GL_FLOAT, GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,color));

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements);


    rend.render(veccount);
 // Draw the triangles !
 /*glDrawElements(
     GL_LINES,      // mode
     cs,    // count
     GL_UNSIGNED_INT,   // type
     (void*)0           // element array buffer offset
 );
    //return;
    //glDrawArrays(GL_TRIANGLES, 0, 36);//mode, starting index, count

    //clean up
    glDisableVertexAttribArray(loc_position);
    glDisableVertexAttribArray(loc_color);*/
    rend.disableVertexAttribPointer("v_color");
    rend.disableVertexAttribPointer("v_position"); 
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