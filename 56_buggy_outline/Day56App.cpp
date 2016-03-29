#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Day56App : public App {
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
    
    void setupGlsl();
    void saveGif();
    
    gl::GlslProgRef     mGlsl;
    gl::GlslProgRef     mOutline;
    CameraPersp         mCam;
    
    gl::BatchRef        mObject;
    gl::BatchRef        mBorderObject;
    
    float theta = 0;
    float scale;
};

void Day56App::setup()
{
    setupGlsl();
    
    mCam.lookAt(vec3(0,0,5), vec3(0));
    mCam.setPerspective(45.f, getWindowAspectRatio(), 1.f, 1000.f);
    
    auto cube = geom::Icosahedron();
    
    mObject = gl::Batch::create(cube, mGlsl);
    mBorderObject = gl::Batch::create(cube, mOutline);
    
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
}

void Day56App::mouseDown( MouseEvent event )
{
}

void Day56App::update()
{
    theta += 0.0523;
    scale = 1.3 * cos(theta/2);
}

void Day56App::draw()
{
    gl::setMatrices(mCam);
    gl::clear( Color( 0.3, 0.3, 0.3 ) );
    gl::color(Color(0.,0.,0.));
    gl::clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    // enable stencil test to be able to give the stencil buffers values.
    gl::ScopedState scopeStencil( GL_STENCIL_TEST, true );
    
    {
        gl::ScopedMatrices push;
        gl::rotate(angleAxis(theta, vec3(1.0, 0., 0.)));
    
        //DRAW THE MAIN OBJECT (1ST RENDER PASS) AS NORMAL, FILL THE STENCIL BUFFER
        gl::stencilFunc(GL_ALWAYS, 1, 0xFF); //ALL FRAGMENTS MUST UPDATE THE STENCIL BUFFER
        gl::stencilMask(0xFF); //ENABLE WRITING TO THE STENCIL BUFFER
        mObject->draw();
    }

    //IF THE STENCIL TEST FAILS, KEEP THE FRAGMENT
    //IF STENCIL PASSES AND DEPTH FAILS, KEEP THE FRAGMENT
    //IF THE DEPTH TEST AND STENCIL TEST PASS, REPLACE THE FRAGMENT
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    
    //2ND RENDER PASS - DRAW OBJECT SLIGHTLY BIGGER, BUT DISABLE STENCIL WRITING.  BECAUSE THE STENCIL BUFFER IS FILLED WITH 1s (PARTICULARLY IN THE AREA OF THE ORIGINAL OBJECT), THE ONLY THING THAT WILL BE DRAWN IS THE AREA OF THE 2ND OBJECT, IE. THE SIZE DIFFERENCE
    
    gl::stencilFunc(GL_NOTEQUAL, 1, 0xFF); //don't update the stencil buffer
    gl::stencilMask(0x00);
    gl::disable(GL_DEPTH_TEST);
    {
        gl::ScopedMatrices push;
    
        gl::rotate(angleAxis(theta, vec3(1.0, 0., 0.)));
        gl::scale(vec3(1.f * scale));
        mBorderObject->draw();
    }

    
    gl::stencilMask(0xFF);
    gl::enable(GL_DEPTH_TEST);
 
   // saveGif();
}

void Day56App::setupGlsl()
{
    mGlsl = gl::GlslProg::create(loadAsset("vertex.vert"), loadAsset("fragment.frag"));
    mOutline = gl::GlslProg::create(loadAsset("vertex.vert"), loadAsset("border.frag"));
}

void Day56App::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 241) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "56a"/ ("56a-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
}

CINDER_APP( Day56App, RendererGl(RendererGl::Options().msaa(16).stencil()),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(500, 500);
           })
