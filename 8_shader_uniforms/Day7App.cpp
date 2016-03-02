#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Day7App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void saveGif();
    
    float angle;
    
    CameraPersp     mCam;
    gl::BatchRef    mCube;
    gl::BatchRef    mRect;
    gl::GlslProgRef mGlsl;
    
    gl::BatchRef    mIcos;
    
    geom::Rect      mSquare;
};

void Day7App::setup()
{
    angle = 0;
    
    setFrameRate(30);
    
    mCam.lookAt(vec3(2,2,4), vec3(0));
    
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

    //WE SET THE COLOURS FOR EACH VERTEX BY PASSING PARAMETERS WHEN WE CONSTRUCT THE GEOMETRY.
//    mSquare = geom::Rect().colors(    Color(1,0,0),
//                                        Color(0,0,fabs(angle)),
//                                        Color(0,1,0),
//                                        Color(1,0,0) );
    
    mRect = gl::Batch::create(geom::Plane(), mGlsl);
    
    mIcos = gl::Batch::create(geom::Icosahedron(), mGlsl);
    
    //mCube = gl::Batch::create(geom::Cube(), mGlsl);
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
}

void Day7App::mouseDown( MouseEvent event )
{
}

void Day7App::update()
{
    angle += 0.0523;
    
//    mSquare.colors(    Color(1,0,0),
//            Color(0,0,fabs(sin(angle))),
//            Color(0,1,0),
//            Color(1,0,0) );
//    
//    cout<< fabs(sin(angle)) <<endl;
    gl::enableAlphaBlending();
}

void Day7App::draw()
{
//	gl::clear( Color( 0.2f, 0.2f, 0.2f ) );

    gl::clear( );

    gl::setMatrices(mCam);
    
    const int NUM_PLANES = 128;
    
    gl::rotate(angleAxis(angle, vec3(0,1,0)));
    
    gl::pushModelMatrix();
    
    gl::rotate(angleAxis(angle, vec3(0,0,1)));
    
    for (int i = 0; i < NUM_PLANES; ++i)
    {
        float offset = i / (float)NUM_PLANES;
        float temp = sin(angle) + offset;
        
        ColorAf color(CM_HSV, lmap(temp, 0.f, 1.f, .1f, .9f), 1,1,0.3f);
        
        //PASS THE NEW VALUE FOR COLOR TO THE SHADER
        mGlsl->uniform("uColor", color);
        
        gl::ScopedModelMatrix scpMtx;
        
        float angle = M_PI * i /(float)NUM_PLANES;
        
        gl::rotate(angleAxis(angle, vec3(0,0,1)));
        mIcos->draw();
    }
    
               gl::popModelMatrix();
    //mCube->draw();
    
   // mRect->draw();
    
    saveGif();
}

void Day7App::saveGif()
{
    if (getElapsedFrames() % 4 == 0 && getElapsedFrames() < 241) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "7"/ ("7-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}

CINDER_APP( Day7App, RendererGl(RendererGl::Options().msaa(16)),
                    [&](App::Settings *settings){
                        settings->setWindowSize(500,500);
                        settings->setHighDensityDisplayEnabled();
                                })
