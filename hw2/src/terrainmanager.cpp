#include <terrainmanager.h>
#include <triad.h>
terrainManager::terrainManager()
{
	projection = "";
}

std::vector<std::vector<float> > terrainManager::getData(int index, bool largest)
{

}

void terrainManager::interpolateMesh(bool all)
{

}

void terrainManager::generateGraphMesh(bool all)
{
    vector<glm::vec3> marked;
    vector<int> indices;
    vector<int> group;
    vector<glm::vec2> explored = vector<glm::vec2> ();
    queue<glm::vec2> que;
    vector<glm::vec2> inque = vector<glm::vec2>();
    vector<vector<vector<bool>>> activeness;
    // figure out whos the largest
    // use this first element as the precursor to generate the mesh
    int index = -1;
    float largest = -199999;
    for(int i =0; i < terrains.size(); i++)
    {
        float val = abs(terrains[i].getWidth() * terrains[i].getHeight() * terrains[i].getXRes() * terrains[i].getYRes());
        if( val > largest)
        {
            largest = val;
            cout << terrains[i].getXRes() << terrains[i].getYRes()<< endl;
            index = i;
        }
        cout << terrains[i].getXRes() << " " << terrains[i].getStartX() << " " << terrains[i].getHeight() << " " << terrains[i].getWidth() << endl;
    }
    auto first = terrains[index].getWorldPoint(0,0);
    // range variables
    int upperi,upperj,loweri,lowerj;
    vector<vector<float>> vects2;
    int other;
    for(int i = 0; i < terrains.size(); i++)
    {
        if(i != index)
        {
            float k,l;
            vects2 = terrains[i].getData();
            terrains[index].getNearestIndex(terrains[i].getWorldPoint(terrains[i].getWidth()-1,terrains[i].getHeight()-1),k,l);
            upperj = floor(k);
            upperi = floor(l);
            terrains[index].getNearestIndex(terrains[i].getWorldPoint(0,0),k,l);
            loweri = floor(l);
            lowerj = floor(k);
            cout << "INDEX" << k << " " << l << endl;
            other = i;
        }
    }

    // Now to triangulate the two meshes --- for now
    vector<Point> vect;
    auto vects = terrains[index].getData();
    for(int i = 0; i < terrains[index].getWidth(); i++)
    {

        for (int j = 0; j < terrains[index].getHeight();j++)
        {
            // check if there are between indexes
            if ( i <= upperi && i > loweri && j <= upperj && j > lowerj)
            {
                // Iterate through other mesh
                int ci = i;
                int cip = i+1;
                float t,s;
                float v;
                vector<int> ranges = {};
                // Find the indexs to use
                terrains[other].getNearestIndex(terrains[index].getWorldPoint(i,j),t,s);

                for(int k = 0; k < terrains[other].getHeight(); k++ )
                {
                    terrains[other].getNearestIndex(terrains[index].getWorldPoint(i,j),t,s);
                    
                    //if(t < cip && t >= ci )
                    //{
                    cout << s << endl;
                    //exit(0);
                    glm::vec2 p = terrains[other].getWorldPoint(i,j);
                    Point p2 = Point(p.x,p.y);
                    vect.push_back(p2);
                    terrains[other].setactive(k,s,true);
                    terrains[index].getNearestIndex(terrains[other].getWorldPoint(s,k),t,s);
                    terrains[index].setactive(t,terrains[index].getWidth() - s,false);
                    //}
                }
                j = ceil(upperj);
            }
            else
            {
                //vects.push_back(terrains[index].getWorldPoint(j,i));
                glm::vec2 p = terrains[index].getWorldPoint(i,j);
                Point p2 = Point(p.x,p.y);
                vect.push_back(p2);
            }
        }


    }
    triad(vect);
}

int terrainManager::numberOfTerrains()
{
	return terrains.size();
}

string terrainManager::getProjection()
{
	return projection;
}

bool terrainManager::addTerrain(string filename)
{
    terrain t;
    terrains.push_back(t);
    bool status = terrains.back().loadTerrain(filename);
    if(status)
    {
        //Set biggest projection
        int index = -1;
        float largest = -199999;
        for(int i =0; i < terrains.size(); i++)
        {
            float val = abs(terrains[i].getWidth() * terrains[i].getHeight() * terrains[i].getXRes() * terrains[i].getYRes());
            if( val > largest)
            {
                largest = val;
                cout << "LARGEST" << terrains[i].getXRes() << terrains[i].getYRes()<< endl;
                index = i;
            }
            //cout << terrains[i].getStartX() << " " << terrains[i].getStartY() << endl;
        }
        for(int i = 0; i < terrains.size(); i++ )
        {
            terrains[i].setTransformation(terrains[index].getSpatialReference());
        }
        
        origin = glm::vec3(terrains[index].getStartX()+terrains[index].getXRes()*terrains[index].getWidth(),terrains[index].getData()[0][0],terrains[index].getStartY());
        scalex = terrains[index].getWidth()/terrains[index].getWorldWidth();
        scaley = terrains[index].getHeight()/terrains[index].getWorldHeight();
        return true;
    }
    return false;
    //t.loadTerrain(filename);
}

bool terrainManager::addShape(string filename)
{
    shape s;
    bool status = s.load(filename);
    cout << "LOADED SHAPE" << status << endl;
    if(status)
    {
        shapes.push_back(s);
        return true;
    }   
    return false;
}

void terrainManager::render(glm::mat4& projection,glm::mat4& view,GLuint& texture)
{
    /*int index = -1;
    float largest = -199999;
    for(int i =0; i < terrains.size(); i++)
    {
        float val = abs(terrains[i].getWidth() * terrains[i].getHeight() * terrains[i].getXRes() * terrains[i].getYRes());
        if( val > largest)
        {
            largest = val;
            //cout << terrains[i].getXRes() << terrains[i].getYRes()<< endl;
            index = i;
        }
        //cout << terrains[i].getXRes() << " " << terrains[i].getStartX() << " " << terrains[i].getHeight() << " " << terrains[i].getWidth() << endl;
    }*/
    for(int i = 0; i < terrains.size();i++)
    {
        float y = abs(terrains[i].getStartY()-origin.z)*terrains[i].getYScale();
        float x = abs(origin.x-terrains[i].getStartX()-(terrains[i].getXRes()*terrains[i].getWidth()))*terrains[i].getXScale();

        terrains[i].translate(glm::vec3(y,0,x));
        terrains[i].render(projection,view,texture);
    }
    for(int i = 0; i < shapes.size(); i++)
    {
        //cout << "RENDER" << endl;
        shapes[i].render(projection,view);
    }
}

void terrainManager::createMeshes()
{
    cout << "CREATE MESHES!!!" << endl;
    int index = -1;
    float largest = -199999;
    for(int i =0; i < terrains.size(); i++)
    {
        float val = abs(terrains[i].getWidth() * terrains[i].getHeight() * terrains[i].getXRes() * terrains[i].getYRes());
        if( val > largest)
        {
            largest = val;
            cout << terrains[i].getXRes() << terrains[i].getYRes()<< endl;
            index = i;
        }
        cout << terrains[i].getXRes() << " " << terrains[i].getStartX() << " " << terrains[i].getHeight() << " " << terrains[i].getWidth() << endl;
    }
    float min = terrains[index].getMin();
    float max = terrains[index].getMax();
    for(int i=0; i < terrains.size(); i++)
    {
        cout << "MIN: " << min << "Max: " << max << endl;
        //exit(0);
        terrains[i].setMin(min);
        terrains[i].setMax(max);
        terrains[i].setMaxH(10);
        if(i==0)
        terrains[i].createMesh(glm::vec3(1,0,0),terrains[index].getWorldWidth(),terrains[index].getWorldHeight());
        else
        terrains[i].createMesh(glm::vec3(0,1,0),terrains[index].getWorldWidth(),terrains[index].getWorldHeight());
    }

    for(int i = 0; i < shapes.size(); i++)
    {
        shapes[i].createMesh(&terrains[index].getSpatialReference(),glm::vec2(origin.x,origin.z),glm::vec2(terrains[index].getXScale(),terrains[index].getYScale() ),terrains[index] );
    }
    cout << GL_PATCHES << endl;
    printf("OpenGL version supported by this platform (%s): \n",
        glGetString(GL_VERSION));
}


void terrainManager::setActiveTerrain()
{
    vector<glm::vec3> marked;
    vector<int> indices;
    vector<int> group;
    vector<glm::vec2> explored = vector<glm::vec2> ();
    queue<glm::vec2> que;
    vector<glm::vec2> inque = vector<glm::vec2>();
    vector<vector<vector<bool>>> activeness;
    // figure out whos the largest
    // use this first element as the precursor to generate the mesh
    int index = -1;
    float largest = -199999;
    for(int i =0; i < terrains.size(); i++)
    {
        float val = abs(terrains[i].getWidth() * terrains[i].getHeight() * terrains[i].getXRes() * terrains[i].getYRes());
        if( val > largest)
        {
            largest = val;
            cout << terrains[i].getXRes() << terrains[i].getYRes()<< endl;
            index = i;
        }
        cout << terrains[i].getXRes() << " " << terrains[i].getStartX() << " " << terrains[i].getHeight() << " " << terrains[i].getWidth() << endl;
    }
    auto first = terrains[index].getWorldPoint(0,0);
    // range variables
    int upperi,upperj,loweri,lowerj;
    vector<vector<float>> vects2;
    int other;
    for(int i = 0; i < terrains.size(); i++)
    {
        if(i != index)
        {
            float k,l;
            vects2 = terrains[i].getData();
            terrains[index].getNearestIndex(terrains[i].getWorldPoint(terrains[i].getWidth()-1,terrains[i].getHeight()-1),k,l);
            upperj = floor(k);
            upperi = floor(l);
            terrains[index].getNearestIndex(terrains[i].getWorldPoint(0,0),k,l);
            loweri = floor(l);
            lowerj = floor(k);
            cout << "INDEX" << k << " " << l << endl;
            other = i;
        }
    }

    // Now to triangulate the two meshes --- for now
    vector<Point> vect;
    auto vects = terrains[index].getData();
    for(int i = 0; i < terrains[index].getWidth(); i++)
    {

        for (int j = 0; j < terrains[index].getHeight();j++)
        {
            // check if there are between indexes
            if ( i <= upperi && i > loweri && j <= upperj && j > lowerj)
            {
                // Iterate through other mesh
                int ci = i;
                int cip = i+1;
                float t,s;
                float v;
                vector<int> ranges = {};
                // Find the indexs to use
                terrains[other].getNearestIndex(terrains[index].getWorldPoint(i,j),t,s);

                for(int k = 0; k < terrains[other].getHeight(); k++ )
                {
                    terrains[other].getNearestIndex(terrains[index].getWorldPoint(i,j),t,s);
                    
                    //if(t < cip && t >= ci )
                    //{
                    cout << s << endl;
                    //exit(0);
                    glm::vec2 p = terrains[other].getWorldPoint(i,j);
                    Point p2 = Point(p.x,p.y);
                    vect.push_back(p2);
                    terrains[other].setactive(k,s,true);
                    terrains[index].getNearestIndex(terrains[other].getWorldPoint(s,k),t,s);
                    terrains[index].setactive(t,terrains[index].getWidth() - s,false);
                    //}
                }
                j = ceil(upperj);
            }
            else
            {
                //vects.push_back(terrains[index].getWorldPoint(j,i));
                glm::vec2 p = terrains[index].getWorldPoint(i,j);
                Point p2 = Point(p.x,p.y);
                vect.push_back(p2);
            }
        }


    }
}

void terrainManager::selectTerrain(int i)
{
    selected = i;
    terrains[i].setactive(true);
}
void terrainManager::addRaster(string filename)
{
    terrains[selected].addRaster(filename);
}
void terrainManager::addMask(string filename)
{
    terrains[selected].addMask(filename);
}

void terrainManager::selectTexture(int i,int bn)
{
    terrains[selected].selectTexture(i,bn);
}
