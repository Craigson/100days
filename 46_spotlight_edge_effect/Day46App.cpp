#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/CameraUi.h"
#include "cinder/Rand.h"

#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;


#define NUM_OBJS 800
#define NUM_X 30
#define NUM_Y 30
#define GRID_SIZE 2

class Day46App : public App {
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
    std::vector<float>  mScales;
    
    float               theta;
};

void Day46App::setup()
{
    mCam.lookAt(vec3(0,0,7.5), vec3(0, 0, 0.));
    mCam.setPerspective(45.f, getWindowAspectRatio(), 1.f, 1000.f);
    
    lightPos = vec3(0.f);
    
    theta = 0.0;
    
    setupGlsl();
    
    mCamUi = CameraUi(&mCam, getWindow());
    
    auto cube = geom::Cube();
    
    auto s = geom::Sphere().subdivisions(128);
    auto scal = geom::Scale(1.5,1.5,1.5);
    
    mBatchB = gl::Batch::create(s, mGlsl);
    
    mBatch = gl::Batch::create(cube, mGlsl);
    
    auto sphere = geom::Sphere();
    auto scale = geom::Scale(0.1f,0.1f,0.1f);
    auto shader = gl::ShaderDef().color();
    auto rotate = geom::Rotate(angleAxis(2.1f, vec3(1.0,0.,0.f)));
    mLight = gl::Batch::create(sphere >> scale >> rotate, gl::getStockShader(shader));
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    for (int i=0; i < NUM_OBJS; i++)
    {
        mPositions.push_back(vec3(randFloat(-20.,20),randFloat(-20.,20),randFloat(-20.,-7)));
    }
    
    for (int i =0; i < NUM_X*NUM_Y ; i++)
    {
        mScales.push_back(randFloat(0.2f, 15.f));
    }
    
    setFrameRate(60);
}

void Day46App::mouseDown( MouseEvent event )
{
}

void Day46App::update()
{
//    lightPos.x = 5.f * cos(theta);
//    lightPos.y = 5.f * sin(theta);
//    lightPos.z = 0.f;
    
    lightPos = vec3(0.f);
    
    theta += 0.0523;
    
    eyePos = mCam.getEyePoint();
    
    //VERY NB! CONVERT THE LIGHT'S POSITION TO VIEW SPACE BEFORE SENDING IT TO THE GPU, THIS IS BECAUSE ALL THE CALCULATIONS IN THE SHADERS ARE HAPPENING IN VIEW SPACE! THE LIGHT'S POSITION ACTUALLY ACATS A TARGET FOR THE LIGHT, THE CAMERA POSITION IS THE ACTUAL "LOCATION" OF THE LIGHT
    
    mGlsl->uniform("light.position", vec3( mCam.getViewMatrix() * vec4( lightPos, 1 )) );
    
    //SET THE CUTOFF ANGLE FOR THE SPOTLIGHT (WE CALCULAT THE COSINE OF THE ANGLE BECAUSE WE NEED TO COMPARE THE TWO VECTORS USING THE DOT PRODUCT, IT WOULD BE MUCH MORE EXPENSIVE TO DO THIS FOR FRAGMENT ON THE GPU SIDE.. WHATEVER WE PASS AS THE ARGUMENT TO toRadian() IS THE "FOV" OF THE SPOTLIGHT CONE
    
    mGlsl->uniform("light.innerCutOff", cos(toRadians(5.f))); //THIS IS PHI
    mGlsl->uniform("light.outerCutOff", cos(toRadians(35.f))); //THIS IS EPSILON
    
    cout << eyePos << endl;
    
    //CREATE A VEC3 THAT POINTS FROM THE CAMERA'S POSITION TO IT'S TARGET (WILL REVERSE THE DIRECTION AND NORMALIZE IN THE FRAG SHADER) - THIS IS THE SPOTLIGHT'S DIRECTION VECTOR
    vec3 camToTarget = vec3(0.,0.,-15.) - eyePos;
    mGlsl->uniform("light.direction", vec3( mCam.getViewMatrix() * vec4( camToTarget, 1 )) );
    
    //PASS UNIFORMS FOR ATTENUATION
    mGlsl->uniform("light.constant", 1.f);
    mGlsl->uniform("light.linear", 0.050f);
    mGlsl->uniform("light.quadratic", 0.0019f);
    
    
}

void Day46App::draw()
{
    gl::setMatrices(mCam);
    gl::clear( Color( 0.0, 0.0, 0.0 ) );
    

    gl::ScopedMatrices scpMtr;
    
    gl::translate(-NUM_X / 4., - NUM_Y/4, -10.);
   // gl::enableWireframe();
    int counter = 0;
    for (int i = 0; i < NUM_X; i++)
    {
        for (int j = 0; j < NUM_Y; j++)
        {
            float x = ( -0.1f + i / 2.0f);
            float y = ( -0.1f + j / 2.0f);
            
            gl::ScopedMatrices push;
            gl::translate(x,y,0);
            gl::scale(.5,.5,mScales[counter]*cos(theta/2));
            mBatch->draw();
            counter++;
        }
    }
    saveGif();
}


void Day46App::setupGlsl()
{
    mGlsl = gl::GlslProg::create(loadAsset("vertex.vert"), loadAsset("fragment.frag"));
    
}

void Day46App::saveGif()
{
    if (getElapsedFrames() % 4 == 0 && getElapsedFrames() < 121) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "46"/ ("46-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
}

CINDER_APP( Day46App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setWindowSize(500,500);
               settings->setHighDensityDisplayEnabled();
           })
