#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIO.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

float TWO_PI;

class Day0App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
    void keyDown (KeyEvent event) override;
	void update() override;
	void draw() override;
    void saveGif();
    
    int mCurrentFrame;
};

void Day0App::setup()
{
    mCurrentFrame = 0;
    TWO_PI = 0;
}

void Day0App::keyDown(KeyEvent event)
{
}

void Day0App::mouseDown( MouseEvent event )
{
}

void Day0App::update()
{
    mCurrentFrame++;
    
    TWO_PI += 0.0523;
}

void Day0App::draw()
{
    gl::clear();
    
    gl::enableAlphaBlending();
    
    // preserve the default Model matrix
    gl::pushModelMatrix();
    
    // move to the window center
    gl::translate( getWindowCenter() );
    
    int numCircles = 400;
    float radius = (getWindowHeight() / 2 ) - 100;
    
    for( int c = 0; c < numCircles; ++c ) {
        float rel = c / (float)numCircles;
        float angle = rel * M_PI * 2;
        vec2 offset( cos( angle ), sin( angle ) );
        
        // preserve the Model matrix
        gl::pushModelMatrix();
        
        // move to the correct position
        gl::translate( offset * radius );
        
        // set the color using HSV color
        gl::color( Color( CM_HSV, rel, 1, 1 ) );
        
        // draw a circle relative to Model matrix
        gl::drawStrokedCircle( vec2(), 50 * sin( TWO_PI)  );
        
        // restore the Model matrix
        gl::popModelMatrix();
    }

    
    // restore the default Model matrix
    gl::popModelMatrix();
    saveGif();
}

void Day0App::saveGif()
{
    if (mCurrentFrame % 4 == 0 && mCurrentFrame < 121) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "0"/ ("0-0" + toString( mCurrentFrame ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}



CINDER_APP( Day0App, RendererGl,
           [&]( App::Settings *settings ) {
               settings->setWindowSize( 600, 600 );
               settings->setHighDensityDisplayEnabled();
           })
