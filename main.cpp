//librerias:
//glm: sudo apt-get install libglm-dev
//freeImage: sudo apt-get install libfreeimage3 libfreeimage-dev
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <glm/glm.hpp>
#include <FreeImage.h>
#define numTEXT 3
using namespace std;
   
int colors = 0;// variable que permite indicar el color seleccionado
float izq_dere= 0.0f; // variable para modificar la posicion de la luz en el eje X
float arriba_abajo = 0.0f; // variable para modificar la posicion de la luz en el eje Y

//texturas
GLuint texID[numTEXT];
char* textureFileNames[numTEXT] = {"assets/metal_test.jpeg","cieloNubes.jpg","vereda.jpg"};//ignorar advertencia por esta linea
void loadTextures() {
  int i;
  glGenTextures(numTEXT,texID);  // Obtener el Id textura
  for (i = 0; i < numTEXT; i++) {
     void* imgData;      // Puntero a data del Archivo
     int imgWidth;   // Ancho de Imagen
     int imgHeight;  // Alto de Imagen
     FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(textureFileNames[i]);
     if (format == FIF_UNKNOWN) {
        printf("Archivo de Imagen desconocido %s\n", textureFileNames[i]);
            continue;
        }
     FIBITMAP* bitmap = FreeImage_Load(format, textureFileNames[i], 0);  //Leer Imagen
     if (!bitmap) {
        printf("Fallo la carga de imagen %s\n", textureFileNames[i]);
        continue;
     }
     FIBITMAP* bitmap2 = FreeImage_ConvertTo24Bits(bitmap);  // Convierte a RGB
     FreeImage_Unload(bitmap);
     imgData = FreeImage_GetBits(bitmap2);
     imgWidth = FreeImage_GetWidth(bitmap2);
     imgHeight = FreeImage_GetHeight(bitmap2);
     if (imgData) {
         printf("Texture cargada %s, tamanio %dx%d\n",
                                     textureFileNames[i], imgWidth, imgHeight);
         int format; // Formato del color.
         if ( FI_RGBA_RED == 0 )
            format = GL_RGB;
         else
            format = GL_BGR;
            glBindTexture( GL_TEXTURE_2D, texID[i] );  // Cargando textura
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, format,GL_UNSIGNED_BYTE, imgData);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
        else {
            printf("Fallo la carga de textura %s\n", textureFileNames[i]);
        }
    }
}
//
bool loadOBJ(const char* path, vector<glm::vec3> & out_vertices,
             vector<glm::vec2> & out_uvs,
             vector<glm::vec3> & out_normals){
    // no sirve para cargar materiales
    vector<unsigned int> vIndices, uvIndices, normalIndices;
    vector<glm::vec3> temp_vertices;
    vector<glm::vec2> temp_uvs;
    vector<glm::vec3> temp_normals;
    FILE * file = fopen(path,"r");
    if(file== NULL){
        printf("No se pudo leer el archivo\n");
        return false;        
    }
    while(true){
        char lineHeader[128];//para cada linea
        int res = fscanf(file,"%s", lineHeader);
        if(res == EOF)//chequea si encuentra el final de la linea
            break;    
        else{
            //verificamos las cabezeras de cada linea y luego parseamos los datos
            if( strcmp(lineHeader, "v" ) == 0){
                glm::vec3 vertex;
                fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
                temp_vertices.push_back(vertex);
            }
            else if( strcmp(lineHeader, "vt") == 0){
                glm::vec2 uv;
                fscanf(file, "%f %f\n", &uv.x, &uv.y);
                temp_uvs.push_back(uv);
            }
            else if( strcmp(lineHeader, "vn") == 0){
                glm::vec3 normal;
                fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
                temp_normals.push_back(normal);
            }
            else if( strcmp(lineHeader, "f") == 0){
                string vertex1, vertex2, vertex3;
                unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
                int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0],&uvIndex[0],
                                     &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                                     &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
                
                if(matches!=9){
                    printf("No se pudo parsear las lineas que contienen las caras\n");
                    return false;
                }
                //guardamos los indices
                vIndices.push_back(vertexIndex[0]);
                vIndices.push_back(vertexIndex[1]);
                vIndices.push_back(vertexIndex[2]);
                uvIndices.push_back(uvIndex[0]);
                uvIndices.push_back(uvIndex[1]);
                uvIndices.push_back(uvIndex[2]);
                normalIndices.push_back(normalIndex[0]);
                normalIndices.push_back(normalIndex[1]);
                normalIndices.push_back(normalIndex[2]);
            }
        }
    }
    //usandolos indices obtenidos guardamos en los vectores para retornar estos valores
    for(unsigned int i=0; i<vIndices.size();i++){
        unsigned int vertexIndex = vIndices[i];
        glm::vec3 vertex = temp_vertices[vertexIndex-1];
        out_vertices.push_back(vertex);
    }
    for(unsigned int i=0; i<uvIndices.size();i++){
        unsigned int uvIndex = uvIndices[i];
        glm::vec2 uv = temp_uvs[uvIndex-1];
        out_uvs.push_back(uv);
    }
    for(unsigned int i=0; i<normalIndices.size();i++){
        unsigned int normalIndex = normalIndices[i];
        glm::vec3 normal = temp_normals[normalIndex-1];
        out_normals.push_back(normal);
    }
    return true;
}
void keyboardFunc( unsigned char key, int x, int y ){
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
    case 'c':
        // modifica el color activo haciando variar la variable global colors
		colors+=1;
        colors%=4;
        break;
    default:
        cout << "Unhandled key press " << key << "." << endl;
    }

	// this will refresh the screen so that the user sees the color change
    glutPostRedisplay();
}
void specialFunc( int key, int x, int y ){
    switch ( key )
    {
    case GLUT_KEY_UP: // modifica la posicion de la luz hacia arriba
		arriba_abajo+=0.5f;
		break;
    case GLUT_KEY_DOWN: // modifica la posicion de la luz hacia abajo
        arriba_abajo-=0.5f;
		break;
    case GLUT_KEY_LEFT: // modifica la posicion de la luz hacia la izquierda
        izq_dere -= 0.5f;
		break;
    case GLUT_KEY_RIGHT:// modifica la posicion de la luz hacia la derecha
        izq_dere += 0.5f;
		break;
    }

	// this will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}
void dibujarObj(vector<glm::vec3> vertices, vector<glm::vec2> uvs, vector <glm::vec3> normales, int indiceTEX){
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texID[indiceTEX]);//textura del metal
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    for(int i=0; i<vertices.size()-2;i++){ //usamos la informacion ya leida para dibujar el objeto
            int size = vertices.size();
            glBegin(GL_TRIANGLES);
            glNormal3d(normales[i].x,normales[i].y,normales[i].z);
            glVertex3d(vertices[i].x,vertices[i].y,vertices[i].z);
            glTexCoord2f(uvs[i].x,uvs[i].y);
            glNormal3d(normales[(i+1)].x,normales[(i+1)].y,normales[(i+1)].z);
            glVertex3d(vertices[(i+1)].x,vertices[(i+1)].y,vertices[(i+1)].z);
            glTexCoord2f(uvs[i+1].x,uvs[i+1].y);
            glNormal3d(normales[(i+2)].x,normales[(i+2)].y,normales[(i+2)].z);
            glVertex3d(vertices[(i+2)].x,vertices[(i+2)].y,vertices[(i+2)].z);
            glTexCoord2f(uvs[i+2].x,uvs[i+2].y);    
            glEnd();
            } 
        glDisable(GL_TEXTURE_2D);
}
void draw(void){
    vector<glm::vec3> vertices;
    vector<glm::vec2> uvs;
    vector<glm::vec3> normales;
    bool test = loadOBJ("assets/test_4.obj",vertices,uvs,normales);
	if(test){ // verificamos si pudo leer el archivo
        glPushMatrix();
        glTranslatef(1.0f,0.0f,0.0f);// posicion inicial del objeto
        //glScalef(0.5f,0.5f,0.5f);
        dibujarObj(vertices,uvs,normales,0);//creo el primer objeto( para cada parte del laberinto, con su indice de textura (0)
        glPopMatrix();
        
        glPushMatrix();
        glTranslatef(2.0,0.0,2.0);
        dibujarObj(vertices,uvs,normales,0);
        
        glPopMatrix();
    }
    
}
void drawScene(void){
    int i;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode( GL_MODELVIEW );  
    glLoadIdentity();             
    // camara posicionada en [0,10,10] mirando hacia [0,0,0] 
    // con [0,0,1] como vector hacia arriba
    gluLookAt(0.0, 10.0, 10.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);
    GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
                                 {0.9, 0.5, 0.5, 1.0},
                                 {0.5, 0.9, 0.3, 1.0},
                                 {0.3, 0.8, 0.9, 1.0}};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors[colors]);

    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
    // posicion de la luz
    GLfloat Lt0pos[] = {1.0f+izq_dere, 1.0f+arriba_abajo, 5.0f, 1.0f};
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);
    draw();
    glutSwapBuffers();
}

void initRendering(){
    glEnable(GL_DEPTH_TEST);   
    glEnable(GL_LIGHTING);     
    glEnable(GL_LIGHT0);       // luz #0.
    //glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    loadTextures();

}
void reshapeFunc(int w, int h){
    if (w > h) {
        glViewport((w - h) / 2, 0, h, h);
    } else {
        glViewport(0, (h - w) / 2, w, w);
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0, 1.0, 1.0, 100.0);
}

int main( int argc, char** argv ){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 800, 600 );
    glutCreateWindow("Test");
    initRendering();
    glutKeyboardFunc(keyboardFunc); 
    glutSpecialFunc(specialFunc);   
    glutReshapeFunc( reshapeFunc );
    glutDisplayFunc( drawScene );
    glutMainLoop( );

    return 0;
}
