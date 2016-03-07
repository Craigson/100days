#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/CameraUi.h"

#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Day38App : public App {
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

void Day38App::setup()
{
    mCam.lookAt(vec3(0,0,5), vec3(0));
    mCam.setPerspective(45.f, getWindowAspectRatio(), 1.f, 1000.f);
    
    lightPos = vec4(0.f);
    
    theta = 0.0;
    
    setupGlsl();
    
    mCamUi = CameraUi(&mCam, getWindow());
    
    auto cube = geom::Sphere().subdivisions(128);
    
    mBatch = gl::Batch::create(cube, mGlsl);
    
    auto sphere = geom::Sphere();
    auto scale = geom::Scale(0.1f,0.1f,0.1f);
    auto shader = gl::ShaderDef().color();
    auto rotate = geom::Rotate(angleAxis(2.1f, vec3(1.0,0.,0.f)));
    mLight = gl::Batch::create(sphere >> scale >> rotate, gl::getStockShader(shader));
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
}

void Day38App::mouseDown( MouseEvent event )
{
}

void Day38App::update()
{
    lightPos.x = 1.5f * cos(theta);
    lightPos.y = 1.5f * sin(theta);
    lightPos.z = 1.5f * cos(sin(theta)/2.);
    
    theta += 0.0523;
    
    eyePos = vec4(mCam.getEyePoint(), 1.);
    
    mGlsl->uniform("uLightColor", vec3( 1.0f, 1.0f, 1.0f));
    mGlsl->uniform("uObjectColor", vec3(fabs(sin(theta/2)), fabs(cos(theta /3.)), 0.11f));
    
    mGlsl->uniform("uCam", vec3( gl::getModelView() * eyePos));
    mGlsl->uniform("uLightPos", vec3( gl::getModelView() * lightPos)); //WE MULTIPLY BY getModelView BECAUSE ALL OUR LIGHTING CALCULATIONS ARE BEING DONE IN WORLD-SPACE
    
    cout << lightPos << endl;
    
}

void Day38App::draw()
{
    gl::setMatrices(mCam);
    gl::clear( Color( .0, 0., 0. ) );
    
    {
        gl::ScopedMatrices push;
       // gl::rotate(theta, vec3(1.,0.,0.));
        mBatch->draw();
    }
    
    {
        gl::ScopedMatrices push;
        gl::translate(lightPos.x, lightPos.y, lightPos.z);
        mLight->draw();
    }
    
    if (getElapsedFrames() > 240.f) saveGif();
    
}


void Day38App::setupGlsl()
{
    mGlsl = gl::GlslProg::create(loadAsset("phong.vert"), loadAsset("phong.frag"));
}

void Day38App::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 481) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "38"/ ("38-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
}

CINDER_APP( Day38App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setWindowSize(500,500);
               settings->setHighDensityDisplayEnabled();
           })
