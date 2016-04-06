#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define FBO_DIMENSIONS 256

class Day63App : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;
    void saveGif();
    
    void updateFbo();
    
    gl::BatchRef        mBatch;
    gl::GlslProgRef     mGlsl;
    gl::FboRef          mFbo;
    
    mat4                mRotationMatrix;
    
    CameraPersp         mCam;
    CameraUi            mCamUi;
    
    float theta = 0;
    
};

void Day63App::setup()
{
    mCam.lookAt(vec3(0,0,10), vec3(0));
    mCam.setPerspective(45.f, getWindowAspectRatio(), 1.f, 1000.f);
    mCamUi = CameraUi(&mCam, getWindow());
    
    auto shader = gl::ShaderDef().lambert().color();
    auto t = gl::getStockShader(shader);
    //mGlsl = gl::GlslProg::create(t);
    
    mBatch = gl::Batch::create(geom::Cube(), t);
    
    //SETUP THE FBO
    gl::Fbo::Format format;
    format.setSamples( 4 ); // SET ANTIALIASING
    mFbo = gl::Fbo::create(FBO_DIMENSIONS, FBO_DIMENSIONS, format.depthTexture());
    
    gl::enableDepth();
    
}

void Day63App::update()
{
    theta += 0.0523;
    updateFbo();
}

void Day63App::updateFbo()
{
        //THIS METHOD RENDERS WHATEVER WE'RE DRAWING INTO THE FBO
    
    gl::ScopedFramebuffer fbScp( mFbo ); //CREATING A SCOPED BUFFER MEANS THAT THE DEFAULT FRAMEBUFFER BINDING WILL BE RESTORED WHEN WE LEAVE THE SCOPE OF THIS FUNCTION
    
    gl::clear(Color(0.1f, 0.1f, 0.1f));
    
    //SET THE VIEWPORT TO MATCH THE DIMENSIONS OF THE FBO - 1ST ARGUMENT IS THE POSITION )
    gl::ScopedViewport scpVp ( ivec2(0), mFbo->getSize());
    
    //SET UP A TEMPORARY CAMERA - 3RD ARGUMENT IS FOV
    CameraPersp     tempCam(mFbo->getWidth(), mFbo->getHeight(), 60.f);
    tempCam.setPerspective(60.f, mFbo->getAspectRatio(), 1.f, 1000.f);
    tempCam.lookAt(vec3(2.8f, 1.8f, -2.8f), vec3(0));
    gl::setMatrices(tempCam);
    gl::color(ColorAf(1.,1.,1.,1.));
    gl::rotate(angleAxis(theta, vec3(0.,1.,0)));
    mBatch->draw();
    
    
    
}

void Day63App::draw()
{
    gl::setMatrices(mCam);
	gl::clear( Color( 0.05, 0.05, 0.05 ) );
   
    gl::setMatrices(mCam);
    
    // use the scene we rendered into the FBO as a texture
    mFbo->bindTexture();
    
    // draw a cube textured with the FBO
    {
        gl::rotate(angleAxis(theta/2, normalize(vec3(0., 1., 0.))));
        gl::rotate(angleAxis(theta/4, normalize(vec3(1., 0., 0.))));
        gl::ScopedGlslProg shaderScp( gl::getStockShader( gl::ShaderDef().texture() ) );
        gl::drawCube( vec3( 0 ), vec3( 2.2f ) );
    }
    
    saveGif();
}


void Day63App::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 241) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "63"/ ("63-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
}

CINDER_APP( Day63App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(500, 500);
           })