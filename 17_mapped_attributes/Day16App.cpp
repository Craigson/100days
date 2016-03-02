#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Perlin.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Day16App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void setupBuffer();
    
    void saveGif();
    
    CameraPersp     mCam;
    
    gl::VboRef      mVbo;
    gl::VaoRef      mVao;
    
    gl::GlslProgRef mGlsl;
    
    float angle = 0;
    
    GLfloat vertices[18] = {
        // Positions         // Colors
        0.5f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,   // Bottom Right
        -0.5f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,   // Bottom Left
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // Top
    };
};

void Day16App::setupBuffer()
{
    mVbo = gl::Vbo::create( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    
    mVao = gl::Vao::create();
    
    gl::ScopedVao scopeVao( mVao );
    
    gl::ScopedBuffer scopeBuffer( mVbo );
    
    // Position attribute
    gl::vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    gl::enableVertexAttribArray(0);
    // Color attribute
    gl::vertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3* sizeof(GLfloat)));
    gl::enableVertexAttribArray(1);

}
void Day16App::setup()
{
    mCam.lookAt(vec3(0,0,0), vec3(0));
    setFrameRate(30);
    mGlsl = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset("vs.vert")).fragment(loadAsset("fs.frag")));
    
    setupBuffer();
}

void Day16App::mouseDown( MouseEvent event )
{
}

void Day16App::update()
{
    angle += 0.0523*2;
    
    vertices[12] = sin(angle);
    vertices[13] = cos(angle);
    vertices[14] = sin(angle/2.);
    // Copy particle data onto the GPU.
    // Map the GPU memory and write over it.
    void *gpuMem = mVbo->mapReplace();
    memcpy( gpuMem, vertices, sizeof(vertices) );
    mVbo->unmap();
}

void Day16App::draw()
{
	//gl::clear( ColorA ( 0.2, 0.2, 0.2, 0.5) );
    
    gl::setMatrices(mCam);
    
    gl::ScopedGlslProg scpGlsl( mGlsl );
    
    gl::ScopedVao scpVao (mVao);
    
    gl::setDefaultShaderVars();
    
    
    mGlsl->uniform("uTime", (float)(getElapsedFrames()/20.));
    mGlsl->uniform("uOffset", angle);

    gl::drawArrays(GL_TRIANGLES, 0, 3);
    
    saveGif();
}

void Day16App::saveGif()
{
    if (getElapsedFrames() % 4 == 0 && getElapsedFrames() < 121) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "17"/ ("17-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}

CINDER_APP( Day16App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setWindowSize(500,500);
               settings->setHighDensityDisplayEnabled();
           })
