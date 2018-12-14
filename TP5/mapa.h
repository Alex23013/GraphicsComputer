
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <stdio.h>
#include <vector>
#include <numeric>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
using namespace std;


class PuntoTerreno
{
public:
	GLfloat s, t; // Coordenadas de texturas
	GLfloat nx, ny, nz; // Coordenadas de la normal
	GLfloat x, y, z; // posición del vértice

	PuntoTerreno (float x,float y,float z){
		this ->x = x;
		this ->y = y;
		this ->z = z;
	}

	void print(){
		cout<<" ( "<<x<<" , "<< y<<" , "<<z<<" ) ";
	}
} ;

class Terreno
{
  public :  
    Terreno(){}
    ~Terreno(){}
    bool load(char *filename) ; // carga de un archivo de modelo digital de terreno
    vector<unsigned int> genIndices(unsigned int xTotal, unsigned int zTotal) ;    
    
    
    void computeNormals(); // calcula las normales de cada vertice
    void display();
    void genTexCoords(unsigned int xTotal, unsigned int zTotal);
    void genNormals(const vector<unsigned int> &indices, const vector<PuntoTerreno>& points) ;    
    GLint texture;
    int nb_pt_x, nb_pt_z; // quantidad de punto en X y Z
    vector<PuntoTerreno> puntos;
  private:
    
    float dist_x, dist_z; // distancia entre dos puntos segun X y Z
    
    vector<unsigned int> indices ; // Tabla que contiene los indicess

};

bool Terreno :: load(char *filename)
{
    ifstream fichero;
    fichero.open(("fontvieille.txt"));
    string frase,token;
    int line=1;
    int linea_abre=1;

    if (fichero.fail()) 
    {
        cout << "Error al abrir el fontvieille.txt" << endl;
    } 
    else {
       float iz = 0.0f;
       dist_x = 50.0f;
       dist_z = 50.0f;
            while (!fichero.eof()) 
            {
                getline(fichero,frase);
                istringstream iss(frase);
                float i=0.0f;
	        
	        while(getline(iss,token,' ') && i<101)
		    {
		        PuntoTerreno a (dist_x*i,atoi(token.c_str()),-(dist_z*iz));
		        puntos.push_back(a);
		        i++;
		    }
            iz++;   
	    }
        cout<<"Puntos leidos"<<puntos.size()<<endl;
        fichero.close();
    }
    for(int i=0;i<puntos.size();i++) //escala para que se vea en la ventana
    	{
           puntos[i].x=puntos[i].x*0.01;
           puntos[i].y=puntos[i].y*0.01;
           puntos[i].z =puntos[i].z*0.01;
        }
    nb_pt_x = 101;
    nb_pt_z = 101;
    indices = genIndices(nb_pt_x,nb_pt_z);
    genTexCoords(nb_pt_x,nb_pt_z);   
    genNormals(indices, puntos); 
    return true;
}

vector<unsigned int> Terreno :: genIndices(unsigned int xTotal, unsigned int zTotal) 
{
    /* In a regular mesh:
    * A is for the evens points rows.
    * B is for the odds points rows.
    * last1 and last2 are the last indices of the last triangle.
    */
    vector<unsigned int> indices(3 * (xTotal - 1) * 2 * (zTotal - 1));
    size_t idx = 0;
    for (unsigned int i = 0; i < zTotal - 1; i++) {
        unsigned int idxA = i * xTotal;
        unsigned int idxB = (i + 1) * xTotal;
        unsigned int last1 = idxA;
        unsigned int last2 = idxB;
        for (unsigned int j = 0 ; j < xTotal - 1; j++) {
            idxA++;
            idxB++;

            indices[idx] = last1;
            indices[idx + 1] = idxB;
            indices[idx + 2] = last2;

            last2 = idxA;

            indices[idx + 3] = last1;
            indices[idx + 4] = last2;
            indices[idx + 5] = idxB;
            idx += 6;

            last1 = last2;
            last2 = idxB;
        }
    }
    return indices;
}

const GLfloat floorAmbient[4] = {0.5f, 0.5f, 0.5f, 1.0f};
const GLfloat floorDiffuse[4] = {0.7f, 0.7f, 0.7f, 1.0f};
const GLfloat floorSpecular[4] = {0.9f, 0.9f, 0.9f, 1.0f};
const GLfloat floorShininess = 3.0f;

void Terreno:: display()
{  
    /* cout<<"draw pouints\n";
    glBegin(GL_POINTS);
    glColor3f(1.5f,0.0f,0.0f);
    for(int i=0;i<puntos.size();i++)
    {
        glVertex3f(puntos[i].x,puntos[i].y,puntos[i].z);
    }	
    glEnd();
    */
    glBindTexture(GL_TEXTURE_2D, texture);

    glMaterialfv(GL_FRONT, GL_AMBIENT, floorAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, floorDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, floorSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, floorShininess);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glInterleavedArrays(GL_T2F_N3F_V3F, sizeof(PuntoTerreno),&puntos[0]);
    //cout<<"puntos"<<puntos.size()<<" indices: "<<indices.size()<<endl;
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT,&indices[0]); 
}

void Terreno:: genTexCoords(unsigned int xTotal, unsigned int zTotal) {
    vector<pair<float,float>> texCoords;
    texCoords.reserve(xTotal * zTotal);
    float sStep = 1.0f/float(xTotal - 1);
    float tStep = 1.0f/float(zTotal - 1);

    for (float s = 0.0f; s <= 1.0f; s += sStep) {
        for (float t = 0.0f; t <= 1.0f; t += tStep) {        
            texCoords.emplace_back(make_pair(s,t));
        }
    }
    cout<<"tex Coord: "<<  texCoords.size();
    for(int i = 0; i< texCoords.size();i++){
        puntos[i].s = texCoords[i].first;
        puntos[i].t = texCoords[i].second;
    }
}

void Terreno:: genNormals(const vector<unsigned int> &indices, const vector<PuntoTerreno>& points) 
{
  vector<vector<glm::vec3>> trianglesNors(points.size(), vector<glm::vec3>());
  vector<glm::vec3> vertexNors(points.size(), glm::vec3());

  for (size_t i = 0; i < indices.size(); i += 3) {

    const glm::vec3 &first = glm::vec3(points[indices[i]].x, points[indices[i]].y, points[indices[i]].z);
    const glm::vec3 &second = glm::vec3(points[indices[i+1]].x, points[indices[i+1]].y, points[indices[i+1]].z);
    const glm::vec3 &third = glm::vec3(points[indices[i+2]].x, points[indices[i+2]].y, points[indices[i+2]].z);
    const glm::vec3 one = second - first;
    const glm::vec3 two = third - first;
    const glm::vec3 norm = glm::normalize(glm::cross(one,two));

    trianglesNors[indices[i]].push_back(norm);
    trianglesNors[indices[i + 1]].push_back(norm);
    trianglesNors[indices[i + 2]].push_back(norm);
  }

  for (size_t i = 0; i < vertexNors.size(); i++) {
    vertexNors[i] = accumulate(trianglesNors[i].begin(), trianglesNors[i].end(),
                               glm::vec3(0));
    vertexNors[i] /= trianglesNors[i].size();
    puntos[i].nx = vertexNors[i].x;
    puntos[i].ny = vertexNors[i].y;
    puntos[i].nz = vertexNors[i].z;
  }

}