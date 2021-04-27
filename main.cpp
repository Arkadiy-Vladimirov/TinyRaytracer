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


  //___________________________scene_definition________________________________________________________
    Vec3f camera_position(0,0,0), view_direction(1,0,0); 
    Camera camera(camera_position, view_direction);
    GrObjCollection scene;

    Schlick        glass(1.5,1);
    Lambert        lapis_lazuli_gypsum(Color(31,71,136));
    Lambert        desert_rose_gypsum(Color(207,105,119));
    SimpleEmission sky_blue_emitter(Color(117,187,253));
    SimpleEmission white_emitter(Color(255,255,255));

    //__________fwd,lft,dwn___
    Vec3f s0_pos(0,0,100000); float s0_rad = 99990;
    scene[0] = new Ball(Repere(s0_pos), &desert_rose_gypsum, s0_rad); //floor

    Vec3f s1_pos(0,0,0); float s1_rad = 1000;
    scene[1] = new Ball(Repere(s1_pos), &sky_blue_emitter, s1_rad);   //sky

    Vec3f s2_pos(13,5,0); float s2_rad = 1;
    scene[2] = new Ball(Repere(s2_pos), &white_emitter, s2_rad);      //small emitter

    Vec3f s3_pos(13,0,0); float s3_rad = 3;
    scene[3] = new Ball(Repere(s3_pos), &lapis_lazuli_gypsum, s3_rad);

    Vec3f s4_pos(8,2,0); float s4_rad = 1;
    scene[4] = new Ball(Repere(s4_pos), &glass, s4_rad);

    Vec3f s6_pos(7,-1,0);
    Vec3f v1(0,0,0), v2(2,0,2), v3(0,2,2), v4(-2,0,2), v5(0,-2,2), v6(0,0,4);
    Vec3f vert1[24] = {v1,v3,v2,  v1,v4,v3, v1,v5,v4, v1,v2,v3, v6,v2,v3, v6,v3,v4, v6,v4,v5, v6,v5,v2};
    scene[5] = new PolygonMesh(Repere(s6_pos), &glass, vert1, 24);

	  Image screenBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 3);
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

  glfwTerminate();
	
  return 0;

} catch(const char *exception) {
    fprintf(stderr,"%s\n",exception);
  }
  return 1;
}

