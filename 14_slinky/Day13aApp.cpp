#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const int NUM_OBJECTS = 1536;

class Day13aApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void saveGif();
    
    float angle = 0;
    
    CameraPersp         mCam;
    gl::BatchRef        mBatch;
    gl::GlslProgRef     mGlsl;
    

};

void Day13aApp::setup()
{
    setFrameRate(30);
    
    mCam.lookAt(vec3(2,15,20), vec3(0,2,0));
    
    //CREATE THE SHADER BY ADDING THE VERTEX AND FRAGMENT SHADERS INLINE
    mGlsl = gl::GlslProg::create(gl::GlslProg::Format()
                                 
                                 .vertex ( CI_GLSL(150,
                                                   
                                                   uniform mat4    ciModelViewProjection;
                                                   in vec4         ciPosition;
                                                   in vec4         ciColor;
                                                   out vec4        Color;
                                                   
                                                   
                                                   
                                                   void main(void)
                                                   {
                                                       gl_Position = ciModelViewProjection * ciPosition;
                                                       Color = ciColor;
                                                   }))
                                 
                                 .fragment ( CI_GLSL(150,
                                                     
                                                     uniform vec4    uColor;
                                                     out vec4        oColor;
                                                     
                                                     void main(void)
                                                     {
                                                         oColor = uColor;
                                                     }
                                                     )));
    
    
    mBatch = gl::Batch::create(geom::Sphere(), mGlsl);
    
        mCam.setPerspective( 35.0f, 1.0f, 1.f, 1000.0f );
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
}

void Day13aApp::mouseDown( MouseEvent event )
{
}

void Day13aApp::update()
{
    angle += 0.0523;
}

void Day13aApp::draw()
{
	gl::clear( );
    
    gl::setMatrices(mCam);
    
    float maxAngle = M_PI * 20;
    float spiralRadius = 8;
    float height = 3;
    float anim = getElapsedFrames() / 30.0f;
    
    for (int i = 0; i < NUM_OBJECTS; i++)
    {
        float rel = i / (float)NUM_OBJECTS;
        float theta = rel * maxAngle;
        float y = fabs( sin (rel * M_PI + anim)) * height;
        float r = rel * spiralRadius;
        vec3 offset(r * cos(theta), y/2, r * sin(theta));
        
        gl::ScopedModelMatrix scpMtx;
        gl::translate(offset);
        gl::translate(vec3(0,y,0));
        
        gl::scale(0.2,0.2,0.2);
//        gl::color(Color(CM_HSV, rel, 1 , 1));
        
        ColorAf color(CM_HSV, 1.0, 0,1,0.7f);
        
        //PASS THE NEW VALUE FOR COLOR TO THE SHADER
        mGlsl->uniform("uColor", color);
    
        mBatch->draw();
        
    }
    
    saveGif();
}

void Day13aApp::saveGif()
{
    if (getElapsedFrames() % 4 == 0 && getElapsedFrames() < 241) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "13"/ ("13-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}


CINDER_APP( Day13aApp, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setWindowSize(500,500);
               settings->setHighDensityDisplayEnabled();
           });