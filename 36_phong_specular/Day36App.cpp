#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/CameraUi.h"

#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Day36App : public App {
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
    vec3                eyePos;
    
    float               theta;
};

void Day36App::setup()
{
    mCam.lookAt(vec3(0,0,5), vec3(0));
    mCam.setPerspective(45.f, getWindowAspectRatio(), 1.f, 1000.f);
    
    lightPos = vec3(1.5f, 1.5f, 1.5f);
    
    theta = 0.0;
    
    setupGlsl();
    
    mCamUi = CameraUi(&mCam, getWindow());
    
    auto cube = geom::Icosahedron();
    
    mBatch = gl::Batch::create(cube, mGlsl);
    
    auto sphere = geom::Sphere();
    auto scale = geom::Scale(0.1f,0.1f,0.1f);
    auto shader = gl::ShaderDef().color();
    auto rotate = geom::Rotate(angleAxis(2.1f, vec3(1.0,0.,0.f)));
    mLight = gl::Batch::create(sphere >> scale >> rotate, gl::getStockShader(shader));
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
}

void Day36App::mouseDown( MouseEvent event )
{
}

void Day36App::update()
{
    lightPos.x = 1.5f * cos(theta);
    lightPos.y = 1.5f * sin(theta);
    lightPos.z = 1.5f * cos(sin(theta)*2.f);
    theta += 0.0523;
    
    eyePos = mCam.getEyePoint();
    
    cout << eyePos << endl;
    
    mGlsl->uniform("uLightColor", vec3(1.f,1.f,1.f));
    mGlsl->uniform("uObjectColor", vec3(1.f, 0.1f, 0.1f));
    mGlsl->uniform("uLightPos", lightPos);
    mGlsl->uniform("uEyePoint", eyePos);
    
    
    //cout << lightPos << endl;
    
}

void Day36App::draw()
{
    gl::setMatrices(mCam);
    gl::clear( Color( 0.2, 0.2, 0.2 ) );
    
    mBatch->draw();
    
    gl::ScopedModelMatrix scpMtx;
    gl::translate(lightPos);
    mLight->draw();
    
   // if (getElapsedFrames() > 240.f) saveGif();
    
}


void Day36App::setupGlsl()
{
    mGlsl = gl::GlslProg::create(gl::GlslProg::Format()
                                 .vertex(CI_GLSL(150,
                                                 
                                                 //THESE MATRICES ARE SUPPLIED BY CINDER BY DEFAULT AND ARE PASSED IN AS UNIFORMS BEHIND THE SCENES
                                                 uniform mat4 ciModelViewProjection;
                                                 uniform mat4 ciModelMatrix;
                                                 uniform mat3 ciNormalMatrix;
                                                 uniform mat4 ciModelView;
                                                 uniform mat4 ciViewMatrix;
                                                 
                                                 //THE POSITION AND NORMAL FOR EACH VERTEX ARE SUPPLIED AUTOMATICALLY BY CINDER
                                                 in vec4	ciPosition;
                                                 in vec3	ciNormal;
                                                 
                                                 
                                                 //CREATE A STRUCT TO SEND DATA TO THE FRAG SHADER
                                                 
                                                   out  vec3 vNormal;
                                                   out  vec4 vPosition;
                                                 

                                                 
                                                 void main(void)
                                                 {
                                                     //SET THE VERTEX POSITION FOR THE FRAG SHADER
                                                     vPosition = ciModelView * ciPosition;
                                                     
                                                     //GET THE VERTEX NORMAL
                                                     vNormal = ciNormalMatrix * ciNormal;
                                                    
                                                     gl_Position = ciModelViewProjection * ciPosition;
                                                 }
                                                 
                                                 ))
                                 .fragment(CI_GLSL(150,
                                                   
                                                 //THE POSITION AND NORMAL FOR EACH VERTEX COME FROM THE VERT SHADER
                                                   
                                                   in    vec3 vNormal;
                                                   in    vec4 vPosition;
                                                   
                                                   
                                                   //WE SEND THESE AS UNIFORMS TO THE FRAG SHADER AS THEY STAY THE SAME FOR EACH FRAGMENT
                                                   uniform vec3 uObjectColor;
                                                   uniform vec3 uLightColor;
                                                   uniform vec3 uLightPos;
                                                   
                                                   //ADD A UNIFORM FOR THE CAMERA'S EYE POINT TO WORK OUT THE SPECULAR ELEMENT
                                                   uniform vec3 uEyePoint;
                                                   
                                                   //THE OUTPUT COLOR
                                                   out vec4 oColor;
                                                   
                                                   void main()
                                                   {
                                            
                                                       
                                                       //NORMALIZE THE NORMAL AND LIGHT DIRECTION VECTORS
                                                       vec3 Normal = normalize(vNormal);
                                                       vec3 LightVec = normalize( uLightPos - vPosition.xyz);
                                                       
                                                       //DEFINE THE SPECULAR INTENSITY AND CALCULATE THE VIEW DIRECTION AND REFLECTION DIRECTION
                                                       float specularStrength = 0.5f;
                                                       vec3 viewDir = normalize(uEyePoint - vPosition.xyz);
                                                       vec3 reflectDir = reflect(-LightVec, Normal);
                                                       
                                                       //CALCULATE THE SPECULAR COMPONENT
                                                       int shininess = 32;
                                                       float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
                                                       vec3 Specular = specularStrength * spec * uLightColor;
                                                       
                                                       //GET THE ANGLE BETWEEN THE LIGHT SOURCE AND THE NORMAL TO DETERMINE THE STRENGTH OF THE LIGHTING
                                                       //THE max() FUNCTION RETURNS THE GREATER OF THE TWO ARGUMENTS, ENSURING THAT NdotL IS NEVER NEGATIVE
                                                       float NdotL = max(dot(Normal, LightVec), 0.0);
                                                       
                                                       //GET THE DIFFUSE COMPONENT
                                                       vec3 Diffuse = vec3(NdotL);
                                                       
                                                       //SET THE AMBIENT COMPONENT
                                                       vec3 Ambient = vec3 (0.1f);
                                                       
                                                       oColor.rgb = ( Diffuse + Ambient + Specular) * uObjectColor;
                                                       oColor.a = 1.0f;
                                                       
                                                   }
                                                   
                                                   )));
    
}

void Day36App::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 361) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "35"/ ("35-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
}

CINDER_APP( Day36App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setWindowSize(500,500);
               settings->setHighDensityDisplayEnabled();
           })
