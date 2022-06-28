#include "common.h"
#include "Image.h"
#include "lin_al.hpp"
#include "graphicObjects.hpp"
#include <stdio.h>
#include <cstdlib>

#define GLFW_DLL
#include <GLFW/glfw3.h>

constexpr GLsizei WINDOW_WIDTH = 512, WINDOW_HEIGHT = 512;
//const char* save_to = "../resources/315_vladimirov_13.png";

//constexpr GLsizei WINDOW_WIDTH = 1024, WINDOW_HEIGHT = 1024;
//const char* save_to = "../resources/rendered_test_scene.png";

const char* save_to = "../resources/test_scene.png";

struct InputState
{
  bool keys[1024]{}; //массив состояний кнопок - нажата/не нажата
  GLfloat lastX = 400, lastY = 300; //исходное положение мыши
  bool firstMouse = true;
  bool captureMouse         = true;  // Мышка захвачена нашим приложением или нет?
  bool capturedMouseJustNow = false;
} static Input;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		break;
  case GLFW_KEY_1:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case GLFW_KEY_2:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    break;
	default:
		if (action == GLFW_PRESS)
      Input.keys[key] = true;
		else if (action == GLFW_RELEASE)
      Input.keys[key] = false;
	}
}

void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    Input.captureMouse = !Input.captureMouse;

  if (Input.captureMouse)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Input.capturedMouseJustNow = true;
  }
  else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (Input.firstMouse)
  {
    Input.lastX = float(xpos);
    Input.lastY = float(ypos);
    Input.firstMouse = false;
  }

  GLfloat xoffset = float(xpos) - Input.lastX;
  GLfloat yoffset = Input.lastY - float(ypos);

  Input.lastX = float(xpos);
  Input.lastY = float(ypos);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  // ...
}


int initGL()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  std::cout << "Controls: "<< std::endl;
  std::cout << "press right mouse button to capture/release mouse cursor  "<< std::endl;
  std::cout << "press ESC to exit" << std::endl;

	return 0;
}

int main(int argc, char** argv)
{
try {
  //___________________________scene_definition________________________________________________________
    Vec3f camera_position(1,4,-2), view_direction(1,-0.4,0.2);
    //Vec3f camera_position(0,0,-2),  view_direction(1,0,0.3);
    //Vec3f camera_position(0,0,0), view_direction(1,0,0);
    float fov = PI/2; Vec2un res(WINDOW_WIDTH,WINDOW_HEIGHT);
    Camera camera(camera_position, view_direction, fov, res);
    GrObjCollection scene;

    Schlick        glass(1.5,1);
    Schlick        green_glass  (1.5,1,Color(235,235,180));
    Lambert        dark_blue_gypsum   (Color(0  ,0,  255));
    Lambert        lapis_lazuli_gypsum(Color(31 ,71, 136));
    Lambert        desert_rose_gypsum (Color(207,105,119));
    Lambert        grey_gypsum        (Color(153,149,140));
    Lambert        bright_gypsum       (Color(255,250,205));
    SimpleEmission sky_blue_emitter   (Color(117,187,253));
    SimpleEmission night_blue_emitter (Color(0  ,75.5 ,151));
    SimpleEmission white_emitter      (Color(255,255,255));
    SimpleEmission yellow_emitter     (Color(255,255,205));

    //__________fwd,lft,dwn___
    Vec3f s0_pos(0,0,100000); float s0_rad = 99996;
    scene[0] = new Ball(Repere(s0_pos), &desert_rose_gypsum, s0_rad); //floor

    Vec3f s1_pos(0,0,0); float s1_rad = 100;
    scene[1] = new Ball(Repere(s1_pos), &night_blue_emitter, s1_rad); //sky

    Vec3f s2_pos(13,5,2.5); float s2_rad = 1;
    scene[2] = new Ball(Repere(s2_pos), &white_emitter, s2_rad);      //small emitter
     
    {                                                                 //glass cube 
    float d = 1, l = 1, h = 1, k = 2;
    Vec3f s_pos(10,0,0); 
    Vec3f v000(-d,-l,-h), v001(-d,-l,h), v010(-d,l,-h), v011(-d,l,h), v100(d,-l,-h), v101(d,-l,h), v110(d,l,-h), v111(d,l,h);
    Vec3f vert[36] =  {v111,v101,v100, v100,v110,v111,   v000,v001,v011, v011,v010,v000,
                       v111,v011,v001, v001,v101,v111,   v000,v010,v110, v110,v100,v000,
                       v111,v110,v010, v010,v011,v111,   v000,v100,v101, v101,v001,v000};
    scene[3] = new PolygonMesh(Repere(s_pos), &glass, vert, 36, k);
    }

    //cubic pedestal
    {
    float d = 3, l = 3, h = 1, k = 1;
    Vec3f s6_pos(10,0,0); s6_pos.z += 2 + k*h;
    Vec3f v000(-d,-l,-h), v001(-d,-l,h), v010(-d,l,-h), v011(-d,l,h), v100(d,-l,-h), v101(d,-l,h), v110(d,l,-h), v111(d,l,h);
    Vec3f vert[36] = {v111,v101,v100, v100,v110,v111,   v000,v001,v011, v011,v010,v000,
                       v111,v011,v001, v001,v101,v111,   v000,v010,v110, v110,v100,v000,
                       v111,v110,v010, v010,v011,v111,   v000,v100,v101, v101,v001,v000};
    scene[4] = new PolygonMesh(Repere(s6_pos), &lapis_lazuli_gypsum, vert, 36);
    }
   
    Vec3f s7_pos(10,0,0);
    float d = 1, l = 3, h = 2, k = 0.3;
    //panel house
    {
    Vec3f v000(-d,-l,-h), v001(-d,-l,h), v010(-d,l,-h), v011(-d,l,h), v100(d,-l,-h), v101(d,-l,h), v110(d,l,-h), v111(d,l,h);
    Vec3f vert[36] = {v111,v101,v100, v100,v110,v111,   v000,v001,v011, v011,v010,v000,
                       v111,v011,v001, v001,v101,v111,   v000,v010,v110, v110,v100,v000,
                       v111,v110,v010, v010,v011,v111,   v000,v100,v101, v101,v001,v000};
    scene[5] = new PolygonMesh(Repere(s7_pos), &grey_gypsum, vert, 36, k);
    }
    //ligtening windows
    {
    float eps = Ray::GetEpsilon(); d = -d - eps/2;
    float sy = l/30, sz = h/20;
    Vec3f w1(d,l-3*sy,h-3*sz), w2(d,l-8*sy,h-3*sz), w3(d,l-8*sy,h-8*sz), w4(d,l-3*sy,h-8*sz);
    Vec3f w5(d,l-43*sy,h-23*sz), w6(d,l-48*sy,h-23*sz), w7(d,l-48*sy,h-28*sz), w8(d,l-43*sy,h-28*sz);
    Vec3f w9(d,l-13*sy,h-31*sz), w10(d,l-18*sy,h-31*sz), w11(d,l-18*sy,h-36*sz), w12(d,l-13*sy,h-36*sz);
    Vec3f vert[18] = {w1,w2,w3, w3,w4,w1,   w5,w6,w7, w7,w8,w5,   w9,w10,w11, w11,w12,w9};
    scene[6] = new PolygonMesh(Repere(s7_pos), &yellow_emitter, vert, 18, k);
    }

    { 
    Vec3f pos(12,-5,-3); float rad = 1;
    scene[7] = new Ball(Repere(pos), &bright_gypsum, rad);        //bright diffuse ball
    }

    {
    Vec3f pos(10,-6,-4); float rad = 1.5;
    scene[8] = new Ball(Repere(pos), &green_glass, rad);         //green glass ball
    }

    Image screenBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 4);
    screenBuffer = camera.RenderImage(scene);
  //_____________________________________________________________________________________________________

	if(!glfwInit())
    return -1;

//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow*  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "task2 base project", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window); 

	glfwSetKeyCallback        (window, OnKeyboardPressed);  
	glfwSetCursorPosCallback  (window, OnMouseMove); 
  glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
	glfwSetScrollCallback     (window, OnMouseScroll);

	if(initGL() != 0) 
		return -1;
	
  //Reset any OpenGL errors which could be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();

	//Image img("../resources/tex.png");

  Image backgroundImage(screenBuffer);                  //

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  GL_CHECK_ERRORS;
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GL_CHECK_ERRORS;

  //game loop
	while (!glfwWindowShouldClose(window))
	{
		//GLfloat currentFrame = glfwGetTime();
		//deltaTime = currentFrame - lastFrame;
		//lastFrame = currentFrame;
    glfwPollEvents();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
    glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;

		glfwSwapBuffers(window);
  }

  screenBuffer.Save(save_to);
  glfwTerminate();
	
  return 0;

} catch(const char *exception) {
    fprintf(stderr,"%s\n",exception);
  }
  return 1;
}

