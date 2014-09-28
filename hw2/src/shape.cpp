#include <shape.h>

shape::shape()
{
	 points = vector<vector<glm::vec2>>();
     model = glm::mat4(1);
}
bool shape::load(string filename)
{
    OGRRegisterAll();
	

	// First open the shape file
	ds = OGRSFDriverRegistrar::Open( filename.c_str(), FALSE );

	// Now to load in the points .....
	points = vector<vector<glm::vec2>>();

	// Grab the first layer
    OGRLayer* layer = ds->GetLayer(0);

    // Grab the spatial reference and create a coordinate transform function
    sr = layer->GetSpatialRef();
	
    // Taking from http://www.compsci.wm.edu/SciClone/documentation/software/geo/gdal-1.9.0/html/ogr/ogr_apitut.html
    OGRFeature *poFeature;

    layer->ResetReading();
    while( (poFeature = layer->GetNextFeature()) != NULL )
    {
        OGRFeatureDefn *poFDefn = layer->GetLayerDefn();
        int iField;

        OGRGeometry *poGeometry;

        poGeometry = poFeature->GetGeometryRef();
        if( poGeometry != NULL 
            && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint )
        {
            OGRPoint *poPoint = (OGRPoint *) poGeometry;

            //printf( "%.3f,%3.f\n", poPoint->getX(), poPoint->getY() );
        }
        else if (poGeometry != NULL 
            && wkbFlatten(poGeometry->getGeometryType()) == wkbLineString)
        {
            //cout << "LINES!!!!" << endl;
            OGRLineString* ls= (OGRLineString*)poGeometry;
            points.push_back(vector<glm::vec2>());
            for(int i = 0; i < ls->getNumPoints(); i++ )
            {
            	OGRPoint p;
            	ls->getPoint(i,&p);

            	// This function can transform a larget set of points.....
            	double x = p.getX();
            	double y = p.getY();
            	points[points.size()-1].push_back(glm::vec2(x,y));
            	//poTransform->Transform (1, &x, &y);
            	//cout << p.getX() << " " << p.getY()  << "Transformed!: " << x << " " << y << endl;
            }
        }  
        else if (poGeometry != NULL 
            && wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon)
        {

            OGRLineString* ls= (OGRLineString*)poGeometry->getBoundary();
            points.push_back(vector<glm::vec2>());
            cout << "POLYGON" << ls->getNumPoints() << endl;
            //exit(0);
            for(int i = 0; i < ls->getNumPoints(); i++ )
            {
                OGRPoint p;
                ls->getPoint(i,&p);

                // This function can transform a larget set of points.....
                double x = p.getX();
                double y = p.getY();
                points[points.size()-1].push_back(glm::vec2(x,y));
                //poTransform->Transform (1, &x, &y);
                //cout << p.getX() << " " << p.getY()  << "Transformed!: " << x << " " << y << endl;
            }
        }      
        OGRFeature::DestroyFeature( poFeature );
    }
    return true;
}

bool shape::createMesh(OGRSpatialReference* sr2,glm::vec2 origin,glm::vec2 scale,terrain& t)
{
	// Now to pair the points
	vector<Vertex> vertexs = {};
	vector<int> indicies = {};
    rend.init();
    GLuint vertex_shader2;
    GLuint frag_shader2;
    vertex_shader2 = glCreateShader(GL_VERTEX_SHADER);

    frag_shader2 = glCreateShader(GL_FRAGMENT_SHADER);
    cout << "LINE RENDERER CREATOR" << endl;
    if(!rend.addShader(vertex_shader2,"../shaders/linevert.shader"))
    {
      cout << "Failed to add shader" << endl;
      return false;
    }
    if(!rend.addShader(frag_shader2,"../shaders/linefrag.shader"))
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
	OGRCoordinateTransformation* transform;
    // Create a coordinate transform
    if(sr != NULL)
    {
    	transform = OGRCreateCoordinateTransformation( sr, sr2 );
    }
    else
    {
    	return false;
    }
    /*Vertex tempo = {{0,0,0},{1,0,0},{1,1}};
    Vertex tempo2 = {{0,100,0},{1,0,0},{1,1}};
    Vertex tempo3 = {{100,0,100},{1,0,0},{1,1}};
    Vertex tempo4 = {{100,100,100},{1,0,0},{1,1}};
    vertexs.push_back(tempo);
    vertexs.push_back(tempo2);
    vertexs.push_back(tempo3);
    vertexs.push_back(tempo4);
    indicies.push_back(0);
    indicies.push_back(1);
    indicies.push_back(3);*/
    //indicies.push_back(2);
    //indicies.push_back(3);
    //indicies.push_back(0);
	// triangulate and create lines between vertices
    bool first = true;
	for(int i = 0; i < points.size(); i++)
	{
        //break;
        first = true;
		for(int j = 0; j < points[i].size(); j++)
		{
            if(j == points[i].size() - 1)
            {
                continue;
            }
			double x = points[i][j].x;
            double y = points[i][j].y;
            //points[points.size()-1].push_back(glm::vec2(x,y));
            transform->Transform (1, &x, &y);
            glm::vec2 orig = glm::vec2(x,y);
            x = abs(x -origin.x) *scale.x;
            y = abs(y-origin.y)*scale.y;

			Vertex temp = {{y,t.interpolateHeight(orig),x},{1,0,0},{1,1}};
            //cout << "Vertex: " << temp.position.x << " " << temp.position.y << " " << temp.position.z << endl;
            if(first)
            {
			 vertexs.push_back(temp);
			 temp.position.y += 2;
			 vertexs.push_back(temp);
             first = false;
            }
			if(j+1 < points[i].size())
			{
				x = points[i][j+1].x;
            	y = points[i][j+1].y;
                transform->Transform (1, &x, &y);
                orig = glm::vec2(x,y);
                x = abs(x -origin.x) *scale.x;
                y = abs(y-origin.y)*scale.y;

				Vertex temp2 = {{y,t.interpolateHeight(orig),x},{1,0,0},{1,1}};
                //cout << "Vertex: " << temp2.position.x << " " << temp2.position.y << " " << temp2.position.z << endl;
				vertexs.push_back(temp2);
				temp2.position.y += 2;
				vertexs.push_back(temp2);
                // Triangulate!
                indicies.push_back(vertexs.size()-3);
                indicies.push_back(vertexs.size()-1);
                indicies.push_back(vertexs.size()-4);
                indicies.push_back(vertexs.size()-2);
                indicies.push_back(vertexs.size()-4);
                indicies.push_back(vertexs.size()-1);   
			}


		}
	}

    // Generate vbo
    veccount = indicies.size();
    cout << "Allocate VBO" << endl;
    glGenBuffers(1, &vbo_geometry);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertexs.size(), &vertexs[0], GL_STATIC_DRAW);

    // Generate element array
    cout << "Allocate Elements" << endl;
    glGenBuffers(1, &elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(int), &indicies[0], GL_STATIC_DRAW);
    return true;
}
void shape::render(glm::mat4& projection,glm::mat4& view)
{
    glm::mat4 mvp;
    //cout << veccount << "VECCOUNT " << endl;
    mvp = projection * view * model;

    rend.useProgram();
    //glDepthMask( GL_FALSE );
    //glPolygonOffset(   2.0, 1.0);
    rend.setUniformMatrix4x4("mvpMatrix",mvp);

    rend.enableVertexAttribPointer("v_position");
    rend.enableVertexAttribPointer("v_color");

    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);

    rend.setGLVertexAttribPointer("v_position",3, GL_FLOAT, GL_FALSE,sizeof(Vertex),0);
    rend.setGLVertexAttribPointer("v_color",3, GL_FLOAT, GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,color));

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements);

    //cout << "COUNT: " << veccount << endl;
    rend.render(veccount);
    rend.disableVertexAttribPointer("v_color");
    rend.disableVertexAttribPointer("v_position");  
}