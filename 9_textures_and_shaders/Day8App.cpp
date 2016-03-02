#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Day8App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void saveGif();
    
    CameraPersp     mCam;
    
    gl::BatchRef    mRect;
    
    gl::GlslProgRef mGlsl;
    
    gl::Texture2dRef   mTex;
    
    float angle = 0;
    
    
};

void Day8App::setup()
{
    mCam.lookAt(vec3(3,2,3), vec3(0));
    
    auto img = loadImage(loadAsset("texture.jpg"));
    
    setFrameRate(60);
    
    mTex = gl::Texture2d::create(img);
    mTex->bind(0);
    
    /*  THIS SHADER PROGRAM IS FOR MULTIPLYING THE TEXTURE BY THE COLOUR
     
    mGlsl = gl::GlslProg::create( gl::GlslProg::Format().
                                 vertex(    CI_GLSL(150,
                                                    uniform mat4    ciModelViewProjection;
                                                    in vec4         ciPosition;
                                                    in vec2         ciTexCoord0; //THIS COMES FROM CINDER AUTOMATICALLY FROM geom::Attrib::TEX_COORD_0
                                                    out vec2        TexCoord0; //SEND IT TO THE FRAG SHADER
                                                    
                                                    void main(void)
                                                    {
                                                        gl_Position = ciModelViewProjection * ciPosition;
                                                        TexCoord0 = ciTexCoord0;
                                                    })).
                                 
                                 fragment(  CI_GLSL(150,
                                                    uniform vec4        uColor;
                                                    uniform sampler2D   uTex0;
                                                    
                                                    in vec2             TexCoord0;
                                                    out vec4            oColor;
                                                    
                                                    void main(void)
                                                    {
                                                        oColor = texture(uTex0, TexCoord0) * uColor;
                                                    })
                                          ));
    
    
    */
    
    mGlsl = gl::GlslProg::create( gl::GlslProg::Format().
                                 vertex( CI_GLSL(150,
                                                 
                                                 uniform mat4   ciModelViewProjection;
                                                 uniform vec2   uRes;
                                                 in vec4        ciPosition;
                                                 in vec2        ciTexCoord0;
                                                 out vec2       TexCoord0;
                                                 uniform float  uAngle; //VARIABLE FROM THE CPU FOR MODULATING THE SINE WAVE
                                                 
                                                 /*
                                                 float offset(vec2 uv)
                                                 {
                                                     //return (sin(uv.x * 15.0) +
                                                     //        cos(uv.y * 7.0f +uv.x * 13.0f )) * 0.1f;
                                                     vec4 pos = ciPosition;
                                                     float z = sin(uAng
                                                     return cos(sqrt( uv.x * uv.x + uv.y * uv.y));
                                                     
                                                   
                                                 }
                                                 */
                                                 
                                                 void main(void)
                                                 {
                                                     vec4 pos       = ciPosition;
                                                     pos.y          = sin(uAngle * sqrt(pow(pos.x,2) + pow(pos.z,2))) / 5.;
                                                     gl_Position    = ciModelViewProjection * pos;
                                                     TexCoord0      = ciTexCoord0;
                                                 }
                                                 )).
                                        fragment(   CI_GLSL(150,
                                                            
                                                            uniform float   uCheckSize;
                                                            uniform sampler2D   uTex0;
                                                            
                                                            in vec2         TexCoord0;
                                                            out vec4        oColor;
                                                            
                                                            // THIS IS FOR CREATING THE CHECKERED PATTERN
                                                            vec4 checker(vec2 uv)
                                                            {
                                                                float v = floor(uCheckSize * uv.x) +
                                                                floor( uCheckSize * uv.y);
                                                                
                                                                if (mod(v, 2.0) < 1.0)
                                                                    return vec4 (1,1,1,0.1);
                                                                else
                                                                    return vec4(0,0,0,0.1);
                                                            }
                                                             
                                                            
                                                            void main(void)
                                                            {
                                                                //oColor = checker (TexCoord0);
                                                                oColor = texture(uTex0, TexCoord0);
                                                            }
                                                            )));
                                 
    
    auto plane = geom::Plane().subdivisions(ivec2(30));
    
    mRect = gl::Batch::create(plane, mGlsl);
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
}

void Day8App::mouseDown( MouseEvent event )
{
}

void Day8App::update()
{
    angle = getElapsedFrames()/15.;
}

void Day8App::draw()
{
	gl::clear( Color( 0.2f, 0.2f, 0.2f ) );
    gl::setMatrices(mCam);
    
    gl::rotate(angleAxis( angle/2, vec3(0,1,0)));
    
    //mGlsl->uniform("uCheckSize", 60.0f);
    
    //SEND THE INCREASING ANGLE VARIABLE TO THE SHADER
    mGlsl->uniform("uAngle", angle);
    mRect->draw();
    
    /*
    const int NUM_PLANES = 2;
    
    for (int p = 0; p < NUM_PLANES; ++p)
    {
        float hue = p / (float) NUM_PLANES;
        ColorAf color(CM_HSV, hue, 1, 1, 1);
        mGlsl->uniform("uTex0", 0);
        mGlsl->uniform("uColor", color);
        
        gl::ScopedModelMatrix scpMtx;
        
        float angle = M_PI * p / (float(NUM_PLANES));
        gl::rotate(angleAxis(angle, vec3(1,0,0)));
        mRect->draw();
    }
     */
   saveGif();
}

void Day8App::saveGif()
{
    if (getElapsedFrames() % 4 == 0 && getElapsedFrames() < 241) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "8"/ ("8-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}

CINDER_APP( Day8App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(500,500);
           })
