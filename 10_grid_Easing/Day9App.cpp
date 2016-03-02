#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Easing.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const int NUM_ROWS = 11;
const int NUM_COLS = 11;
const int GRID_SIZE = 3 ;

class Day9App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void saveGif();
    
    CameraPersp mCam;
    
    gl::BatchRef mCubes[NUM_ROWS][NUM_COLS];
    
    gl::GlslProgRef mGlsl;
    
    const float delay = 0.1f;
    
    //TIME IN SECONDS FOR ONE SLICE TO ROTATE
    const float rotationTime = 2.0f;
    
    //TIME IN SECONDS TO DELAY EACH SLICES ROTATION
    const float rotationOffset = 0.15f;
    
    bool complete = false;
    
    
    
};

void Day9App::setup()
{
    auto lambert = gl::ShaderDef().lambert().color();
    
    auto shader = gl::getStockShader(lambert);
    
    auto color = geom::Constant(geom::COLOR, Color(CM_HSV, 0, 0, 1 ));
    
    setFrameRate(30);
    
    for (int i = 0; i < NUM_COLS; i++)
    {
        for (int j = 0; j < NUM_ROWS; j++)
        {
            mCubes[i][j] = gl::Batch::create(geom::Cube() >> color, shader);
        }
    }
    
    mCam.lookAt(vec3(0,0,40.), vec3(0));
    
    mCam.setPerspective( 35.0f, 1.0f, 1.f, 1000.0f );
}

void Day9App::mouseDown( MouseEvent event )
{
}

void Day9App::update()
{
}

void Day9App::draw()
{
	gl::clear();
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    gl::setMatrices(mCam);
    
    gl::pushModelMatrix();
    
    gl::translate(-1 * NUM_COLS*GRID_SIZE/4.4, -1 * NUM_ROWS*GRID_SIZE/4.4, 0);
    
   
    //TOTAL TIME FOR ENTIRE ANIMATION
    const float totalTime = (delay + rotationTime + NUM_ROWS*NUM_COLS * rotationOffset);
    
    //LOOP EVERY 'totalTime' SECONDS
    float time = fmod( getElapsedFrames() / 30.0f, totalTime);
    
    //ANIMATES 0->1
    float rotation = 0;

    float offset = 0;
    
    for (int i = 0; i < NUM_COLS; i++)
    {

        //WHEN DOES THE SLICE BEGIN ROTATING
        float startTime = i * rotationOffset;
        
        for (int j = 0; j < NUM_ROWS; j++)
        {


            
            //WHEN DOES IT COMPLETE
            float endTime = startTime + rotationTime;
            
            //ARE WE IN THE MIDDLE OF OUR TIME SECTION?
            if (time > startTime && time < endTime) rotation = (time - startTime) / rotationTime;
        
            //EASE FUNCTION ON ROTATION, THEN CONVERT TO RADIANS
            float angle = easeInOutQuint(rotation + offset) * M_PI / 2;
            
                float x = ( -0.1f + i / 2.0f) * GRID_SIZE;
                float y = ( -0.1f + j / 2.0f) * GRID_SIZE;
                
                gl::ScopedModelMatrix scpMtx;
                
                gl::translate(x,y,0);
                
                gl::rotate(angleAxis(angle, vec3(1,0,0)));
                
                gl::color(200,200,200);
                
                mCubes[i][j]->draw();
            
                offset += 0.005;
    
        }
    }
    
    gl::popModelMatrix();
    
    //saveGif();
}

void Day9App::saveGif()
{
    if (getElapsedFrames() % 4 == 0 && getElapsedFrames() < 121) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "9"/ ("9-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}

CINDER_APP( Day9App, RendererGl(RendererGl::Options().msaa(16)),
                                [&](App::Settings *settings){
                                    settings->setHighDensityDisplayEnabled();
                                    settings->setWindowSize(500,500);
                                })
