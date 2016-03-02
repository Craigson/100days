#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Utilities.h"

#include "cinder/CameraUi.h"



using namespace ci;
using namespace ci::app;
using namespace std;

class Day20aApp : public App {
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
    
    void setupGlsl();
    
    CameraPersp mCam;
    CameraUi       mUi;
    
    gl::BatchRef        mBatch;
    gl::GlslProgRef     mGlsl;
    gl::Texture2dRef      mTexture;
};

void Day20aApp::setup()
{
    mCam.lookAt(vec3(0,0,10), vec3(0));
    mCam.setPerspective( 35.0f, 1.0f, 1.f, 1000.0f );
    
    setupGlsl();
    
    mUi = CameraUi(&mCam, getWindow());
    
    auto img = loadImage(loadAsset("smiley.png"));
    
    gl::Texture2d::Format fmt;
    
    //fmt.setTarget(GL_ALPHA);
    
    mTexture = gl::Texture2d::create(img, fmt);
    
    mTexture->bind(0);
    
    auto scale = geom::Scale(2.,2.,2.);
    auto sphere = geom::Sphere().subdivisions(64);
    auto rotate = geom::Rotate(angleAxis(210.f, vec3(0,1,0)));
    mBatch = gl::Batch::create(sphere >> scale >> rotate, mGlsl);
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    
    
    
}

void Day20aApp::mouseDown( MouseEvent event )
{
}

void Day20aApp::update()
{
}

void Day20aApp::draw()
{
    gl::clear( Color( 0.2, 0.2, 0.2 ) );
    
    ColorAf color(1.,.9,.2);
    
    gl::color(Color(1.,.9,.2));
    
    gl::setMatrices(mCam);
    
//    gl::enableAlphaBlending();
//    
//    gl::ScopedBlendAlpha blend;
////    
//    mGlsl->uniform("uTex0", 0);
//        mGlsl->uniform("color", color);
    
    gl::ScopedBlend scpBlend(false);
   
    mBatch->draw();
    
    
}

void Day20aApp::setupGlsl()
{

    
    auto shader = gl::ShaderDef().lambert().color().texture();
     mGlsl = gl::getStockShader(shader);
    
}

CINDER_APP( Day20aApp, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setWindowSize(500,500);
               settings->setHighDensityDisplayEnabled();
           })
