#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Day19App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void setupGlsl();
    
    CameraPersp mCam;
    
    GLfloat vertices[18] =  {
        // Positions         // Colors
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Bottom Right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // Bottom Left
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // Top
    };
    
    gl::VboRef          mVbo;
    gl::VaoRef          mVao;
    gl::GlslProgRef     mGlsl;
    
};

void Day19App::setup()
{
    GLint numAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttribs);
    cout << numAttribs << endl;
    
    mCam.lookAt(vec3(0,0,10), vec3(0));
    mCam.setPerspective( 35.0f, 1.0f, 1.f, 1000.0f );
    
    setupGlsl();
    
    mVao = gl::Vao::create();
    
    gl::ScopedVao scopeVao( mVao );
    
    mVbo = gl::Vbo::create(GL_ARRAY_BUFFER, sizeof(vertices), vertices);
    
    gl::ScopedBuffer scopeBuffer( mVbo );
    
    //args: referencing this vertex attribute's location / number of components (ie a vec3 for color will have 3) / type / should we normalize / stride (between each vertex) ie 6 values (3 for position, 3 for colour) / offset (where in the array does the information appear)
    
    //POSITION ATTRIBUTE
    gl::vertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),  (GLvoid*)0);
    gl::enableVertexAttribArray(0);
    
    //COLOUR ATTRIBUTE
    
    gl::vertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)( 3 * sizeof(GLfloat)));
    gl::enableVertexAttribArray(1);
}

void Day19App::mouseDown( MouseEvent event )
{
}

void Day19App::update()
{
}

void Day19App::draw()
{
	gl::clear();
    
    gl::setMatrices(mCam);
    
    gl::ScopedGlslProg scpGlsl( mGlsl );
    
    gl::ScopedVao scpVao (mVao);
    
    gl::drawArrays(GL_TRIANGLES, 0, 3);
    
    
}

void Day19App::setupGlsl()
{
    mGlsl = gl::GlslProg::create(gl::GlslProg::Format()
                                 .vertex(CI_GLSL(330,
                                                 
                                                 layout (location = 0) in vec3 position;
                                                 layout (location = 1) in vec3 color;
                                                 out vec3 ourColor;
                                                 
                                                 void main()
                                                 {
                                                     gl_Position = vec4(position, 1.0f);
                                                     ourColor = color;
                                                 }
                                                 
                                                 ))
                                 .fragment(CI_GLSL(330,
                                                   
                                                   in vec3 ourColor;
                                                   out vec4 color;
                                                   
                                                   void main()
                                                   {
                                                       color = vec4(ourColor, 1.0f);
                                                   }
                                                   
                                                   ))
                                 );
    
}


CINDER_APP( Day19App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setWindowSize(500,500);
               settings->setHighDensityDisplayEnabled();
           })
