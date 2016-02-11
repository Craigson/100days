#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Easing.h"
#include "cinder/Utilities.h"


using namespace ci;
using namespace ci::app;
using namespace std;

const int NUM_ROWS = 150;
const int NUM_COLS = 150;
const int GRID_SPACE = 2 ;

class Day10aApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void saveGif();
    
    CameraPersp mCam;
    
    gl::BatchRef mCubes[NUM_ROWS][NUM_COLS];
    
    gl::GlslProgRef mGlsl;
    
    const float delay = 0.0f;
    
    //TIME IN SECONDS FOR ONE SLICE TO ROTATE
    const float rotationTime = 2.0f;
    
    //TIME IN SECONDS TO DELAY EACH SLICES ROTATION
    const float rotationOffset = 0.15f;
    
    bool complete = false;
    
    int l, n;
    
    float angle = 0;
    
    float distanceA = 200.0f;
    
};

void Day10aApp::setup()
{
    auto lambert = gl::ShaderDef().lambert().color();
    
    auto shader = gl::getStockShader(lambert);
    
    auto scale = geom::Scale(1.5,1.5,1.5);
    
    auto rotate = geom::Rotate(angleAxis(toRadians(90.f), vec3(1,0,0)));
    
    auto color = geom::Constant(geom::COLOR, Color(CM_HSV, 0, 0, 1 ));
    
    setFrameRate(60);
    
    for (int i = 0; i < NUM_COLS; i++)
    {
        for (int j = 0; j < NUM_ROWS; j++)
        {
            mCubes[i][j] = gl::Batch::create(geom::Sphere() >> scale >> rotate >> color, shader);
        }
    }
    
    mCam.lookAt(vec3(0,0,150.), vec3(0));
    
    mCam.setPerspective( 35.0f, 1.0f, 1.f, 1000.0f );
}

void Day10aApp::mouseDown( MouseEvent event )
{
}

void Day10aApp::update()
{
    angle += 0.0523;
   // distance += 0.523;
}

void Day10aApp::draw()
{
    
    
    float t = getElapsedSeconds() /2 ;
    gl::clear();
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    gl::setMatrices(mCam);
    
    //gl::rotate(angle, vec3(0,1,0));
    gl::pushModelMatrix();
    
    //gl::translate(vec3(0,0,-30));
    
    cout << fabs(sin(angle)) << endl;
    
    
    gl::rotate(-80, vec3(1,0,0));
    //gl::translate(-1 * NUM_COLS*GRID_SPACE/4.4, -1 * NUM_ROWS*GRID_SPACE/4.4, 0);
    //gl::rotate(-(fabs(sin(angle))), vec3(1,0,0) );
    
    
    
    //gl::rotate(angleAxis(angle, vec3(0,1,0)));
    
    for (int i = 0; i < NUM_COLS; i++)
    {
        
        
        for (int j = 0; j < NUM_ROWS; j++)
        {

            
            float x1 = (GRID_SPACE * i + GRID_SPACE/2 - GRID_SPACE*NUM_ROWS/2);
            float y1 = (GRID_SPACE * j + GRID_SPACE/2 - GRID_SPACE*NUM_ROWS/2);
            
            gl::ScopedModelMatrix scpMtx;
            
            float dist = distance(vec3(x1,y1,0), vec3(0,0,0) );
            
            float z = pow(2.7182817, -dist/50) * sin( (dist/10)-t)*15;
            
           // float angleA = easeInOutQuint(z);
            
            //float red = lmap(sin(angle),-1.f,1.f,0.f,255.f);
            
            gl::translate(x1,y1,z);
            
          //  gl::rotate(angleAxis(angle, vec3(0,1,0)));
            
            gl::color(255,0,0);
            
            mCubes[i][j]->draw();

            
        }
    }
    
    gl::popModelMatrix();
    
    //saveGif();
    
}


void Day10aApp::saveGif()
{
    if (getElapsedFrames() % 4 == 0 && getElapsedFrames() < 121) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "12"/ ("12-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}

CINDER_APP( Day10aApp, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(500,500);
           })