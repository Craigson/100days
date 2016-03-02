#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "cinder/CameraUi.h"

#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const int NUM_INSTANCES = 1024;

class Day28aApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    bool record = false;
    
    void saveGif();
    
    void setupShader();
    
    gl::VboRef              mInstanceVbo;
    gl::GlslProgRef         mGlsl;
    gl::BatchRef            mBatch;
    CameraPersp             mCam;
    CameraUi                mCamUi;
    
    std::vector<vec3>       mPositions;
    std::vector<vec3>       mScales;
    std::vector<float>       mSpeeds;
    
    float zPos = 0;
    
    double speed = 0.2;
    
    gl::BatchRef            mSphere;
};

void Day28aApp::setup()
{
    mCam.lookAt(vec3(0,0,150), vec3(0));
    mCam.setPerspective(45, getWindowAspectRatio(), .1f, 300.f);
    
    mCamUi = CameraUi(&mCam, getWindow());
    
    setupShader();
    
   
    
    auto lambert = gl::ShaderDef().color();
    
    mBatch = gl::Batch::create(geom::Cube(), gl::getStockShader(lambert));
    
    mSphere = gl::Batch::create(geom::Sphere(), gl::getStockShader(lambert));
    
    for (int i = 0; i < NUM_INSTANCES; i++)
    {
        float scale = Rand::randFloat(.5f, 1.5f);
        mPositions.push_back(vec3(Rand::randFloat(-120., 120.f),Rand::randFloat(-120., 120.f), 0.f));
        mScales.push_back(vec3(scale, scale, Rand::randFloat(1.5f, 50.f)));
        
        mSpeeds.push_back(0.5);
    }
}

void Day28aApp::mouseDown( MouseEvent event )
{
    record = true;
    cout << "click" << endl;
}

void Day28aApp::update()
{
    zPos += 0.0523;
    
   // mCam.lookAt(vec3(0,0,100 + 200*fabs(sin(zPos))), vec3(0));
    
}

void Day28aApp::draw()
{
    gl::setMatrices(mCam);
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::color(Color(1.f,1.f,1.f));
    
    for (int i = 0; i < NUM_INSTANCES; i++)
    {
        gl::ScopedModelMatrix scpMtx;
        
        
        gl::translate(mPositions[i]);
        gl::scale(mScales[i]);
        
        mBatch->draw();
        
        mPositions[i].z = 50*sin(mSpeeds[i] * zPos);
    }
    
  //  if (getElapsedFrames() > 900) saveGif();
}

void Day28aApp::setupShader()
{
    mGlsl = gl::GlslProg::create(gl::GlslProg::Format().
                                 vertex(CI_GLSL(150,
                                                
                                                uniform mat4    ciModelViewProjection;
                                                uniform mat3    ciNormalMatrix;
                                                
                                                in vec4         ciPosition;
                                                in vec4         ciColor;
                                                in vec3         vInstancePosition; //per-instance position variable
                                                in vec3         ciNormal;
                                                
                                                out vec4        Color;
                                                out highp vec3	Normal;
                                                
                                                void main(void)
                                                {
                                                    gl_Position = ciModelViewProjection * (4.0 * ciPosition + vec4 ( vInstancePosition, 0));
                                                    Color = ciColor;
                                                    Normal		= ciNormalMatrix * ciNormal;
                                                }
                                                
                                                )).
                                 fragment(CI_GLSL(150,
                                                  
                                                  in vec4       Color;
                                                  in vec3       Normal;
                                                  
                                                  out vec4      oColor;
                                                  
                                                  void main(void)
                                                  {
                                                      vec3 normal = normalize( -Normal );
                                                      float diffuse = max( dot( normal, vec3( 0, 0, -1 ) ), 0 );
                                                      oColor = Color * diffuse;
                                                  }
                                                   ))
                                 );
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
}

void Day28aApp::saveGif()
{
    if (getElapsedFrames() % 4 == 0 && getElapsedFrames() < 1500) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "28"/ ("28-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}
CINDER_APP( Day28aApp, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(500,500);
           })
