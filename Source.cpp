#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include<gl\glm\glm.hpp>
using namespace std;
using namespace glm;


enum DrawingMode {
    Points,
    Lines,
    FilledTriangle
};

struct Vertex {
    vec3 Position;
    vec3 Color;
};
enum Shape {
    None,
    Circle,
    Sierpinski,
    SinWave,
    LineDrawing
};

const GLint WIDTH = 600, HEIGHT = 600;
GLuint VBO, BasiceprogramId;
DrawingMode Current_DrawingMode = DrawingMode::Lines;
vector<GLfloat> mousePositions;
bool isDrawing = false;        
int depth = 3;                 
Shape currentShape = Shape::None;
GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name);

void CompileShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name, GLuint& programId) {
    programId = InitShader(vertex_shader_file_name, fragment_shader_file_name);
    glUseProgram(programId);
}

void CreateSinWave() {
    const int numVertices = 90;
    GLfloat sinwave[numVertices * 3];

    for (int i = 0; i < numVertices; ++i) {
        float x = i * 0.01f;
        float y = sin(x * 20);

        sinwave[i * 3] = x;
        sinwave[i * 3 + 1] = y;
        sinwave[i * 3 + 2] = 0.0f;
    }

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sinwave), sinwave, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
}
void CircleColored()
{
    GLfloat TriangleVertices[180] =
    {
    };
    TriangleVertices[0] = 0;
    TriangleVertices[1] = 0;
    TriangleVertices[3] = 1;
    TriangleVertices[4] = 0.5;
    TriangleVertices[5] = 0.5;
    for (int i = 6; i < 174; i += 6) {
        TriangleVertices[i] = cos((i * 2 * 3.14) / 162);
        TriangleVertices[i + 1] = sin((i * 2 * 3.14) / 162);
        TriangleVertices[i + 3] = cos((i * 2 * 3.14) / 162);
        TriangleVertices[i + 4] = sin((i * 2 * 3.14) / 162);
        TriangleVertices[i + 5] = sin((i * 2 * 3.14) / 162);
    }
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleVertices), TriangleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(GL_FLOAT), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(GL_FLOAT), (char*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);
}

void CreateSierpinski(int depth, GLfloat* vertices, int& index, vec2 p1, vec2 p2, vec2 p3) {
    if (depth == 0) {
        vertices[index++] = p1.x;
        vertices[index++] = p1.y;
        vertices[index++] = 0.0f;

        vertices[index++] = p2.x;
        vertices[index++] = p2.y;
        vertices[index++] = 0.0f;

        vertices[index++] = p3.x;
        vertices[index++] = p3.y;
        vertices[index++] = 0.0f;
    }
    else {
        vec2 mid1 = (p1 + p2) * 0.5f;
        vec2 mid2 = (p2 + p3) * 0.5f;
        vec2 mid3 = (p3 + p1) * 0.5f;

        CreateSierpinski(depth - 1, vertices, index, p1, mid1, mid3);
        CreateSierpinski(depth - 1, vertices, index, mid1, p2, mid2);
        CreateSierpinski(depth - 1, vertices, index, mid3, mid2, p3);
    }
}

void GenerateSierpinski(int depth) {
    const int maxVertices = 3 * 27;
    GLfloat* sierpinskiVertices = new GLfloat[maxVertices * 3];
    int index = 0;

    vec2 p1(-0.9f, -0.9f);
    vec2 p2(0.9f, -0.9f);
    vec2 p3(0.0f, 0.9f);

    CreateSierpinski(depth, sierpinskiVertices, index, p1, p2, p3);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, index * sizeof(GLfloat), sierpinskiVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    delete[] sierpinskiVertices;
}


int Init() {
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        cout << "Error initializing GLEW\n";
        getchar();
        return 1;
    }

    cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;

    CompileShader("VS.glsl", "FS.glsl", BasiceprogramId);
    
    glClearColor(0, 0.5, 0.5, 1);
    return 0;
}



void UpdateVBO() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mousePositions.size() * sizeof(GLfloat), mousePositions.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
}



void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    switch (Current_DrawingMode)
    {
    case Points:
        glPointSize(10);
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    case Lines:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case FilledTriangle:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    default:
        break;
    }
    switch (currentShape) {
    case Shape::Circle:
        glDrawArrays(GL_TRIANGLE_FAN, 0, 30);  
        break;

    case Shape::Sierpinski:
        glDrawArrays(GL_TRIANGLES, 0, 27 * 3);
        break;

    case Shape::SinWave:
        glDrawArrays(GL_LINE_STRIP, 0, 90); 
        break;

    case Shape::LineDrawing:
        if (!mousePositions.empty()) {
            glDrawArrays(GL_LINE_STRIP, 0, mousePositions.size() / 3);
        }
        break;

    case Shape::None:
    default:
        break;
    }
    
}
static float theta = 0.0f;
void Update() {
    
    theta += 0.0001f;

}

int main() {
    sf::ContextSettings context;
    context.depthBits = 24;
    sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "OpenGL Combined Example", sf::Style::Close, context);

    if (Init()) return 1;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    isDrawing = true;
                }
                break;
            case sf::Event::MouseButtonReleased:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    isDrawing = false;
                }
                break;
            case sf::Event::MouseMoved:
                if (isDrawing && currentShape == Shape::LineDrawing) {
                    float x = (event.mouseMove.x / float(WIDTH)) * 2.0f - 1.0f;
                    float y = -((event.mouseMove.y / float(HEIGHT)) * 2.0f - 1.0f);
                    mousePositions.push_back(x);
                    mousePositions.push_back(y);
                    mousePositions.push_back(0.0f);
                    UpdateVBO();

                }
                break;
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Num1) {
                    Current_DrawingMode = DrawingMode::Points;
                }
                if (event.key.code == sf::Keyboard::Num2) {
                    Current_DrawingMode = DrawingMode::Lines;
                }
                if (event.key.code == sf::Keyboard::Num3) {
                    Current_DrawingMode = DrawingMode::FilledTriangle;
                }
                if (event.key.code == sf::Keyboard::Num4) {
                    currentShape = Shape::Circle; 
                    CircleColored();
                }
                else if (event.key.code == sf::Keyboard::Num5) {
                    currentShape = Shape::Sierpinski;  
                    GenerateSierpinski(depth);
                }
                else if (event.key.code == sf::Keyboard::Num6) {
                    currentShape = Shape::SinWave;
                    CreateSinWave();
                }
                else if (event.key.code == sf::Keyboard::Num7) {
                    currentShape = Shape::LineDrawing;    
                }
                
                
                
                break;
            }
        }


        Update();
        Render();
        window.display();
    }

    glDeleteBuffers(1, &VBO);
    glDeleteProgram(BasiceprogramId);

    return 0;
}
