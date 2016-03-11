#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Rand.h"

#include "cinder/CameraUi.h"

#include "cinder/Utilities.h"

#define NUM_OBJECTS 100

using namespace ci;
using namespace ci::app;
using namespace std;

class Day42App : public App {
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
    
    std::vector<std::pair<vec3, float>>   mPositions;
    
    quat                mQuat;
};

void Day42App::setup()
{
    mCam.lookAt(vec3(0,20,15), vec3(0., 0., 0.));
    mCam.setPerspective(45.f, getWindowAspectRatio(), 1.f, 1000.f);
    
    auto img = loadImage(loadAsset("box.png"));
    auto img1 = loadImage(loadAsset("box_spec.png"));
    mDiffuseMap = gl::Texture2d::create(img);
    mSpecularMap = gl::Texture2d::create(img1);
    mDiffuseMap->bind(0);
    mSpecularMap->bind(1);
    
    mQuat = quat(0.9, vec3(1.,0.5,.3));
    
    lightPos = vec3(0.f);
    
    for (int i = 0; i < NUM_OBJECTS; i++)
    {
        mPositions.push_back(make_pair(vec3(randFloat(-5.f,5.f), randFloat(-3.f,3.f), randFloat(-5.f,5.f) ), randFloat(-2.,2.)));
    }
    
    theta = 0.0;
    
    setupGlsl();
    
    mCamUi = CameraUi(&mCam, getWindow());
    
    auto scale1 = geom::Scale(0.3,0.3,0.3);
    auto cube = geom::Icosahedron();
    
    mBatch = gl::Batch::create(cube >> scale1, mGlsl);
    
    auto sphere = geom::Sphere();
    auto scale = geom::Scale(0.1f,0.1f,0.1f);
    auto shader = gl::ShaderDef().color();
    auto rotate = geom::Rotate(angleAxis(2.1f, vec3(1.0,0.,0.f)));
    mLight = gl::Batch::create(sphere >> scale >> rotate, gl::getStockShader(shader));
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
}

void Day42App::mouseDown( MouseEvent event )
{
}

void Day42App::update()
{
    lightPos.x = (1.5f * cos(theta)) - 0.5;
    lightPos.y = 2+ 1.5f * sin(theta);
    lightPos.z = 1.5f;// * sin(theta*2);
    
    theta += 0.0523;
    
    eyePos = mCam.getEyePoint();
    
    mGlsl->uniform("light.direction", vec3(-0.5f, -1.0f, -0.7f));
    
    vec3 lightColor = vec3(1.f);
    
    vec3 diffuseColor = lightColor * vec3(0.9f);
  //  vec3 ambientColor = diffuseColor * vec3(0.5f);
    
    mGlsl->uniform("light.diffuse", diffuseColor);
    mGlsl->uniform("light.specular", vec3(-0.4f, -1.0f, -0.7f));
    
    mGlsl->uniform("material.ambient", vec3(1.0f, 0.5f, 0.31f));
    mGlsl->uniform("material.diffuse", 0); //pass the diffuse map texture to shader
    mGlsl->uniform("material.specular", 1); //pass the specular map texture to the shader
    mGlsl->uniform("material.shininess", 128.f);
    
    
}

void Day42App::draw()
{
    gl::setMatrices(mCam);
    gl::clear( Color( .1, 0.1, 0.1 ) );
    
    //gl::rotate(angleAxis(theta, vec3(0.,1.,0.)));
    
    for (int i = 0; i < NUM_OBJECTS; i++)
    {
        gl::ScopedMatrices push;
        gl::translate(mPositions[i].first);
        gl::rotate(mPositions[i].second *theta/2, vec3(i / NUM_OBJECTS, 1., 0.3));
        mBatch->draw();
    }

if (getElapsedFrames() > 240.f) saveGif();
}


void Day42App::setupGlsl()
{
    mGlsl = gl::GlslProg::create(loadAsset("phong.vert"), loadAsset("phong.frag"));
}

void Day42App::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 361) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "42"/ ("42-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
}

CINDER_APP( Day42App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setWindowSize(500,500);
               settings->setHighDensityDisplayEnabled();
           })
