#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define FBO_DIMENSIONS 512

class Day65App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void saveGif();
    
    gl::GlslProgRef     mGlsl;
    
    void updateFbo();
    
    gl::BatchRef        mBatch;
    gl::FboRef          mFbo;
    
    float angleRot;
    
    //CREATE A VIRTUAL CAMERA
    CameraPersp mCam;
};

void Day65App::setup()
{
    mGlsl = gl::GlslProg::create(loadAsset("vert.vert"), loadAsset("fragment.frag"));
    
    //POSITION THE CAMERA AND SET THE LOOKAT POINT
    mCam.lookAt( vec3(0, 2, 0), vec3(0) );
    
    auto plane = geom::Plane();
    mBatch = gl::Batch::create(plane >> geom::Rotate(angleAxis(toRadians(180.f), vec3(0.,1.,0.))), mGlsl);
    
    //SETUP THE FBO
    gl::Fbo::Format format;
    format.setSamples( 16 ); // SET ANTIALIASING
    mFbo = gl::Fbo::create(FBO_DIMENSIONS, FBO_DIMENSIONS, format.depthTexture());
    
    gl::enableDepth();
    
      angleRot = 0;
}

void Day65App::mouseDown( MouseEvent event )
{
}

void Day65App::update()
{
    updateFbo();
}

void Day65App::updateFbo()
{
    //THIS METHOD RENDERS WHATEVER WE'RE DRAWING INTO THE FBO
    
    gl::ScopedFramebuffer fbScp( mFbo ); //CREATING A SCOPED BUFFER MEANS THAT THE DEFAULT FRAMEBUFFER BINDING WILL BE RESTORED WHEN WE LEAVE THE SCOPE OF THIS FUNCTION
    
    gl::clear(Color(0.1f, 0.1f, 0.1f));
    
    //SET THE VIEWPORT TO MATCH THE DIMENSIONS OF THE FBO - 1ST ARGUMENT IS THE POSITION )
    gl::ScopedViewport scpVp ( ivec2(0), mFbo->getSize());
    
    //SET UP A TEMPORARY CAMERA - 3RD ARGUMENT IS FOV
    CameraPersp     tempCam(mFbo->getWidth(), mFbo->getHeight(), 60.f);
    tempCam.setPerspective(60.f, mFbo->getAspectRatio(), 1.f, 1000.f);
    tempCam.lookAt(vec3(3, 4.5, 4.5), vec3(0,1,0));
    gl::setMatrices(tempCam);
    //gl::color(ColorAf(1.,1.,1.,1.));
    
    //CREATE A BASIC LAMBERT SHADER AND BIND THE RESULT
    auto lambert = gl::ShaderDef().lambert().color();
    auto shader = gl::getStockShader(lambert);
    
    int numSpheres = 128;
    float maxAngle = M_PI * 7;
    float spiralRadius = 1;
    float height = 2;
    float anim = getElapsedFrames() / 30.0f;
    
    // gl::rotate( fabs(sin(angleRot)), vec3(0,1,0));
    
    for (int s = 0; s < numSpheres; ++s)
    {
        float rel = s/ (float)numSpheres;
        float angle = rel * maxAngle;
        float y = fabs( sin( rel * M_PI + anim ) ) * height;
        float r = rel * spiralRadius;
        vec3 offset(r * cos(angle) , y/2 , r * sin(angle) );
        
        gl::pushModelMatrix();
        gl::translate(offset);
        gl::scale( vec3 (0.3f, y/2, 0.3f));
        gl::color(Color(CM_HSV, rel, 1 , 1));
        // gl::drawCube(vec3(), vec3(1) );
        gl::drawSphere(vec3(), 1.0f, 30);
        gl::popModelMatrix();
        
    }
}

void Day65App::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 121) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "65"/ ("65-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}

void Day65App::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::setMatrices(mCam);
    
    mFbo->bindTexture();
    
    //DRAW THE PLANE
    {
        mBatch->draw();
    }
    
  //  saveGif();
}

CINDER_APP( Day65App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(500,500);
           })
