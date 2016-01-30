#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Day1App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void saveGif();
    
    int mCurrentFrame = 0;
    
    float rotAngle;
};

void Day1App::setup()
{
    rotAngle = 0;
}

void Day1App::mouseDown( MouseEvent event )
{
}

void Day1App::update()
{
    rotAngle -= 0.0523;
    mCurrentFrame++;
}

void Day1App::draw()
{
    gl::clear();
    
    gl::enableAlphaBlending();
    
    gl::pushModelMatrix();
    
    gl::translate(getWindowCenter());
    
    gl::rotate(rotAngle);
    
    int numCircles = 256;
    
    float radius = getWindowHeight() / 2 ;
    
    for (int c = 0; c <numCircles; ++c)
    {
        float rel = c / (float) numCircles;
        
        float angle = rel * M_PI * 2;
        
        vec2 offset (cos(angle)/2, sin (angle)/2);
        
        gl::pushModelMatrix();
        
        gl::translate(offset * radius);
        
        gl::rotate(angle);
        
        
        gl::scale(lmap(c,0,numCircles,0,30), 0.25f);
        
        gl::color(Color(CM_HSV, rel, 1,1));
        
        gl::drawSolidCircle(vec2(), 5);
        
        gl::popModelMatrix();
    }
    
    gl::popModelMatrix();
    
    saveGif();
}

void Day1App::saveGif()
{
    if (mCurrentFrame % 4 == 0 && mCurrentFrame < 121) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "1"/ ("1-0" + toString( mCurrentFrame ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}

CINDER_APP( Day1App, RendererGl,
           [&]( App::Settings *settings ) {
               settings->setWindowSize( 600, 600 );
               settings->setHighDensityDisplayEnabled();
           })
