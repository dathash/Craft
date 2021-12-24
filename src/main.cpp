/*
 ~The Craft~
 CSC 471 Final Project
 Alex Hartford
 Fall 2021
 Professor Zoe Wood
 */

#include <iostream>
#include <glad/glad.h>
#include <irrKlang-64bit-1.6.0/include/irrKlang.h>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Texture.h"
#include "Bezier.h"
#include "Spline.h"
#include "particleSys.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;
using namespace irrklang;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	//the partricle system
	particleSys *thePartSystem;

   // Mat shader
	std::shared_ptr<Program> prog;

   // Texture Shader
	std::shared_ptr<Program> texProg;

   // Particle Shader
	std::shared_ptr<Program> partProg;

   //ISoundEngine *SoundEngine = createIrrKlangDevice();

	// OpenGL handle to texture data used in particle (alpha.bmp)
	shared_ptr<Texture> partTex;

	//our geometry
	shared_ptr<Shape> sphere;

   shared_ptr<Shape> andre;
   shared_ptr<Shape> bricks;
   shared_ptr<Shape> cracked;
   shared_ptr<Shape> frame;

	vector<shared_ptr<Shape>> bonfire;
	vector<shared_ptr<Shape>> anvil;
	vector<shared_ptr<Shape>> shield;

	//global data for ground plane - direct load constant defined CPU data to GPU (not obj)
	GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj;
	int g_GiboLen;
	//ground VAO
	GLuint GroundVertexArrayID;
   
   //quad VAO
   GLuint quad_VertexArrayID;
   GLuint quad_vertexbuffer;

	//the image to use as a texture (ground)
	shared_ptr<Texture> texture0;
   // Sky texture
	shared_ptr<Texture> texture1;	
/*
   // flame texture
	shared_ptr<Texture> flameq;	
   // godo texture
	shared_ptr<Texture> godoq;
   // anvil texture
	shared_ptr<Texture> anvilq;	
   // fear texture
	shared_ptr<Texture> fearq;	
   // water texture
	shared_ptr<Texture> waterq;	
*/
	//global data (larger program should be encapsulated)
   //TODO Clean up these and encapsulate)
	vec3 gMin;
   vec3 eye = vec3(-3, 0, 0);
   vec3 lookAtPoint = vec3(0, 0, -1);
   vec3 u = vec3(1, 0, 0);
   vec3 w = vec3(0, 0, 1);
	float gRot = 0;
	float gCamH = 0;
   float theta = 0;
   float phi = 0;
   double deltaX = 0;
   double deltaY = 0;
   float speed = 0.03;
   double PI = 3.141592653879;
	//animation data
	float lightTrans = 0;
	float gTrans = -3;
	float sTheta = 0;
	float eTheta = 0;
	float hTheta = 0;

   //movement vars
   bool zoomIn = false;
   bool zoomOut = false;
   bool strafeRight = false;
   bool strafeLeft = false;
   
	//some particle variables
	float t = 0.0f; //reset in init
	float h = 0.01f;

   //quote vars
   bool flamequote = false;
   bool godoquote = false;
   bool anvilquote = false;
   bool fearquote = false;
   bool waterquote = false;

   Spline splinepath[2];
   bool goCamera = false;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
         speed = 0.05;
		}
		if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
         speed = 0.03;
		}
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_W && action == GLFW_PRESS) {
         zoomIn = true;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
         zoomIn = false;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS) {
         strafeLeft = true;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
         strafeLeft = false;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS) {
         zoomOut = true;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
         zoomOut = false;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS) {
         strafeRight = true;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
         strafeRight = false;
		}
		if (key == GLFW_KEY_F && action == GLFW_PRESS){
			gCamH  -= 0.25;
		}

		if (key == GLFW_KEY_Q && action == GLFW_PRESS){
			lightTrans += 0.5;
		}
		if (key == GLFW_KEY_E && action == GLFW_PRESS){
			lightTrans -= 0.5;
		}
		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
		if (key == GLFW_KEY_G && action == GLFW_RELEASE) {
         goCamera = !goCamera;
		}
      /*
      if (key == GLFW_KEY_Q && action == GLFW_RELEASE) {
         anvilquote = !anvilquote;
      }
      */
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			 glfwGetCursorPos(window, &posX, &posY);
		}
	}

	void scrollCallback(GLFWwindow *window, double in_deltaX, double in_deltaY)
	{
      deltaX = in_deltaX;
      deltaY = in_deltaY;
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

   void resize_obj(std::vector<tinyobj::shape_t> &shapes){
      float minX, minY, minZ;
      float maxX, maxY, maxZ;
      float scaleX, scaleY, scaleZ;
      float shiftX, shiftY, shiftZ;
      float epsilon = 0.001;

      minX = minY = minZ = 1.1754E+38F;
      maxX = maxY = maxZ = -1.1754E+38F;

      //Go through all vertices to determine min and max of each dimension
      for (size_t i = 0; i < shapes.size(); i++) {
         for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
            if(shapes[i].mesh.positions[3*v+0] < minX) minX = shapes[i].mesh.positions[3*v+0];
            if(shapes[i].mesh.positions[3*v+0] > maxX) maxX = shapes[i].mesh.positions[3*v+0];

            if(shapes[i].mesh.positions[3*v+1] < minY) minY = shapes[i].mesh.positions[3*v+1];
            if(shapes[i].mesh.positions[3*v+1] > maxY) maxY = shapes[i].mesh.positions[3*v+1];

            if(shapes[i].mesh.positions[3*v+2] < minZ) minZ = shapes[i].mesh.positions[3*v+2];
            if(shapes[i].mesh.positions[3*v+2] > maxZ) maxZ = shapes[i].mesh.positions[3*v+2];
         }
      }

 //From min and max compute necessary scale and shift for each dimension
      float maxExtent, xExtent, yExtent, zExtent;
      xExtent = maxX-minX;
      yExtent = maxY-minY;
      zExtent = maxZ-minZ;
      if (xExtent >= yExtent && xExtent >= zExtent) {
         maxExtent = xExtent;
      }
      if (yExtent >= xExtent && yExtent >= zExtent) {
         maxExtent = yExtent;
      }
      if (zExtent >= xExtent && zExtent >= yExtent) {
         maxExtent = zExtent;
      }
      scaleX = 2.0 /maxExtent;
      shiftX = minX + (xExtent/ 2.0);
      scaleY = 2.0 / maxExtent;
      shiftY = minY + (yExtent / 2.0);
      scaleZ = 2.0/ maxExtent;
      shiftZ = minZ + (zExtent)/2.0;

      //Go through all verticies shift and scale them
      for (size_t i = 0; i < shapes.size(); i++) {
         for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
            shapes[i].mesh.positions[3*v+0] = (shapes[i].mesh.positions[3*v+0] - shiftX) * scaleX;
            assert(shapes[i].mesh.positions[3*v+0] >= -1.0 - epsilon);
            assert(shapes[i].mesh.positions[3*v+0] <= 1.0 + epsilon);
            shapes[i].mesh.positions[3*v+1] = (shapes[i].mesh.positions[3*v+1] - shiftY) * scaleY;
            assert(shapes[i].mesh.positions[3*v+1] >= -1.0 - epsilon);
            assert(shapes[i].mesh.positions[3*v+1] <= 1.0 + epsilon);
            shapes[i].mesh.positions[3*v+2] = (shapes[i].mesh.positions[3*v+2] - shiftZ) * scaleZ;
            assert(shapes[i].mesh.positions[3*v+2] >= -1.0 - epsilon);
            assert(shapes[i].mesh.positions[3*v+2] <= 1.0 + epsilon);
         }
      }
   }

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.72f, .84f, 1.06f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

      CHECKED_GL_CALL(glEnable(GL_BLEND));
      CHECKED_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
      CHECKED_GL_CALL(glPointSize(240.0f));

      //SoundEngine->play2D((resourceDirectory + "/guts.wav").c_str(), true);

		// Initialize the GLSL program that we will use for local shading
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("MatAmb");
		prog->addUniform("MatDif");
		prog->addUniform("MatSpec");
		prog->addUniform("MatShine");
		prog->addUniform("lightPos");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addAttribute("vertTex");

		// Initialize the GLSL program that we will use for texture mapping
		texProg = make_shared<Program>();
		texProg->setVerbose(true);
		texProg->setShaderNames(resourceDirectory + "/tex_vert.glsl", resourceDirectory + "/tex_frag0.glsl");
		texProg->init();
		texProg->addUniform("P");
		texProg->addUniform("V");
		texProg->addUniform("M");
		texProg->addUniform("flip");
		texProg->addUniform("Texture0");
		texProg->addUniform("MatShine");
		texProg->addUniform("lightPos");
		texProg->addAttribute("vertPos");
		texProg->addAttribute("vertNor");
		texProg->addAttribute("vertTex");

		// Initialize the GLSL program that we will use for particles
		partProg = make_shared<Program>();
		partProg->setVerbose(true);
		partProg->setShaderNames(
			resourceDirectory + "/part_vert.glsl",
			resourceDirectory + "/part_frag.glsl");
		if (! partProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		partProg->addUniform("P");
		partProg->addUniform("M");
		partProg->addUniform("V");
		partProg->addUniform("alphaTexture");
      partProg->addAttribute("pColor");
		partProg->addAttribute("vertPos");

		//read in a load the texture
		texture0 = make_shared<Texture>();
  		texture0->setFilename(resourceDirectory + "/stone.jpeg");
  		texture0->init();
  		texture0->setUnit(0);
  		texture0->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

  		texture1 = make_shared<Texture>();
  		texture1->setFilename(resourceDirectory + "/sky.jpg");
  		texture1->init();
  		texture1->setUnit(1);
  		texture1->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
/*
  		flameq = make_shared<Texture>();
  		flameq->setFilename(resourceDirectory + "/flameq.jpg");
  		flameq->init();
  		flameq->setUnit(2);
  		flameq->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		godoq = make_shared<Texture>();
  		godoq->setFilename(resourceDirectory + "/godoq.jpg");
  		godoq->init();
  		godoq->setUnit(3);
  		godoq->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		anvilq = make_shared<Texture>();
  		anvilq->setFilename(resourceDirectory + "/anvilq.jpg");
  		anvilq->init();
  		anvilq->setUnit(4);
  		anvilq->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		fearq = make_shared<Texture>();
  		fearq->setFilename(resourceDirectory + "/fearq.jpg");
  		fearq->init();
  		fearq->setUnit(5);
  		fearq->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		waterq = make_shared<Texture>();
  		waterq->setFilename(resourceDirectory + "/waterq.jpg");
  		waterq->init();
  		waterq->setUnit(6);
  		waterq->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
*/

      //initialize spline
      splinepath[0] = Spline(glm::vec3(-7,0,0), glm::vec3(0, 0, 14), glm::vec3(7, 0, 0), 10);
      splinepath[1] = Spline(glm::vec3(7, 0, 0), glm::vec3(0,0,-14), glm::vec3(-7, 0, 0), 10);

      //initialize particle system
		thePartSystem = new particleSys(vec3(0, -1, 0));
		thePartSystem->gpuSetup();
	}

	void initGeom(const std::string& resourceDirectory)
	{
 		vector<tinyobj::shape_t> TOshapes;
 		vector<tinyobj::material_t> objMaterials;
 		string errStr;
		//load in the mesh and make the shape(s)
 		bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/sphereWTex.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			sphere = make_shared<Shape>();
			sphere->createShape(TOshapes[0]);
			sphere->measure();
			sphere->init();
		}

 		vector<tinyobj::shape_t> TOshapesA;
 		vector<tinyobj::material_t> objMaterialsA;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesA, objMaterialsA, errStr, (resourceDirectory + "/andre.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			andre = make_shared<Shape>();
			andre->createShape(TOshapesA[0]);
			andre->measure();
			andre->init();
		}

		// Initialize bonfire mesh.
		vector<tinyobj::shape_t> TOshapesB;
 		vector<tinyobj::material_t> objMaterialsB;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesB, objMaterialsB, errStr, (resourceDirectory + "/bonfire.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {	
		   resize_obj(TOshapesB);
         for (int i = 0; i < TOshapesB.size(); ++i) {
            bonfire.push_back(make_shared<Shape>());
            bonfire[i]->createShape(TOshapesB[i]);   
            //bonfire->computeNormals();
            bonfire[i]->measure();
            bonfire[i]->init();
         }   
		}

		vector<tinyobj::shape_t> TOshapesC;
 		vector<tinyobj::material_t> objMaterialsC;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesC, objMaterialsC, errStr, (resourceDirectory + "/anvil.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {	
		   resize_obj(TOshapesC);
         for (int i = 0; i < TOshapesC.size(); ++i) {
            anvil.push_back(make_shared<Shape>());
            anvil[i]->createShape(TOshapesC[i]);   
            //bonfire->computeNormals();
            anvil[i]->measure();
            anvil[i]->init();
         }   
		}

		vector<tinyobj::shape_t> TOshapesD;
 		vector<tinyobj::material_t> objMaterialsD;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesD, objMaterialsD, errStr, (resourceDirectory + "/shield.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {	
		   resize_obj(TOshapesD);
         for (int i = 0; i < TOshapesD.size(); ++i) {
            shield.push_back(make_shared<Shape>());
            shield[i]->createShape(TOshapesD[i]);   
            //bonfire->computeNormals();
            shield[i]->measure();
            shield[i]->init();
         }   
		}

 		vector<tinyobj::shape_t> TOshapesF;
 		vector<tinyobj::material_t> objMaterialsF;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesF, objMaterialsF, errStr, (resourceDirectory + "/bricks.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else { 
		   resize_obj(TOshapesF);
			bricks = make_shared<Shape>();
			bricks->createShape(TOshapesF[0]);
			bricks->measure();
			bricks->init();
		}

 		vector<tinyobj::shape_t> TOshapesG;
 		vector<tinyobj::material_t> objMaterialsG;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesG, objMaterialsG, errStr, (resourceDirectory + "/sword.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else { 
		   resize_obj(TOshapesG);
		   cracked = make_shared<Shape>();
			cracked->createShape(TOshapesG[0]);
		   cracked->measure();
			cracked->init();
		}

 		vector<tinyobj::shape_t> TOshapesH;
 		vector<tinyobj::material_t> objMaterialsH;
		//load in the mesh and make the shape(s)
 		rc = tinyobj::LoadObj(TOshapesH, objMaterialsH, errStr, (resourceDirectory + "/cube.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			frame = make_shared<Shape>();
			frame->createShape(TOshapesH[0]);
			frame->measure();
			frame->init();
		}
		//code to load in the ground plane (CPU defined data passed to GPU)
		initGround();
	}

	//directly pass quad for the ground to the GPU
	void initGround() {

		float g_groundSize = 20;
		float g_groundY = -0.25;

  		// A x-z plane at y = g_groundY of dimension [-g_groundSize, g_groundSize]^2
		float GrndPos[] = {
			-g_groundSize, g_groundY, -g_groundSize,
			-g_groundSize, g_groundY,  g_groundSize,
			g_groundSize, g_groundY,  g_groundSize,
			g_groundSize, g_groundY, -g_groundSize
		};

		float GrndNorm[] = {
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0
		};

		static GLfloat GrndTex[] = {
      		0, 0, // back
      		0, 1,
      		1, 1,
      		1, 0 };

      	unsigned short idx[] = {0, 1, 2, 0, 2, 3};

		//generate the ground VAO
      	glGenVertexArrays(1, &GroundVertexArrayID);
      	glBindVertexArray(GroundVertexArrayID);

      	g_GiboLen = 6;
      	glGenBuffers(1, &GrndBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndPos), GrndPos, GL_STATIC_DRAW);

      	glGenBuffers(1, &GrndNorBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndNorm), GrndNorm, GL_STATIC_DRAW);

      	glGenBuffers(1, &GrndTexBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, sizeof(GrndTex), GrndTex, GL_STATIC_DRAW);

      	glGenBuffers(1, &GIndxBuffObj);
     	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
      	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
      }

      //code to draw the ground plane
     void drawGround(shared_ptr<Program> curS) {
     	curS->bind();
     	glBindVertexArray(GroundVertexArrayID);
		//draw the ground plane 
  		SetModel(vec3(0, -1, 0), 0, 0, 1, curS);
  		glEnableVertexAttribArray(0);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
  		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  		glEnableVertexAttribArray(1);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
  		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  		glEnableVertexAttribArray(2);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
  		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

   		// draw!
  		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
  		glDrawElements(GL_TRIANGLES, g_GiboLen, GL_UNSIGNED_SHORT, 0);

  		glDisableVertexAttribArray(0);
  		glDisableVertexAttribArray(1);
  		glDisableVertexAttribArray(2);
  		curS->unbind();
     }

	// Code to load in the texture
	void initTex(const std::string& resourceDirectory)
	{
		partTex = make_shared<Texture>();
		partTex->setFilename(resourceDirectory + "/alpha.bmp");
		partTex->init();
		partTex->setUnit(0);
		partTex->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	}

        //helper function to pass material data to the GPU
      void SetMaterial(shared_ptr<Program> curS, int i) {

         switch (i) {
            case 0: // Copper
               glUniform3f(curS->getUniform("MatAmb"), 0.32, 0.22, 0.02);
               glUniform3f(curS->getUniform("MatDif"), 0.78, 0.57, 0.11);
               glUniform3f(curS->getUniform("MatSpec"), 0.99, 0.94, 0.80);
               glUniform1f(curS->getUniform("MatShine"), 100);
            break;
            case 1: // Ground
               glUniform3f(curS->getUniform("MatAmb"), 0.1, 0.1, 0.1);
               glUniform3f(curS->getUniform("MatDif"), 0.3, 0.1, 0.1);
               glUniform3f(curS->getUniform("MatSpec"), 0.0, 0.0, 0.0);
               glUniform1f(curS->getUniform("MatShine"), 4.0);
            break;
            case 2: // White
               glUniform3f(curS->getUniform("MatAmb"), 0.9, 0.9, 0.9);
               glUniform3f(curS->getUniform("MatDif"), 1.0, 0.9, 0.9);
               glUniform3f(curS->getUniform("MatSpec"), 0.2, 0.2, 0.2);
               glUniform1f(curS->getUniform("MatShine"), 27.9);
            break;
            case 3: // Chrome
               glUniform3f(curS->getUniform("MatAmb"), 0.25, 0.25, 0.25);
               glUniform3f(curS->getUniform("MatDif"), 0.4, 0.4, 0.4);
               //glUniform3f(curS->getUniform("MatSpec"), 0.7, 0.7, 0.7);
               glUniform3f(curS->getUniform("MatSpec"), 0.2, 0.2, 0.2);
               glUniform1f(curS->getUniform("MatShine"), 90);
            break;
         }
      }

	/* helper function to set model trasnforms */
  	void SetModel(vec3 trans, float rotY, float rotX, float sc, shared_ptr<Program> curS) {
  		mat4 Trans = glm::translate( glm::mat4(1.0f), trans);
  		mat4 RotX = glm::rotate( glm::mat4(1.0f), rotX, vec3(1, 0, 0));
  		mat4 RotY = glm::rotate( glm::mat4(1.0f), rotY, vec3(0, 1, 0));
  		mat4 ScaleS = glm::scale(glm::mat4(1.0f), vec3(sc));
  		mat4 ctm = Trans*RotX*RotY*ScaleS;
  		glUniformMatrix4fv(curS->getUniform("M"), 1, GL_FALSE, value_ptr(ctm));
  	}

	void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M) {
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
   	}

   void updateUsingCameraPath(float frametime)  {
      if (goCamera) {
         if(!splinepath[0].isDone()){
            splinepath[0].update(frametime);
            eye = splinepath[0].getPosition();
         } else if (!splinepath[1].isDone()) {
            splinepath[1].update(frametime);
            eye = splinepath[1].getPosition();
         }
         else { 
            splinepath[0] = Spline(glm::vec3(-7,0,0), glm::vec3(0, 0, 14), glm::vec3(7, 0, 0), 10);
            splinepath[1] = Spline(glm::vec3(7, 0, 0), glm::vec3(0,0,-14), glm::vec3(-7, 0, 0), 10);
         }
      }
   }


	void render(float frametime) {
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
		//Use the matrix stack for Lab 6
		float aspect = width/(float)height;

		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
      glm::mat4 View;
		auto Model = make_shared<MatrixStack>();

      //update the camera position
      updateUsingCameraPath(frametime);


		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.1f, 100.0f);

      theta += radians((deltaX / width) * 180);
      phi -= radians((deltaY / height) * 180);
      if (phi > 1.4) {
         phi = 1.4;
      } else if (phi < -1.4) {
         phi = -1.4;
      }
      glm::vec3 up = glm::vec3(0, 1, 0);
      
      lookAtPoint = glm::vec3(cos(phi) * cos(theta),
                              sin(phi),
                              cos(phi) * cos((PI/2.0)-theta));
   
      w = -1.0f * glm::normalize(lookAtPoint);
      u = glm::cross(up, w);
      glm::vec3 v = glm::cross(w, u);

      
      if (goCamera) {
         View = glm::lookAt(eye, vec3(0, 0.5, 0), up);
      }
      else {
         View = glm::lookAt(eye, eye + lookAtPoint, up);
      }

      float oldx = eye.x;
      float oldy = eye.y;
      float oldz = eye.z;
      float oldlax = lookAtPoint.x;
      float oldlay = lookAtPoint.y;
      float oldlaz = lookAtPoint.z;
      

		if (zoomIn) {
         lookAtPoint += -1.0f * (speed * w);
         eye += -1.0f * (speed * w);
		}
		if (strafeLeft) {
         lookAtPoint += -1.0f * (speed * u);
         eye += -1.0f * (speed * u);
		}
		if (zoomOut) {
         lookAtPoint += (speed * w);
         eye += (speed * w);
		}
		if (strafeRight){
         lookAtPoint += (speed * u);
         eye += (speed * u);
		}

      // Skybox Collision
      if (pow(pow(eye.x, 2) + pow(eye.y, 2) + pow(eye.z, 2), 0.5) > 7.5) {
         eye.x = oldx;
         eye.y = oldy;
         eye.z = oldz;
         lookAtPoint.x = oldlax;
         lookAtPoint.y = oldlay;
         lookAtPoint.z = oldlaz;
      }

      // Floor Collision
      if (eye.y < -0.5) {
         eye.y = oldy;
         lookAtPoint.y = oldlay;
      }
      // cout << eye.x << ", " << eye.y << ", " << eye.z << "\n";

		thePartSystem->setCamera(View);

		// Draw Textured Components
		texProg->bind();
      
		glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(texProg->getUniform("V"), 1, GL_FALSE, value_ptr(View));
		glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		glUniform3f(texProg->getUniform("lightPos"), 0.0, 0.5, 0.0);
		glUniform1f(texProg->getUniform("MatShine"), 50);	
		glUniform1i(texProg->getUniform("flip"), 1);

      texture1->bind(texProg->getUniform("Texture0"));

		//draw big background sphere
		glUniform1i(texProg->getUniform("flip"), -1);
		Model->pushMatrix();
			Model->loadIdentity();
			Model->scale(vec3(8.0));
         //Makes glUniform call for "M".
			setModel(texProg, Model);
         //Draw Sphere		
         sphere->draw(texProg);
		Model->popMatrix();

      texture1->unbind();
      
      //draw ground
      texture0->bind(texProg->getUniform("Texture0"));
		glUniform1i(texProg->getUniform("flip"), 1);
      //Func contains transforms
		drawGround(texProg);
      texture0->unbind();

/*
      if (flamequote) {
         flameq->bind(tex_prog->getUniform("texture0"));
         //Func contains transforms
         //drawText();
         flameq->unbind();
      }

      if (godoquote) {
         godoq->bind(tex_prog->getUniform("texture0"));
         //Func contains transforms
         //drawText();
         godoq->unbind();
      }

      if (anvilquote) {
         anvilq->bind(texProg->getUniform("Texture0"));
		   glUniform1i(texProg->getUniform("flip"), 1);
         SetModel(vec3(0, 2, 0), 0, 0, 2.0, texProg);
         frame->draw(texProg);
         anvilq->unbind();
      }

      if (fearquote) {
         fearq->bind(tex_prog->getUniform("texture0"));
         //Func contains transforms
         //drawText();
         fearq->unbind();
      }

      if (waterquote) {
         waterq->bind(tex_prog->getUniform("texture0"));
         //Func contains transforms
         //drawText;
         waterq->unbind();
      }
*/
       
		texProg->unbind();
		


		// draw the bonfire
      prog->bind();

		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View));
		glUniform3f(prog->getUniform("lightPos"), 0.0, -0.0, 0.0);

      // Draw Bonfire
      SetModel(vec3(0, -.2, 0), 0, 0, 2.0, prog);
      SetMaterial(prog, 1);
      bonfire[0]->draw(prog); // Pile
      bonfire[2]->draw(prog); // Mound

      SetMaterial(prog, 0);
      bonfire[1]->draw(prog); // Blade

      bonfire[3]->draw(prog); // Rocks

      SetMaterial(prog, 3);
      bonfire[4]->draw(prog); // Hilt
      SetMaterial(prog, 2);
      bonfire[5]->draw(prog); // Bones

      bonfire[6]->draw(prog); // Skull



      SetMaterial(prog, 3);
      SetModel(vec3(4, -.8, -4), 45, 0, 1.0, prog);
      for (int i = 0; i < anvil.size(); i++) {
         anvil[i]->draw(prog);
      }

      SetModel(vec3(0, -1.2, 4), 0, 0, 2.0, prog);
      andre->draw(prog);

      // Failures.
      SetModel(vec3(-3, -1.1, 2), 0, 90, 1, prog);
      cracked->draw(prog);

      SetModel(vec3(-3.5, -1.1, 2.3), 45, 95, 1, prog);
      cracked->draw(prog);
      
      SetModel(vec3(-3.9, -1.1, 2.9), 0, 80, 1, prog);
      cracked->draw(prog);

      SetModel(vec3(-4.2, -1.1, 2.4), 15, 75, 0.9, prog);
      cracked->draw(prog);

      SetModel(vec3(-3, -1.1, 1.1), 45, 45, 0.9, prog);
      cracked->draw(prog);

      SetModel(vec3(-2, -1.1, 0.5), 0, 77, 0.9, prog);
      cracked->draw(prog);


      // Draw some iron bars!
      SetModel(vec3(4, -1.1, 1), 0, 0, 1, prog);
      bricks->draw(prog);

      SetModel(vec3(3, -1.1, 2), 0, 0, 1, prog);
      bricks->draw(prog);


      // shield
      SetModel(vec3(4.2, -.5, -3.4), 0, 45, 1.0, prog);
      // main
      SetMaterial(prog, 1);
      shield[0]->draw(prog);

      // cross
      SetMaterial(prog, 0);
      shield[1]->draw(prog);
      shield[2]->draw(prog);


      prog->unbind();

		// Draw
		partProg->bind();
		partTex->bind(partProg->getUniform("alphaTexture"));
		CHECKED_GL_CALL(glUniformMatrix4fv(partProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix())));
		CHECKED_GL_CALL(glUniformMatrix4fv(partProg->getUniform("V"), 1, GL_FALSE, value_ptr(View)));
		CHECKED_GL_CALL(glUniformMatrix4fv(partProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix())));
		
		thePartSystem->drawMe(partProg);
		thePartSystem->update();

		partProg->unbind();


		// Pop matrix stacks.
      // Cleanup
		Projection->popMatrix();
	}
};

int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initTex(resourceDir);
	application->initGeom(resourceDir);

   auto lastTime = std::chrono::high_resolution_clock::now();
	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
      auto nextLastTime = std::chrono::high_resolution_clock::now();

      float deltaTime =
         chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - lastTime)
            .count();

      deltaTime *= 0.000001;

      lastTime = nextLastTime;

		// Render scene.
		application->render(deltaTime);

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
