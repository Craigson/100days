#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/CameraUi.h"

#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Day35aApp : public App {
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
    vec3                lightPos;
    
    float               theta;
};

void Day35aApp::setup()
{
    mCam.lookAt(vec3(0,0,5), vec3(0));
    mCam.setPerspective(45.f, getWindowAspectRatio(), 1.f, 1000.f);
    
    lightPos = vec3(1.5f, 1.5f, 1.5f);
    
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

void Day35aApp::mouseDown( MouseEvent event )
{
}

void Day35aApp::update()
{
    lightPos.x = 1.5f * cos(theta);
    lightPos.y = 1.5f * sin(theta);
    lightPos.z = 1.5f * cos(sin(theta)*2.f);
    theta += 0.0523;
    
    mGlsl->uniform("uLightColor", vec3(1.f,1.f,1.f));
    mGlsl->uniform("uObjectColor", vec3(1.f, 0.f, 0.f));
    mGlsl->uniform("uLightPos", lightPos);
    
    cout << lightPos << endl;
    
}

void Day35aApp::draw()
{
    gl::setMatrices(mCam);
    gl::clear( Color( 0, 0, 0 ) );
    
    mBatch->draw();
    
    gl::ScopedModelMatrix scpMtx;
    gl::translate(lightPos);
    mLight->draw();
    
    if (getElapsedFrames() > 240.f) saveGif();
    
}


void Day35aApp::setupGlsl()
{
    mGlsl = gl::GlslProg::create(gl::GlslProg::Format()
                                 .vertex(CI_GLSL(150,
                                                 
                                                 //THESE MATRICES ARE SUPPLIED BY CINDER BY DEFAULT AND ARE PASSED IN AS UNIFORMS BEHIND THE SCENES
                                                 uniform mat4 ciModelViewProjection;
                                                 uniform mat4 ciModelMatrix;
                                                 uniform mat3 ciNormalMatrix;
                                                 uniform mat4 ciModelView;
                                                 
                                                 //THE POSITION AND NORMAL FOR EACH VERTEX ARE SUPPLIED AUTOMATICALLY BY CINDER
                                                 in vec4	ciPosition;
                                                 in vec3	ciNormal;
                                                 
                                                 //WE'LL BE SENDING THESE TO THE FRAG SHADER
                                                 out vec4 vColor;
                                                 out vec3 vNormal;
                                                 out vec4 vPosition;
                                                 
                                                 void main(void)
                                                 {
                                                     //SET THE VERTEX POSITION FOR THE FRAG SHADER
                                                     vPosition = ciModelView * ciPosition;
                                                     
                                                     //GET THE VERTEX NORMAL
                                                     vNormal = normalize(ciNormalMatrix * ciNormal);
                                                     
                                                     gl_Position = ciModelViewProjection * ciPosition;
                                                 }
                                                 
                                                 ))
                                 .fragment(CI_GLSL(150,
                                                   
                                                   //WE SEND THESE AS UNIFORMS TO THE FRAG SHADER AS THEY STAY THE SAME FOR EACH FRAGMENT
                                                   uniform vec3 uLightPos;
                                                   uniform vec3 uObjectColor;
                                                   uniform vec3 uLightColor;
                                                   
                                                   //THE POSITION AND NORMAL FOR EACH VERTEX COME FROM THE VERT SHADER
                                                   in vec4 vPosition;
                                                   in vec3 vNormal;
                                                   
                                                   //THE OUTPUT COLOR
                                                   out vec4 oColor;
                                                   
                                                   void main()
                                                   {
                                                       //NORMALIZE THE NORMAL AND LIGHT DIRECTION VECTORS
                                                       vec3 Normal = normalize(vNormal);
                                                       vec3 LightVec = normalize(uLightPos - vPosition.xyz);
                                                       
                                                       //GET THE ANGLE BETWEEN THE LIGHT SOURCE AND THE NORMAL TO DETERMINE THE STRENGTH OF THE LIGHTING
                                                       //THE max() FUNCTION RETURNS THE GREATER OF THE TWO ARGUMENTS, ENSURING THAT NdotL IS NEVER NEGATIVE
                                                       float NdotL = max(dot(vNormal, LightVec), 0.0);
                                                       
                                                       //GET THE DIFFUSE COMPONENT
                                                       vec3 Diffuse = vec3(NdotL);
                                                       
                                                       //SET THE AMBIENT COMPONENT
                                                       vec3 Ambient = vec3 (0.1f);
                                                       
                                                       oColor.rgb = ( Diffuse + Ambient) * uObjectColor;
                                                       oColor.a = 1.0f;
                                                       
                                                   }
                                                   
                                                   )));
    
}

void Day35aApp::saveGif()
{
        if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 361) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "35"/ ("35-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
}

CINDER_APP( Day35aApp, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setWindowSize(500,500);
               settings->setHighDensityDisplayEnabled();
           })
