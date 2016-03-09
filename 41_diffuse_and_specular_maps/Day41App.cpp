#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/CameraUi.h"

#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Day41App : public App {
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
    
    gl::Texture2dRef    mDiffuseMap, mSpecularMap;
    
    vec3                lightColor;
    vec3                objectColor;
    vec3                lightPos;
    vec3                eyePos;
    
    float               theta;
};

void Day41App::setup()
{
    mCam.lookAt(vec3(0,0,5), vec3(0., 1.5, 0.));
    mCam.setPerspective(45.f, getWindowAspectRatio(), 1.f, 1000.f);
    
    auto img = loadImage(loadAsset("box.png"));
    auto img1 = loadImage(loadAsset("box_spec.png"));
    mDiffuseMap = gl::Texture2d::create(img);
    mSpecularMap = gl::Texture2d::create(img1);
    mDiffuseMap->bind(0);
    mSpecularMap->bind(1);
    
    
    lightPos = vec3(0.f);
    
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

void Day41App::mouseDown( MouseEvent event )
{
}

void Day41App::update()
{
    lightPos.x = (1.5f * cos(theta)) - 0.5;
    lightPos.y = 2+ 1.5f * sin(theta);
    lightPos.z = 1.5f;// * sin(theta*2);
    
    theta += 0.0523;
    
    eyePos = mCam.getEyePoint();
    
    mGlsl->uniform("light.position", vec3(mCam.getViewMatrix() * vec4(lightPos, 1.0f)));
    mGlsl->uniform("viewPos",  vec3(mCam.getViewMatrix() * vec4(eyePos, 1.0f)));
    
    vec3 lightColor = vec3(1.f);
    
    vec3 diffuseColor = lightColor * vec3(0.9f);
    vec3 ambientColor = diffuseColor * vec3(0.5f);
    
    mGlsl->uniform("light.diffuse", diffuseColor);
    mGlsl->uniform("light.specular", vec3(1.f));
    
    mGlsl->uniform("material.ambient", vec3(1.0f, 0.5f, 0.31f));
    mGlsl->uniform("material.diffuse", 0); //pass the diffuse map texture to shader
    mGlsl->uniform("material.specular", 1); //pass the specular map texture to the shader
    mGlsl->uniform("material.shininess", 32.f);
    
    
}

void Day41App::draw()
{
    gl::setMatrices(mCam);
    gl::clear( Color( .0, 0., 0. ) );
    
    for (int i = 0; i < 4; i++)
    {
        gl::ScopedMatrices push;
        gl::translate(0.,1.*i, 0.);;
        gl::rotate(0.5*sin(i*3), vec3(0.,1.,0.));
        mBatch->draw();
    }
    
    {
        gl::ScopedMatrices push;
        gl::translate(lightPos);
        mLight->draw();
    }
   if (getElapsedFrames() > 240.f) saveGif();
    
}


void Day41App::setupGlsl()
{
    mGlsl = gl::GlslProg::create(loadAsset("phong.vert"), loadAsset("phong.frag"));
}

void Day41App::saveGif()
{
    if (getElapsedFrames() % 4 == 0 && getElapsedFrames() < 481) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "41"/ ("41-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
}

CINDER_APP( Day41App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setWindowSize(500,500);
               settings->setHighDensityDisplayEnabled();
           })
