#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/CameraUi.h"
#include "cinder/Rand.h"

#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;


#define NUM_OBJS 200

class Day43App : public App {
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
    gl::BatchRef        mBatch, mBatchB;
    gl::BatchRef        mLight;
    
    vec3                lightColor;
    vec3                objectColor;
    vec3                lightPos;
    vec3                eyePos;
    
    std::vector<vec3>   mPositions;
    
    float               theta;
};

void Day43App::setup()
{
    mCam.lookAt(vec3(0,0,5), vec3(0));
    mCam.setPerspective(45.f, getWindowAspectRatio(), 1.f, 1000.f);
    
    lightPos = vec3(0.f);
    
    theta = 0.0;
    
    setupGlsl();
    
    mCamUi = CameraUi(&mCam, getWindow());
    
    auto cube = geom::Icosahedron();
    
    auto s = geom::Sphere().subdivisions(128);
    auto scal = geom::Scale(0.3,0.3,0.3);
    
    mBatchB = gl::Batch::create(s, mGlsl);
    
    mBatch = gl::Batch::create(cube >> scal, mGlsl);
    
    auto sphere = geom::Sphere();
    auto scale = geom::Scale(0.1f,0.1f,0.1f);
    auto shader = gl::ShaderDef().color();
    auto rotate = geom::Rotate(angleAxis(2.1f, vec3(1.0,0.,0.f)));
    mLight = gl::Batch::create(sphere >> scale >> rotate, gl::getStockShader(shader));
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    for (int i=0; i < NUM_OBJS; i++)
    {
        mPositions.push_back(vec3(randFloat(-10.,10),randFloat(-10.,10),randFloat(-10.,10)));
    }
    setFrameRate(60);
}

void Day43App::mouseDown( MouseEvent event )
{
}

void Day43App::update()
{
    lightPos.x = 3.2f * cos(theta);
    lightPos.y = 0.8 * sin(cos(theta*2));
    lightPos.z = 2.f * sin(theta * 2);
    
    theta += 0.0523;
    
    eyePos = mCam.getEyePoint();
    
    //VERY NB! CONVERT THE LIGHT'S POSITION TO VIEW SPACE BEFORE SENDING IT TO THE GPU, THIS IS BECAUSE ALL THE CALCULATIONS IN THE SHADERS ARE HAPPENING IN VIEW SPACE!
    
    mGlsl->uniform("uLightPos", vec3( mCam.getViewMatrix() * vec4( lightPos, 1 )));
    mGlsl->uniform("uConstant", 1.f);
    mGlsl->uniform("uLinear", 0.045f);
    mGlsl->uniform("uQuatratic", 0.0075f);
    
    cout << lightPos << endl;
    
}

void Day43App::draw()
{
    gl::setMatrices(mCam);
    gl::clear( Color( 0.05, 0.05, 0.05 ) );
    
    gl::rotate(angleAxis(theta, vec3(0.1,1.,0.)));
   // gl::rotate(angleAxis(theta, vec3(0.3,0.,0.)));
    
    int counter = 100;
    for (vec3 v : mPositions)
    {
        gl::ScopedMatrices push;
        gl::translate(v);
        gl::rotate(counter + theta, vec3(1.,0.,0.));
        gl::rotate(counter + theta, vec3(0.5,0.8,0.));
        mBatch->draw();
        counter++;
    }
    
//    {
//        gl::ScopedMatrices push;
//        gl::translate(1.5f, 0.f,0.f);
//        mBatchB->draw();
//    }
    
    {
        gl::ScopedMatrices push;
        gl::translate(lightPos.x, lightPos.y, lightPos.z);
        mLight->draw();
    }
    
    if (getElapsedFrames() > 240.f) saveGif();
    
}


void Day43App::setupGlsl()
{
    mGlsl = gl::GlslProg::create(loadAsset("vertex.vert"), loadAsset("fragment.frag"));
    
}

void Day43App::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 481) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "43"/ ("43-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
}

CINDER_APP( Day43App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setWindowSize(500,500);
               settings->setHighDensityDisplayEnabled();
           })
