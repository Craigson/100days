#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/CameraUi.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define NUM_X 10
#define NUM_Y 10
#define NUM_Z 10

class Day47App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void setupShader();
    void saveGif();
    
    gl::GlslProgRef         mGlsl;
    gl::BatchRef            mBatch;
    gl::BatchRef            mLight;
    
    std::vector<vec3>       mPointLights;
    
    CameraPersp             mCam;
    CameraUi                mCamUi;
    
    float theta = 0.;
    
    
};

void Day47App::setup()
{
    
    setupShader();
    
    mCam.lookAt(vec3(0,0,10), vec3(0,0,0));
    mCam.setPerspective(45.f, getWindowAspectRatio(), 1.f, 1000.f);
    
    mCamUi = CameraUi(&mCam, getWindow());
    
    auto cube = geom::Sphere().subdivisions(128);
    
    mBatch = gl::Batch::create(cube >> geom::Scale(2.,2.,2.), mGlsl);
    
    auto shader = gl::ShaderDef().color();
    
    auto light = geom::Sphere().subdivisions(16);
    
    mLight = gl::Batch::create(light >> geom::Scale (0.1, 0.1,0.1), gl::getStockShader(shader));
    
    mPointLights = {
        vec3( 3.0f,  0.f,   .0f),
        vec3( -3.f,  0.f,  3.0f),
        vec3( 0.f ,  3.f,    0.),
        vec3( 0.0f,  -3.0f,  0.f)
    };
    
    vec3 rando = randVec3();
    
    cout << rando << endl;
}

void Day47App::mouseDown( MouseEvent event )
{
}

void Day47App::update()
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

void Day47App::draw()
{
    gl::setMatrices(mCam);
	gl::clear( Color( 0, 0, 0 ) );
    gl::color(ColorAf(1.0f,1.f,1.f,1.f));
    

    {
        gl::ScopedMatrices push;
        mBatch->draw();

    }
    
    //COLOUR AND DRAW THE POINT LIGHTS
    for (int i = 0; i < mPointLights.size(); i++)
    {
        
        gl::ScopedMatrices push;
        gl::translate(mPointLights[i]);
        mLight->draw();
        
    }
    
    saveGif();
}

void Day47App::setupShader()
{
    mGlsl = gl::GlslProg::create(loadAsset("vert.vert"), loadAsset("frag.frag"));
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
}

void Day47App::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 121) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "47"/ ("47-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
}

CINDER_APP( Day47App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setWindowSize(500,500);
               settings->setHighDensityDisplayEnabled();
           })
