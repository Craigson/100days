#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Day34App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    void setupShader();
    
    void saveGif();
    
    gl::GlslProgRef     mGlsl;
    gl::BatchRef        mBatch;
    
    CameraPersp         mCam;
    
    gl::BatchRef        mLight;
    vec3                mLightColor;
    vec3                mLightPos;
    vec3                mObjectColor;
    
    float angle = 0;
};

void Day34App::setup()
{
    mCam.lookAt(vec3(0,0,10), vec3(0));
    mCam.setPerspective(45.f, getWindowAspectRatio(), 1.f, 1000.f);
    

    
    setupShader();
    
    auto sphere = geom::Sphere();
    auto shader = gl::ShaderDef().color();
    
    mLight = gl::Batch::create(geom::Sphere(), gl::getStockShader(shader));
    
    auto cube = geom::Cube();
    
    auto scale = geom::Scale(3.,3.,3.);
    mBatch = gl::Batch::create(cube >> scale, mGlsl);
    
    mLightPos = vec3(0.5f,0.5f,0.0f);
    mLightColor = vec3(1.f,1.f,1.f);
    mObjectColor = vec3(1.f, 0.f, 0.f);
    
   // gl::enableDepthRead();
   // gl::enableDepthWrite();
   // gl::enableWireframe();
    gl::enableAlphaBlending();
}

void Day34App::mouseDown( MouseEvent event )
{
}

void Day34App::update()
{
    cout << mLightColor << endl;
    cout << mObjectColor << endl;
    
    mGlsl->uniform("objectColor", mObjectColor);
    mGlsl->uniform("lightColor", mLightColor);
    
    angle += 0.0523;
}

void Day34App::draw()
{
    gl::setMatrices(mCam);
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::ScopedModelMatrix scpMtx;
    gl::rotate(angleAxis(angle, vec3(.0, 1.0, 0.0)));
    gl::rotate(angleAxis(5.f, vec3(1., 0.0, 0.0)));
   // gl::color(Color(1.,0.,0.0));
    
    mBatch->draw();
    
    saveGif();
}

void Day34App::setupShader()
{
    mGlsl = gl::GlslProg::create(gl::GlslProg::Format()
                                 .vertex(CI_GLSL(150,
                                                 
                                                 uniform mat4	ciModelViewProjection;
                                                 
                                                 in vec4 ciPosition;
                                                 
                                                 void main(void)
                                                 {
                                                     gl_Position = ciModelViewProjection * ciPosition;
                                                 }
                                                 
                                                 ))
                                 .fragment(CI_GLSL(150,
                                                   
                                                   out vec4 oColor;
                                                   
                                                   uniform vec3 objectColor;
                                                   uniform vec3 lightColor;
                                                   
                                                   void main(void)
                                                   {
                                                       vec3 temp = objectColor * lightColor;
                                                       
                                                       oColor = vec4(temp, 1.f);
                                                   }
                                                   
                                                   )));
    

}
void Day34App::saveGif()
{
    
    if (getElapsedFrames() % 4 == 0 && getElapsedFrames() < 121) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "34"/ ("34-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
}

CINDER_APP( Day34App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setWindowSize(500,500);
               settings->setHighDensityDisplayEnabled();
           })
