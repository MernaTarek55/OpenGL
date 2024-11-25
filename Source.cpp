#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include<gl\glm\glm.hpp>

using namespace std;
using namespace glm;
enum DrawingMode
{
    Points,
    Lines,
    FilledTriangle
};

GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name);

const GLint WIDTH = 600, HEIGHT = 600;
GLuint VBO, BasiceprogramId;
DrawingMode Current_DrawingMode = DrawingMode::Lines;

// Mouse drawing variables
vector<GLfloat> mousePositions; // Stores x, y, z for each point
bool isDrawing = false;         // Whether the mouse button is held

// Initialize OpenGL
int Init()
{
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        cout << "Error initializing GLEW\n";
        getchar();
        return 1;
    }

    cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;

    // Create initial buffer
    glGenBuffers(1, &VBO);

    // Compile shader program
    BasiceprogramId = InitShader("VS.glsl", "FS.glsl");
    glUseProgram(BasiceprogramId);

    // Set clear color
    glClearColor(0, 0.5, 0.5, 1);

    return 0;
}

// Update Vertex Buffer Object
void UpdateVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mousePositions.size() * sizeof(GLfloat), mousePositions.data(), GL_DYNAMIC_DRAW);

    // Set vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
}

// Render the points/lines
void Render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    switch (Current_DrawingMode)
    {
    case Points:
        glPointSize(5);
        glDrawArrays(GL_POINTS, 0, mousePositions.size() / 3);
        break;
    case Lines:
        glDrawArrays(GL_LINE_STRIP, 0, mousePositions.size() / 3);
        break;
    case FilledTriangle:
        // Not applicable for mouse drawing
        break;
    default:
        break;
    }
}

// Main function
int main()
{
    sf::ContextSettings context;
    context.depthBits = 24;
    sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "OpenGL Mouse Drawing", sf::Style::Close, context);

    if (Init())
        return 1;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    isDrawing = true;
                }
                break;

            case sf::Event::MouseButtonReleased:
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    isDrawing = false;
                }
                break;

            case sf::Event::MouseMoved:
                if (isDrawing)
                {
                    // Map screen coordinates to OpenGL coordinates
                    float x = (event.mouseMove.x / float(WIDTH)) * 2.0f - 1.0f;
                    float y = -((event.mouseMove.y / float(HEIGHT)) * 2.0f - 1.0f);

                    // Add the point to the buffer
                    mousePositions.push_back(x);
                    mousePositions.push_back(y);
                    mousePositions.push_back(0.0f);

                    // Update the VBO
                    UpdateVBO();
                }
                break;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Num1)
                {
                    Current_DrawingMode = DrawingMode::Points;
                }
                if (event.key.code == sf::Keyboard::Num2)
                {
                    Current_DrawingMode = DrawingMode::Lines;
                }
                break;
            }
        }

        Render();
        window.display();
    }

    // Cleanup
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(BasiceprogramId);

    return 0;
}