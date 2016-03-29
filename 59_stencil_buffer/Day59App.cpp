#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"
#include "cinder/Perlin.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define NUM_OBJS 500

class Day59App : public App {
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
    void saveGif();
    
    void setupGlsl();
    
    gl::GlslProgRef     mGlsl;
    gl::GlslProgRef     mOutline;
    CameraPersp         mCam;
    
    gl::BatchRef        mObject;
    gl::BatchRef        mBorderObject;
    
    float theta = 0;
    float omega = 0;
    float scale;
    
    std::vector<vec3> mRandomPositions, mScales;
    std::vector<float> mOffsets;
    
    Perlin      mPerlin;

};

void Day59App::setup()
{
    setupGlsl();
    
    
    
    mCam.lookAt(vec3(0,0,100), vec3(0,0,0));
    mCam.setPerspective(45.f, getWindowAspectRatio(), 1.f, 1000.f);
    
    auto cube = geom::Icosahedron();
    mObject = gl::Batch::create(cube, mGlsl);
    mBorderObject = gl::Batch::create(cube, mOutline);
    
    for (int i = 0; i < NUM_OBJS; i++)
    {
        vec3 rando = vec3(randFloat(-50., 50), randFloat(-50., 50), randFloat(-50., 50));
        mRandomPositions.push_back(rando);
        
        mScales.push_back(vec3(randFloat(0.5,3)));
        
        mOffsets.push_back(randFloat(0, 1000));
    }
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
}

void Day59App::mouseDown( MouseEvent event )
{
}

void Day59App::update()
{
    theta += 0.0523;
    omega += 0.0523/2;
    
    scale = 1.3 * cos(omega);
}

void Day59App::draw()
{
    gl::setMatrices(mCam);
    gl::clear( Color( 0.1, 0.1, 0.1 ) );
    gl::color(Color(0.,0.,0.));
    gl::clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    mCam.setEyePoint(vec3(0.,0.,fabs(cos(omega) * 50.f)));
  mPerlin.setSeed(getElapsedFrames());
    
    // enable stencil test to be able to give the stencil buffers values.
    gl::ScopedState scopeStencil( GL_STENCIL_TEST, true );
  //  gl::rotate(angleAxis(theta/2, vec3(0.,1.,0.)));
    for (int i =0; i <NUM_OBJS; i++)
    {
        
        vec3 noise = mPerlin.dfBm(mRandomPositions[i]);
    
        gl::ScopedMatrices scpMtx;
        gl::translate(mRandomPositions[i]);
        
    
        {
            gl::ScopedMatrices push;
            gl::rotate(angleAxis(theta + mOffsets[i], vec3(1.0, 0., 0.)));
            gl::translate(mRandomPositions[i]);
            gl::scale(mScales[i]);
            
            //DRAW THE MAIN OBJECT (1ST RENDER PASS) AS NORMAL, FILL THE STENCIL BUFFER
            gl::stencilFunc(GL_ALWAYS, 1, 0xFF); //ALL FRAGMENTS MUST UPDATE THE STENCIL BUFFER
            gl::stencilMask(0xFF); //ENABLE WRITING TO THE STENCIL BUFFER
            mObject->draw();
        }
        
        //IF THE STENCIL TEST FAILS, KEEP THE FRAGMENT
        //IF STENCIL PASSES AND DEPTH FAILS, KEEP THE FRAGMENT
        //IF THE DEPTH TEST AND STENCIL TEST PASS, REPLACE THE FRAGMENT
        glStencilOp(GL_KEEP  , GL_KEEP, GL_REPLACE);
        
        //2ND RENDER PASS - DRAW OBJECT SLIGHTLY BIGGER, BUT DISABLE STENCIL WRITING.  BECAUSE THE STENCIL BUFFER IS FILLED WITH 1s (PARTICULARLY IN THE AREA OF THE ORIGINAL OBJECT), THE ONLY THING THAT WILL BE DRAWN IS THE AREA OF THE 2ND OBJECT, IE. THE SIZE DIFFERENCE
        
        gl::stencilFunc(GL_NOTEQUAL, 1, 0xFF); //don't update the stencil buffer
        gl::stencilMask(0x00);
        gl::disable(GL_DEPTH_TEST);
        {
            gl::ScopedMatrices push;
            gl::rotate(angleAxis(theta + mOffsets[i], vec3(1.0, 0., 0.)));
            gl::translate(mRandomPositions[i]);
            
            gl::scale(vec3(mScales[i] + vec3(0.1) ));
            
            mBorderObject->draw();
        }
    
    }
    
    gl::stencilMask(0xFF);
    gl::enable(GL_DEPTH_TEST);
  //  saveGif();
    
}

void Day59App::setupGlsl()
{
    mGlsl = gl::GlslProg::create(loadAsset("vertex.vert"), loadAsset("fragment.frag"));
    mOutline = gl::GlslProg::create(loadAsset("vertex.vert"), loadAsset("border.frag"));
}

void Day59App::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 241) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "59"/ ("59-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
}

CINDER_APP( Day59App, RendererGl(RendererGl::Options().msaa(16).stencil()),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(500, 500);
           })
