#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/CameraUi.h"

#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Day39App : public App {
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
    
    void setupGlsl();
    
    void saveGif();
    
    CameraPersp         mCam;
    CameraUi            mCamUi;
    
    gl::GlslProgRef     mGlsl;
    gl::BatchRef        mBatch;
    gl::BatchRef        mLight;
    
    vec3                lightColor;
    vec3                objectColor;
    vec4                lightPos;
    vec4                eyePos;
    
    float               theta;
};

void Day39App::setup()
{
    mCam.lookAt(vec3(0,0,5), vec3(0));
    mCam.setPerspective(45.f, getWindowAspectRatio(), 1.f, 1000.f);
    
    lightPos = vec4(0.f);
    
    theta = 0.0;
    
    setupGlsl();
    
    mCamUi = CameraUi(&mCam, getWindow());
    
    auto cube = geom::Cube();
    
    mBatch = gl::Batch::create(cube, mGlsl);
    
    auto sphere = geom::Sphere();
    auto scale = geom::Scale(0.1f,0.1f,0.1f);
    auto shader = gl::ShaderDef().color();
    auto rotate = geom::Rotate(angleAxis(2.1f, vec3(1.0,0.,0.f)));
    mLight = gl::Batch::create(sphere >> scale >> rotate, gl::getStockShader(shader));
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
}

void Day39App::mouseDown( MouseEvent event )
{
}

void Day39App::update()
{
    lightPos.x = 1.5f * cos(theta);
    lightPos.y = 1.5f * sin(-theta);
    lightPos.z = 1.5f * cos(sin(theta)/2.);
    
    theta += 0.0523;
    
    eyePos = vec4(mCam.getEyePoint(), 1.);
    
    mGlsl->uniform("light.position", gl::getModelView() * lightPos);
    mGlsl->uniform("viewPos",  eyePos);
    
    vec3 lightColor = vec3(sin(getElapsedSeconds() * 2.0f), sin(getElapsedSeconds() * 0.7f), sin(getElapsedSeconds() * 1.3f));
    
    vec3 diffuseColor = lightColor * vec3(0.5f);
    vec3 ambientColor = diffuseColor * vec3(0.2f);
    
    mGlsl->uniform("light.ambient", ambientColor);
    mGlsl->uniform("light.diffuse", diffuseColor);
    mGlsl->uniform("light.specular", vec3(1.f));
    
    mGlsl->uniform("material.ambient", vec3(1.0f, 0.5f, 0.31f));
    mGlsl->uniform("material.diffuse", vec3(1.0f, 0.5f, 0.31f));
    mGlsl->uniform("material.specular", vec3(0.5f, 0.5f, 0.5f));
    mGlsl->uniform("material.shininess", 32.f);
    
    
}

void Day39App::draw()
{
    gl::setMatrices(mCam);
    gl::clear( Color( .0, 0., 0. ) );
    
    {
        gl::ScopedMatrices push;
        gl::rotate(theta/2., vec3(1.,0.,0.));
        mBatch->draw();
    }
    
   if (getElapsedFrames() > 240.f) saveGif();
    
}


void Day39App::setupGlsl()
{
    mGlsl = gl::GlslProg::create(loadAsset("phong.vert"), loadAsset("phong.frag"));
}

void Day39App::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 481) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "39"/ ("39-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
}

CINDER_APP( Day39App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setWindowSize(500,500);
               settings->setHighDensityDisplayEnabled();
           })
