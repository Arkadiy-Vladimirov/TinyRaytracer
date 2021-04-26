#include "common.h"
//#include "Image.h"
//#include "lin_al.hpp"
#include "graphicObjects.hpp"
#include <stdio.h>
#include <cstdlib>

#define GLFW_DLL
#include <GLFW/glfw3.h>

constexpr GLsizei WINDOW_WIDTH = 1024, WINDOW_HEIGHT = 1024;
//constexpr GLsizei WINDOW_WIDTH = 320, WINDOW_HEIGHT = 320;

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


  //___________________________scene_definition____________________________
    //Vec<3,float> a(1,2,3), b(3,2,1);
    //Vec<3,float> c, d;
    //c = d = a + b;
    //fprintf(stdout,"%f\n",c.x);
    //fprintf(stdout,"%f\n",c.y);
    //fprintf(stdout,"%f\n",c.z); 
    Vec3f camera_position(0,0,0), view_direction(1,0,0); 
    Camera camera(camera_position, view_direction);
    Scene scene;


    Vec3f s1_pos(0,0,100000); float s1_rad = 99990; Color s1_col(64,96,64);
    scene[0] = new DiffuseBall(Repere(s1_pos), s1_rad, s1_col);//floor
    //__________fwd,lft,dwn___
    Vec3f s2_pos(13,5,0); float s2_rad = 1; Color s2_col(255,255,255);
    scene[1] = new EmittingBall(Repere(s2_pos), s2_rad, s2_col);

    Vec3f s3_pos(13,0,0); float s3_rad = 3; Color s3_col(64,64,255);
    scene[2] = new DiffuseBall(Repere(s3_pos), s3_rad, s3_col);
     
    Vec3f s4_pos(0,0,0); float s4_rad = 50; Color s4_col(128,128,192);
    scene[3] = new EmittingBall(Repere(s4_pos), s4_rad, s4_col);//sky

    Vec3f s5_pos(8,2,0); float s5_rad = 1; //Color s5_col(0,0,255);
    scene[4] = new RefractiveBall(Repere(s5_pos), s5_rad);

    Vec3f vert[3] = {Vec3f(7,-1,0), Vec3f(7,-2,0), Vec3f(7,-1.5, sqrt(3)/2)};
    scene[5] = new Polygon(vert);

    /*float rn1 = std::rand(); rn1 = rn1/RAND_MAX;
    float rn2 = std::rand(); rn2 = rn2/RAND_MAX;
    float rn3 = std::rand(); rn3 = rn3/RAND_MAX;
    fprintf(stdout,"%f\n",rn1);
    fprintf(stdout,"%f\n",rn2);
    fprintf(stdout,"%f\n",rn3);

    int rm = RAND_MAX;
    fprintf(stdout,"%d\n",rm);*/

	  Image screenBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 3);
    screenBuffer = camera.RenderImage(scene);
  //_____________

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

  glfwTerminate();
	
  return 0;

} catch(const char *exception) {
    fprintf(stderr,"%s\n",exception);
  }
  return 1;
}

