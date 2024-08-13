#include <iostream>
#include <stdexcept>

#define LEVEL_WIDTH 744  // 750
#define LEVEL_HEIGHT 504 // 500

const int WIDTH = 750;
const int HEIGHT = 500;

typedef struct GLAttributes {
	GLuint gProgramID;
	GLint gVertexPos2DLocation;
	GLuint gVBO; 
	GLuint gVAO;
} GLAttributes;

void debugProgramLog(GLuint program);
void debugShaderLog(GLuint shader);
bool initGL(GLAttributes *glAttributes);
void render(GLAttributes *glAttributes);

int main() {
	SDL_Window *gWindow = NULL;
	SDL_GLContext gContext = NULL;
	
	GLAttributes glAttributes;

	std::string debug_msg = "";

	// Initilize SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		debug_msg = "Failed to initiliaze SDL: " + static_cast<std::string>(SDL_GetError());
		std::runtime_error(debug_msg.c_str());
	}
	
	// Use OpenGL 3.1 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// Create window
	gWindow = SDL_CreateWindow("OpenGL SDL2 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	if (gWindow == NULL) {
		debug_msg = "OpenGL context could not be created! SDL_Error: " + static_cast<std::string>(SDL_GetError());
		std::cout << debug_msg << std::endl;
		std::runtime_error(debug_msg.c_str());
	}
	
	// Create context
	gContext = SDL_GL_CreateContext(gWindow);
	if (gContext == NULL) {
		debug_msg = "OpenGL context could not be created! SDL_Error: " + static_cast<std::string>(SDL_GetError());
		std::cout << debug_msg << std::endl;
		std::runtime_error(debug_msg.c_str());
	}

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();

	if (glewError != GLEW_OK) {
		const char* glew_error_msg_char_p = reinterpret_cast<const char*>(glewGetErrorString(glewError));
		std::string glew_error_msg = static_cast<std::string>(glew_error_msg_char_p);

		debug_msg = "Error initializing GLEW! " + glew_error_msg;
		std::cout << debug_msg << std::endl;
		std::runtime_error(debug_msg.c_str());
	}

	// Use Vsync
	if (SDL_GL_SetSwapInterval(1) < 0) {
		debug_msg = "Warning: Unable to set VSync! SDL Error: " + static_cast<std::string>(SDL_GetError());
		std::cout << debug_msg << std::endl;
		std::runtime_error(debug_msg.c_str());
	}


	// Initialize OpenGL
	if (!initGL(&glAttributes)) {
		debug_msg = "Unable to initialize OpenGL!";
		std::cout << debug_msg << std::endl;
		std::runtime_error(debug_msg.c_str());
	}


	bool quit = false;

	while(!quit) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYDOWN:
					if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
						quit = true;
					}
					break;
			}
		}

		// Render the scene
		render(&glAttributes);
		
		// Swap the back-buffer and present it
		SDL_GL_SwapWindow(gWindow);
	}

	// Free Resources
	SDL_GL_DeleteContext(gContext);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();

    return 0;
}

bool initGL(GLAttributes *glAttributes) {
	// Graphics program
	GLuint gProgramID = 0;
	GLint gVertexPos2DLocation = -1;
	GLuint gVBO = 0;
	GLuint gVAO = 0;

	std::string debug_msg = "";

	// Create vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// Get vertex source
	const GLchar* vertexShaderSource[] = {
		"#version 140\nin vec3 aPos; void main() { gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); }"
	};

	// Set vertex source
	glShaderSource(vertexShader, 1, vertexShaderSource, NULL);

	// Compile vertex source
	glCompileShader(vertexShader);

	// Check vertex shader for errors
	GLint vShaderCompiled = GL_FALSE;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);

	if (vShaderCompiled != GL_TRUE) {
		debug_msg = "Unable to compile vertex shader " + std::to_string(vertexShader) + "!";
		std::cout << debug_msg << std::endl;
		debugShaderLog(vertexShader);
		return false;
	}

	// Create fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Get fragment source
	const GLchar* fragmentShaderSource[] = {
		"#version 140\nout vec4 LFragment; void main() { LFragment = vec4(1.0, 1.0, 1.0, 1.0); }"
	};

	// Set fragment source
	glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);

	// Compile fragment source
	glCompileShader(fragmentShader);

	// Check fragment shader for errors
	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
	if (fShaderCompiled != GL_TRUE) {
		debug_msg = "Unable to compile fragment shader " + std::to_string(fragmentShader) + "!";
		std::cout << debug_msg << std::endl;
		debugShaderLog(fragmentShader);
		return false;
	}

	/* Link program */
	// Generate program
	gProgramID = glCreateProgram();

	// Attach vertex shader to program
	glAttachShader(gProgramID, vertexShader);
	
	// Attach fragment shader to program
	glAttachShader(gProgramID, fragmentShader);
	
	glLinkProgram(gProgramID);

	// Check for errors
	GLint programSuccess = GL_TRUE;
	glGetProgramiv(gProgramID, GL_LINK_STATUS, &programSuccess);
	if (programSuccess != GL_TRUE) {
		debug_msg = "Error linking program " + std::to_string(gProgramID) + "!";
		std::cout << debug_msg << std::endl;
		debugProgramLog(gProgramID);
		return false;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Initialize clear color
	glClearColor(0.f, 0.f, 0.f, 1.f);

	// VBO data
	GLfloat vertexData[] = {
		// positions		// colors
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left vertex
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right vertex
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f // top vertex
	};

	// Here
	glGenVertexArrays(1, &gVAO);
	glGenBuffers(1, &gVBO);
	glBindVertexArray(gVAO);

	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glAttributes->gVAO = gVAO;
	glAttributes->gVBO = gVBO;
	glAttributes->gProgramID = gProgramID;
	glAttributes->gVertexPos2DLocation = gVertexPos2DLocation;

	glUseProgram(glAttributes->gProgramID);

	return true;
}

void debugProgramLog(GLuint program) {
	std::string debug_msg = "";
	// make sure name is shader
	if (glIsProgram(program)) {
		// Program log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		// Get info string length
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// Allocate string
		char* infoLog = new char[maxLength];

		// Get info log
		glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
		if (infoLogLength > 0) {
			// Print Log
			std::cout << infoLog << std::endl;
		}

		// Deallcate string
		delete[] infoLog;
	}
	else {
		debug_msg = "Name " + std::to_string(program) + " is not a prgram";
		std::cout << debug_msg << std::endl;
	}
}

void debugShaderLog(GLuint shader) {
	std::string debug_msg = "";
	// make sure name is shader
	if (glIsShader(shader)) {
		// Program log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		// Get info string length
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// Allocate string
		char* infoLog = new char[maxLength];

		// Get info log
		glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
		if (infoLogLength > 0) {
			// Print Log
			std::cout << infoLog << std::endl;
		}

		// Deallcate string
		delete[] infoLog;
	}
	else {
		debug_msg = "Name " + std::to_string(shader) + " is not a shader";
		std::cout << debug_msg << std::endl;
	}
}

GLuint gProgramID = 0;
	GLint gVertexPos2DLocation = -1;
	GLuint gVBO = 0;
	GLuint gIBO = 0;

void render(GLAttributes *glAttributes) {
	// Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);
	
	// Draw triangle
	glUseProgram(glAttributes->gProgramID);
	glBindVertexArray(glAttributes->gVAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
