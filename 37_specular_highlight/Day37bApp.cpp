#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/CameraUi.h"

#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Day37bApp : public App {
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
    vec3                eyePos;
    
    float               theta;
};

void Day37bApp::setup()
{
    mCam.lookAt(vec3(0,0,5), vec3(0));
    mCam.setPerspective(45.f, getWindowAspectRatio(), 1.f, 1000.f);
    
    lightPos = vec4(0.f);
    
    theta = 0.0;
    
    setupGlsl();
    
    mCamUi = CameraUi(&mCam, getWindow());
    
    auto cube = geom::TorusKnot();
    
    mBatch = gl::Batch::create(cube, mGlsl);
    
    auto sphere = geom::Sphere();
    auto scale = geom::Scale(0.1f,0.1f,0.1f);
    auto shader = gl::ShaderDef().color();
    auto rotate = geom::Rotate(angleAxis(2.1f, vec3(1.0,0.,0.f)));
    mLight = gl::Batch::create(sphere >> scale >> rotate, gl::getStockShader(shader));
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
}

void Day37bApp::mouseDown( MouseEvent event )
{
}

void Day37bApp::update()
{
    lightPos.x = 1.f * cos(theta);
    lightPos.y = 1.f * sin(theta);
     lightPos.z = 1.f * cos(sin(theta)*2.f);
    
    theta += 0.0523;
    
    eyePos = mCam.getEyePoint();
    
    mGlsl->uniform("uLightPos", vec3( gl::getModelView() * lightPos));
    
    cout << lightPos << endl;
    
}

void Day37bApp::draw()
{
    gl::setMatrices(mCam);
    gl::clear( Color( 0., 0., 0 ) );
    
    {
        gl::ScopedMatrices push;
        gl::rotate(theta, vec3(1.,0.,0.));
        mBatch->draw();
    }
    
    {
        gl::ScopedMatrices push;
        gl::translate(lightPos.x, lightPos.y, lightPos.z);
        mLight->draw();
    }
    
    if (getElapsedFrames() > 240.f) saveGif();
    
}


void Day37bApp::setupGlsl()
{
    mGlsl = gl::GlslProg::create(gl::GlslProg::Format()
                                 .vertex(CI_GLSL(150,
                                                 
                                                 uniform mat4 ciModelViewProjection;
                                                 uniform mat4 ciModelView;
                                                 uniform mat3 ciNormalMatrix;
                                                 
                                                 in vec4 ciPosition;
                                                 in vec3 ciNormal;
                                                 
                                                 out vec4 vPosition;
                                                 out vec3 vNormal;
                                                 
                                                 
                                                 
                                                 
                                                 void main(void)
                                                 {
                                                     vNormal = ciNormalMatrix * ciNormal;
                                                     
                                                     vPosition = ciModelView * ciPosition;
                                                     
                                                     gl_Position = ciModelViewProjection * ciPosition;
                                                     
                                                 }
                                                 
                                                 ))
                                 
                                 .fragment(CI_GLSL(150,
                                                   
                                                   uniform vec3 uLightPos;
                                                   
                                                   in vec4 vPosition;
                                                   in vec3 vNormal;
                                                   
                                                   out vec4 oColor;
                                                   
                                                   void main()
                                                   {
                                                       
                                                       vec3 ambient = vec3(.05f); //AMBIENT
                                                       
                                                       //DIFFUSE
                                                       vec3 normal = normalize(vNormal);
                                                       
                                                       vec3 lightDir = normalize(uLightPos - vPosition.xyz); //gl_FragCoord might not be appropriate
                                                    
                                                       float dist = length(lightDir);
                                                       
                                                       float attenuation = 1. / (dist + dist*dist);
                                                       
                                                       vec3 cameraVector = normalize( -vPosition.xyz);
                                                       
                                                       vec3 reflection = normalize( reflect(-lightDir, normal));
                                                       
                                                       vec3 diffuse = vec3( max( dot( normal, lightDir), 0.0) );
                                                       
                                                       const float shininess_factor = 32.;
                                                       
                                                       vec3 specular = vec3( pow( max( dot( reflection, cameraVector), 0.0), shininess_factor) );
                                                       
                                                       oColor = vec4(attenuation * ( specular + diffuse) + ambient, 1.);
                                                       
                                                       
                                                   }
                                                   
                                                   )));
    
}

void Day37bApp::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 361) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "37"/ ("37-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
}

CINDER_APP( Day37bApp, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setWindowSize(500,500);
               settings->setHighDensityDisplayEnabled();
           })
