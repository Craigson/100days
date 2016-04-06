#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define FBO_DIMENSIONS 2048
#define NUM_OBJECTS 8

class Day67App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
    void keyDown (KeyEvent event) override;
	void update() override;
	void draw() override;
    
    void saveGif();
    
    gl::GlslProgRef     mGlsl;
    
    void updateFbo();
    
    gl::BatchRef        mBatch, mIcos;
    gl::FboRef          mFbo;
    
    float angleRot;
    
    //CREATE A VIRTUAL CAMERA
    CameraPersp mCam;
    
    std::vector<std::pair<vec3, float>>   mPositions;
    
    bool process;
    
};

void Day67App::setup()
{
    mGlsl = gl::GlslProg::create(loadAsset("vert.vert"), loadAsset("fragment.frag"));
    
    //POSITION THE CAMERA AND SET THE LOOKAT POINT
    mCam.lookAt( vec3(0, 1 ,0), vec3(0) );
    
    auto plane = geom::Plane();
    mBatch = gl::Batch::create(plane >> geom::Rotate(angleAxis(toRadians(180.f), vec3(0.,1.,0.))), mGlsl);
    
    auto lambert = gl::ShaderDef().lambert().color();
    auto shader = gl::getStockShader(lambert);
    
    mIcos = gl::Batch::create(geom::Icosahedron() >> geom::Scale(0.6,0.6,0.6), shader);
    
    //SETUP THE FBO
    gl::Fbo::Format format;
    format.setSamples( 16 ); // SET ANTIALIASING
    mFbo = gl::Fbo::create(FBO_DIMENSIONS, FBO_DIMENSIONS, format.depthTexture());
    
    gl::enableDepth();
    
    angleRot = 0;
    
    for (int i = 0; i < NUM_OBJECTS; i++)
    {
        mPositions.push_back(make_pair(vec3(randFloat(-1.f,1.f), randFloat(-1.f,1.f), randFloat(-1.f,1.f) ), randFloat(-2.,2.)));
    }
    
    process = false;
}

void Day67App::keyDown( KeyEvent event )
{
    if (event.getChar() == 'p') process = !process;
}


void Day67App::updateFbo()
{
    //THIS METHOD RENDERS WHATEVER WE'RE DRAWING INTO THE FBO
    
    gl::ScopedFramebuffer fbScp( mFbo ); //CREATING A SCOPED BUFFER MEANS THAT THE DEFAULT FRAMEBUFFER BINDING WILL BE RESTORED WHEN WE LEAVE THE SCOPE OF THIS FUNCTION
    
    gl::clear(Color(0.05f, 0.05f, 0.05f));
    
    //SET THE VIEWPORT TO MATCH THE DIMENSIONS OF THE FBO - 1ST ARGUMENT IS THE POSITION )
    gl::ScopedViewport scpVp ( ivec2(0), mFbo->getSize());
    
    //SET UP A TEMPORARY CAMERA - 3RD ARGUMENT IS FOV
    CameraPersp     tempCam(mFbo->getWidth(), mFbo->getHeight(), 60.f);
    tempCam.setPerspective(60.f, mFbo->getAspectRatio(), 1.f, 1000.f);
    tempCam.lookAt(vec3(0,0,10), vec3(0,0,-10));
    gl::setMatrices(tempCam);
    
    gl::rotate(angleAxis(angleRot, normalize(vec3(0.,4.5,1.7))));
    
    gl::color(ColorAf(1.f,1.f,1.f,1.f));
/*
    for (int i = 0; i < NUM_OBJECTS; i++)
    {
        gl::ScopedMatrices push;
        gl::translate(mPositions[i].first);
        //mPositions[i].first.y += 0.04 * cos(angleRot);
        gl::rotate(mPositions[i].second *angleRot/2, vec3(i / NUM_OBJECTS, 1., 0.3));
        mIcos->draw();
    }
 */
    
    gl::ScopedMatrices push;
    gl::rotate(angleAxis(angleRot, normalize(vec3(1.3,3.5,1.9))));
    mIcos->draw();

    
}


void Day67App::mouseDown( MouseEvent event )
{
}

void Day67App::update()
{
    if (getElapsedFrames() < 60) process = false;
    else process = true;
    
    mGlsl->uniform("process", process);
    angleRot += 0.0523;
    updateFbo();
    
}

void Day67App::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    
    gl::setMatrices(mCam);
    
    mFbo->bindTexture();
    
    //DRAW THE PLANE
    {
        mBatch->draw();
    }
    
    saveGif();
}

void Day67App::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 121) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "67"/ ("67-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}


CINDER_APP( Day67App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setWindowSize(500,500);
               settings->setHighDensityDisplayEnabled();
           })

