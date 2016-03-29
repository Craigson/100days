#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/CameraUi.h"
#include "cinder/Utilities.h"

#define NUM_POLES 24

using namespace ci;
using namespace ci::app;
using namespace std;

class Day54App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void saveGif();
    
    gl::BatchRef        mBatch;
    gl::GlslProgRef     mGlsl;
    
    CameraPersp         mCam;
    CameraUi            mCamUi;
    
    float theta = 0;
    float offset = 0.;
};

void Day54App::setup()
{
    mGlsl = gl::GlslProg::create(loadAsset("vertex.vert"), loadAsset("fragment.frag"));
    mCam.lookAt(vec3(0.,0.,0.), vec3(0.,0.,-10));
    mCam.setPerspective(45., getWindowAspectRatio(), 1., 100.);
    mCamUi = CameraUi(&mCam, getWindow());
    
    auto pole = geom::Cylinder();
    
    mBatch = gl::Batch::create(pole >> geom::Scale(1.,100.,1.) >> geom::Rotate(angleAxis(toRadians(90.f), vec3(1.,0.,0.))) >> geom::Translate(0.,0.,-50), mGlsl);
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
}

void Day54App::mouseDown( MouseEvent event )
{
}

void Day54App::update()
{
    offset += 0.0523;
    theta = 0;
}

void Day54App::draw()
{
    gl::setMatrices(mCam);
	gl::clear( Color( 0.05, 0.05, 0.05 ) );
    
    gl::ScopedMatrices scpMtx;
    gl::translate(0.,0., 50*cos(offset));
    for (int i = 0; i < NUM_POLES ; i++)
    {
        gl::ScopedMatrices push;
        gl::translate(5 * cos(theta + offset/2), 5 * sin(theta + offset/2), 0.);
        mBatch->draw();
        theta += 15;
    }
    
    saveGif();
}


void Day54App::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 121) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "54"/ ("54-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
}

CINDER_APP( Day54App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(500, 500);
           })
