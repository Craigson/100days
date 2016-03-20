#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/ObjLoader.h"
#include "cinder/CameraUi.h"


using namespace ci;
using namespace ci::app;
using namespace std;

#define NUM_LIGHTS 10

class Day51App : public App {
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
    
    void setupGlsl();
    void setupLights();
    
    void loadObj(const DataSourceRef &dataSource); //THE DATASOURCE IS THE OBJ FILE DEFINED IN THE RESOURCES HEADER
    
    gl::GlslProgRef             mGlsl;
    CameraPersp                 mCam;
    CameraUi                    mCamUi;
    gl::BatchRef                mBatch;
    gl::BatchRef                mLight;
    TriMeshRef                  mMesh; //THIS WILL STORE THE OBJ'S DATA
    
    
    //LIGHTS
    std::vector<vec3>       mPointLights;
    std::vector<vec3>       mPositions;
    
    float theta = 45;
    
};

void Day51App::setup()
{
    setupGlsl();
    setupLights();
    mCam.lookAt(vec3(4,8,20), vec3(0));
    mCam.setPerspective(45.f, getWindowAspectRatio(), 1.f, 100.f);
    
    mCam.setPivotDistance(20);
    
    mCamUi = CameraUi(&mCam, getWindow());
    
    loadObj(loadAsset("my_head.obj"));
    
    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        vec3 temp = vec3( 8 * cos(theta + i/NUM_LIGHTS) , -5 + i , 5 + 8  * sin(theta + i/NUM_LIGHTS) );
        
        mPointLights.push_back(temp);
    }
    
}

void Day51App::mouseDown( MouseEvent event )
{
}

void Day51App::update()
{
    mGlsl->uniform("uCamPosition", mCam.getViewMatrix() * vec4(mCam.getEyePoint(), 1.f));
    
    mGlsl->uniform("uTime", (float)getElapsedSeconds());
    
    //UPDATE THE LIGHT POSITIONS AND SET UNIFORMS
    
    for (int i = 0; i < NUM_LIGHTS; i++)
    {
//        mPointLights[i].x = 8 * cos(theta);
//        mPointLights[i].y = -5 + i;
//        mPointLights[i].z = 8  * sin(theta);
        
        mGlsl->uniform("pointLights[" + to_string(i) + "].position", vec3(mCam.getViewMatrix() * vec4(mPointLights[i], 1.f)));
        
        mGlsl->uniform("pointLights[" + to_string(i) + "].ambient", vec3(0.0f,0.f,0.0f));
        mGlsl->uniform("pointLights[" + to_string(i) + "].diffuse", vec3(0.03f,0.03f,0.03f));
        mGlsl->uniform("pointLights[" + to_string(i) + "].specular", vec3(1.f,1.f,1.f));
        mGlsl->uniform("pointLights[" + to_string(i) + "].constant", 1.f);
        mGlsl->uniform("pointLights[" + to_string(i) + "].linear", 0.014f);
        mGlsl->uniform("pointLights[" + to_string(i) + "].quadratic", 0.0007f);
        
    }
    
    theta += 0.0523;
    
}

void Day51App::draw()
{
    gl::setMatrices(mCam);
    gl::clear( Color( 0.05, 0.05, 0.05 ) );
    
    // gl::rotate(angleAxis(theta/2, vec3(0.,1.,0.)));
    {
        gl::ScopedMatrices scp;
        mBatch->draw();
    }
    
//    for (int i = 0 ; i < NUM_LIGHTS; i++)
//    {
//        gl::ScopedMatrices push;
//        gl::translate(mPointLights[i]);
//        mLight->draw();
//    }
    
}

void Day51App::setupGlsl()
{
    mGlsl = gl::GlslProg::create(loadAsset("vertex.vert"), loadAsset("fragment.frag"));
    gl::enableDepthRead();
    gl::enableDepthWrite();
}

void Day51App::loadObj(const DataSourceRef &dataSource)
{
    ObjLoader loader(dataSource);
    mMesh = TriMesh::create(loader);
    
    cout << "trimesh created" << endl;
    
    //IF THE MODEL DOESN'T CONTAIN NORMALS, CREATE THEM (I THINK...?)
    if( ! loader.getAvailableAttribs().count( geom::NORMAL ) )
    {
        cout << "recalculating" << endl;
        mMesh->recalculateNormals();
    }
    
    //CREATE A BATCH THAT BINDS THE GEOMETRY WITH THE SHADER
    mBatch = gl::Batch::create(*mMesh >> geom::Scale(30., 30.,30.) >> geom::Rotate(angleAxis(toRadians(-90.f), vec3(1.,0.,0.))) >> geom::Translate(0., -10.,0), mGlsl);
    
}

void Day51App::setupLights()
{
    auto shader = gl::ShaderDef().color();
    
    auto light = geom::Sphere().subdivisions(16);
    
    mLight = gl::Batch::create(light >> geom::Scale (0.1, 0.1,0.1), gl::getStockShader(shader));
    
}


CINDER_APP( Day51App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(500,500);
           })
