#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Utilities.h"
#include "cinder/Easing.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

static const int IMAGE_CHANNELS = 4; // set to 3 to experiment with no alpha channel

class Day60App : public App {
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
    
    void loadImage();
    
    void setupGlsl();
    void updateLights();
    
    void saveGif();
    
    CameraPersp mCam;
    CameraUi       mUi;
    
    gl::BatchRef        mBatch;
    gl::GlslProgRef     mGlsl;
    gl::Texture2dRef    mTex;
    
     std::vector<vec3>       mPointLights;
    
    float angle = 0;
    bool turnComplete = false;
    bool reverse = false;
    
    float rotation = 0;
    
    float theta = 0;
};

void Day60App::setup()
{
    mCam.lookAt(vec3(0,10,10), vec3(0));
    mCam.setPerspective( 35.0f, 1.0f, 1.f, 1000.0f );
    
    setupGlsl();
    
    mUi = CameraUi(&mCam, getWindow());
    
    setFrameRate(30);
    
    auto scale = geom::Scale(2.,2.,2.);
    auto sphere = geom::Sphere().subdivisions(64);
    
    auto rot = geom::Rotate(angleAxis(toRadians(90.f),vec3(1.,0.,0.)));
 
    mBatch = gl::Batch::create(sphere >> scale >> rot, mGlsl);

    loadImage();
    
    mPointLights = {
        vec3( 3.0f,  0.f,   .0f),
        vec3( -3.f,  0.f,  3.0f),
        vec3( 0.f ,  3.f,    0.),
        vec3( 0.0f,  -3.0f,  0.f)
    };
    
}

void Day60App::mouseDown( MouseEvent event )
{
}

void Day60App::update()
{
    updateLights();
    
    //INCREASE ANGLE OF ROTATION
    angle += 0.0523;
    theta += 0.0523;
}

void Day60App::draw()
{
    

    gl::clear( Color( 0.0, 0.0, 0.0 ) );
    
    ColorAf color(1.,.9,.2);
    
    gl::color(ColorAf(1.,.0,.0,1.));
    
    gl::setMatrices(mCam);
    
    gl::ScopedMatrices scp;
    
    gl::rotate(angleAxis(angle,vec3(0.,.0,1.)));
    
    gl::ScopedBlend scpBlend(false);
    
 //   gl::rotate(angleA, vec3(0,1,0));

    mBatch->draw();

    
    if(getElapsedFrames() > 240) saveGif();
    saveGif();
    
}

void Day60App::setupGlsl()
{
    mGlsl = gl::GlslProg::create(loadAsset("vertex.vert"), loadAsset("fragment.frag"));
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
}

void Day60App::loadImage()
{
    auto pic = cinder::loadImage(loadAsset("pattern.png"));
    gl::Texture2d::Format fmt;
    
    //SET THE INTERNAL FORMAT TO RGBA SO THAT WE CAN TEST THE ALPHA VALUE IN THE FRAG SHADER
    fmt.internalFormat((IMAGE_CHANNELS==4) ? GL_RGBA:GL_RGB );

    fmt.wrap(GL_CLAMP_TO_EDGE);
    
    mTex = gl::Texture2d::create(pic, fmt);
    
    mTex->bind();
}

void Day60App::updateLights()
{
    //MOVE THE POINT LIGHTS AROUND
    
    //1
    mPointLights[0].x = -3 * cos(theta / 2);
    mPointLights[0].y = -3 * sin(theta / 2);
    mPointLights[0].z = -3 * cos(theta / 2);
    
    //2
    mPointLights[1].x = 3 * sin(theta / 2 );
    mPointLights[1].y = 3 * cos(theta / 2 );
    mPointLights[1].z = 3 * cos(theta / 2 );
    
    //3
    mPointLights[2].x = 3 * cos(theta / 2 );
    mPointLights[2].y = -3 * sin(theta / 2 );
    mPointLights[2].z = 3 * sin(theta / 2 );
    
    //4
    mPointLights[3].x = 3 * cos( theta / 2 );
    mPointLights[3].y = 3 * cos( theta / 2);
    mPointLights[3].z = 3 * sin( theta / 2);
    
    
    
    //LIGHT 1
    
    mGlsl->uniform("pointLights[0].position", vec3(mCam.getViewMatrix() * vec4(mPointLights[0], 1.f)));
    mGlsl->uniform("pointLights[0].ambient", vec3(0.01f,0.01f,0.01f));
    mGlsl->uniform("pointLights[0].diffuse", vec3(0.25f,0.25f,0.25f));
    mGlsl->uniform("pointLights[0].specular", vec3(0.15f,0.15f,0.15f));
    mGlsl->uniform("pointLights[0].constant", 1.f);
    mGlsl->uniform("pointLights[0].linear", 0.09f);
    mGlsl->uniform("pointLights[0].quadratic", 0.032f);
    
    //LIGHT 2
    
    mGlsl->uniform("pointLights[1].position", vec3(mCam.getViewMatrix() * vec4(mPointLights[1], 1.f)));
    mGlsl->uniform("pointLights[1].ambient", vec3(0.01f,0.01f,0.01f));
    mGlsl->uniform("pointLights[1].diffuse", vec3(0.25f,0.25f,0.25f));
    mGlsl->uniform("pointLights[1].specular", vec3(0.15f,0.15f,0.15f));
    mGlsl->uniform("pointLights[1].constant", 1.f);
    mGlsl->uniform("pointLights[1].linear", 0.09f);
    mGlsl->uniform("pointLights[1].quadratic", 0.032f);
    
    //LIGHT 3
    
    mGlsl->uniform("pointLights[2].position", vec3(mCam.getViewMatrix() * vec4(mPointLights[2], 1.f)));
    mGlsl->uniform("pointLights[2].ambient", vec3(0.01f,0.01f,0.01f));
    mGlsl->uniform("pointLights[2].diffuse", vec3(0.25f,0.25f,0.25f));
    mGlsl->uniform("pointLights[2].specular", vec3(0.15f,0.15f,0.15f));
    mGlsl->uniform("pointLights[2].constant", 1.f);
    mGlsl->uniform("pointLights[2].linear", 0.09f);
    mGlsl->uniform("pointLights[2].quadratic", 0.032f);
    
    //BLUE LIGHT
    
    mGlsl->uniform("pointLights[3].position", vec3(mCam.getViewMatrix() * vec4(mPointLights[3], 1.f)));
    mGlsl->uniform("pointLights[3].ambient", vec3(0.01f,0.01f,0.01f));
    mGlsl->uniform("pointLights[3].diffuse", vec3(0.25f,0.25f,0.25f));
    mGlsl->uniform("pointLights[3].specular", vec3(0.15f,0.15f,0.15f));
    mGlsl->uniform("pointLights[3].constant", 1.f);
    mGlsl->uniform("pointLights[3].linear", 0.09f);
    mGlsl->uniform("pointLights[3].quadratic", 0.032f);
    
    theta += 0.0523;
}

void Day60App::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 361) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "60"/ ("60-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}
CINDER_APP( Day60App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setWindowSize(500,500);
               settings->setHighDensityDisplayEnabled();
           })