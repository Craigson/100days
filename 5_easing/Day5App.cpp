#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Utilities.h"

#include "cinder/Easing.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Day5App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void saveGif();
    
    static const int NUM_SLICES = 64;
    
    CameraPersp mCam;
    gl::BatchRef mSlices[NUM_SLICES];

};

void Day5App::setup()
{
    auto lambert = gl::ShaderDef().lambert().color();
    gl::GlslProgRef shader = gl::getStockShader(lambert);
    
    for (int i = 0; i < NUM_SLICES; i++)
    {
        float rel = i / (float)NUM_SLICES;
        float sliceWidth = 2.0f / NUM_SLICES;
        
        auto slice = geom::Cube().size(sliceWidth,1,1);
        auto trans = geom::Translate(rel,0,0);
        auto color = geom::Constant(geom::COLOR, Color(CM_HSV, 0, 0, 1 ));
        mSlices[i] = gl::Batch::create( slice >> trans >> color, shader);
    }
    
    mCam.lookAt( vec3(2,3,2), vec3(0, 0, 0));
    
    setFrameRate(30);
}

void Day5App::mouseDown( MouseEvent event )
{
}

void Day5App::update()
{
}

void Day5App::draw()
{
    gl::clear();
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    gl::setMatrices(mCam);
    
    gl::pushModelMatrix();
    
    gl::translate(-.5,0,0);
    
    const float delay = 0.25f;
    
    //TIME IN SECONDS FOR ONE SLICE TO ROTATE
    const float rotationTime = 1.5f;
    
    //TIME IN SECONDS TO DELAY EACH SLICES ROTATION
    const float rotationOffset = 0.01f;
    
    //TOTAL TIME FOR ENTIRE ANIMATION
    const float totalTime = (delay + rotationTime + NUM_SLICES * rotationOffset);
    
    //LOOP EVERY 'totalTime' SECONDS
    float time = fmod( getElapsedFrames() / 30.0f, totalTime);
    
    for (int i = 0; i < NUM_SLICES; i++)
    {
        //ANIMATES 0->1
        float rotation = 0;
        
        //WHEN DOES THE SLICE BEGIN ROTATING
        float startTime = i * rotationOffset;
        
        //WHEN DOES IT COMPLETE
        float endTime = startTime + rotationTime;
        
        //ARE WE IN THE MIDDLE OF OUR TIME SECTION?
        if (time > startTime && time < endTime) rotation = (time - startTime) / rotationTime;
        
        //EASE FUNCTION ON ROTATION, THEN CONVERT TO RADIANS
        float angle = easeInOutQuint(rotation) * M_PI/2;
        
        gl::ScopedModelMatrix scpModelMatrix;
        gl::rotate(angleAxis(angle, vec3(1,0,0)));
        mSlices[i]->draw();
    }
    
    gl::popModelMatrix();
    
    saveGif();
}

void Day5App::saveGif()
{
    if (getElapsedFrames() % 4 == 0 && getElapsedFrames() < 121) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "5"/ ("5-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}

CINDER_APP( Day5App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings -> setWindowSize(600,600);
               settings -> setHighDensityDisplayEnabled();
           })
