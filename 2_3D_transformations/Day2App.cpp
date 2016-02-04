#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/CameraUi.h"

#include "cinder/gl/Shader.h"

#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Day2App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void saveGif();
    float angleRot;
    int mCurrentFrame;
};

void Day2App::setup()
{
    angleRot = 0;
    mCurrentFrame = 0;
}

void Day2App::mouseDown( MouseEvent event )
{
}

void Day2App::update()
{
//    cout << gl::getProjectionMatrix() << endl;
    angleRot += 0.0523;
    mCurrentFrame++;
}

void Day2App::draw()
{
    gl::clear();
    
    //TURN ON Z-BUFFERING
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    //CREATE A VIRTUAL CAMERA
    CameraPersp cam;
    
    //POSITION THE CAMERA AND SET THE LOOKAT POINT
    cam.lookAt( vec3(3, 4.5, 4.5), vec3(0,1,0) );
    
    //SET THE VIEW AND PROJECTION MATRICES BY PASSING THE CAMERA (OUR CameraPersp OBJECT) AS AN ARGUMENT
    gl::setMatrices(cam);
    
    //CREATE A BASIC LAMBERT SHADER AND BIND THE RESULT
    auto lambert = gl::ShaderDef().lambert().color();
    auto shader = gl::getStockShader(lambert);
    shader->bind();
    
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
    saveGif();
    
}

void Day2App::saveGif()
{
    if (mCurrentFrame % 4 == 0 && mCurrentFrame < 121) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "2"/ ("2-0" + toString( mCurrentFrame ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}

CINDER_APP( Day2App, RendererGl,
           [&]( App::Settings *settings ) {
               settings->setWindowSize( 600, 600 );
               settings->setHighDensityDisplayEnabled();
           })
