#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Day3App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void saveGif();
    
    //DECLARE A CAMERA OBJECT
    CameraPersp mCam;
    
    //DECLARE A BATCH OBJECT
    gl::BatchRef mBox;
    
    int mCurrentFrame = 0;
    float rotAngle = 0;
};

void Day3App::setup()
{
    //CREATE A DEFAULT LAMBERT SHADER AND CUBE GEOMETRY, INSTANTIATE THE BATCH USING THEM
    auto lambert = gl::ShaderDef().lambert().color();
    gl::GlslProgRef shader = gl::getStockShader(lambert);
    mBox = gl::Batch::create(geom::Cube(), shader);
    
    //POSITION THE CAMERA AND SET THE LOOKAT POINT
    mCam.lookAt(vec3(2, 2.7, 2), vec3(0,0,0));
    
    
}

void Day3App::mouseDown( MouseEvent event )
{
}

void Day3App::update()
{
    mCurrentFrame++;
    rotAngle += 0.0523;
}

void Day3App::draw()
{
    gl::clear();
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    gl::setMatrices(mCam);
    
    int numCubes = 8192;
    float maxAngle = M_PI * 128;
    float spiralRadius = 1;
    float height = 0.5;
    float boxSize = 0.02f;
    float anim = getElapsedFrames() / 30.0f;
    
    gl::pushModelMatrix();
    
    gl::rotate(rotAngle, vec3(0,1,0));
    
    for (int s = 0; s < numCubes; ++s)
    {
        float rel = s / (float)numCubes;
        float angle = rel * maxAngle;
        float y = fabs(cos(rel * M_PI + anim) ) * height;
        //float y = sin(rel * M_PI + anim) * height;
        float r = rel * spiralRadius;
        
        vec3 offset (r * cos(angle), y/4, r * sin(angle));
        
        float value = lmap(y, -1*height, height, 0.1f, 1.0f);
        
        gl::pushModelMatrix();
        gl::translate(offset);
        gl::scale(vec3(boxSize, y, boxSize));
        gl::color(Color(CM_HSV, rel, 1, value));
        //gl::color(ColorAf(rel,rel,rel,0.5));
        mBox->draw();
        gl::popModelMatrix();
        
    }
    gl::popModelMatrix();
    saveGif();
}

void Day3App::saveGif()
{
    if (mCurrentFrame % 4 == 0 && mCurrentFrame < 121) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "3"/ ("3-0" + toString( mCurrentFrame ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}

CINDER_APP( Day3App, RendererGl(RendererGl::Options().msaa( 16 ) ),
           [&]( App::Settings *settings ) {
    settings->setWindowSize( 600, 600 );
    settings->setHighDensityDisplayEnabled();
})
