#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/ObjLoader.h"
#include "cinder/CameraUi.h"
#include "cinder/Utilities.h"


using namespace ci;
using namespace ci::app;
using namespace std;

#define NUM_LIGHTS 10

class Day53aApp : public App {
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
    
    void setupGlsl();
    void setupLights();
    void saveGif();
    
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
    
    float theta = 0;
    
};

void Day53aApp::setup()
{
    setupGlsl();
    setupLights();
    mCam.lookAt(vec3(4,8,20), vec3(0));
    mCam.setPerspective(45.f, getWindowAspectRatio(), 1.f, 100.f);
    
    mCam.setPivotDistance(20);
    
    mCamUi = CameraUi(&mCam, getWindow());
    
    loadObj(loadAsset("hand.obj"));
    
    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        mPointLights.push_back(vec3(0));
    }
    
}

void Day53aApp::mouseDown( MouseEvent event )
{
}

void Day53aApp::update()
{
    
    mGlsl->uniform("uLightPosition", mCam.getViewMatrix() * vec4(mPointLights[0], 1.f));
    
    //UPDATE THE LIGHT POSITIONS AND SET UNIFORMS
    

        mPointLights[0].x = 8 * cos(theta);
       mPointLights[0].y = 2*cos(theta);
        mPointLights[0].z = 8  * sin(theta);

    
    theta += 0.0523;
    
}

void Day53aApp::draw()
{
    gl::setMatrices(mCam);
    gl::clear( Color( 0.95, 0.95, 0.95 ) );
    

    
    {
        gl::ScopedMatrices scp;
        //gl::rotate(angleAxis(theta/2, vec3(0.,1.,0.)));
        mBatch->draw();
    }
    

    {
        gl::ScopedMatrices push;
        gl::translate(mPointLights[0]);
        mLight->draw();
    }
    
    if (getElapsedFrames() > 240) saveGif();
    
}

void Day53aApp::setupGlsl()
{
    mGlsl = gl::GlslProg::create(loadAsset("vertex.vert"), loadAsset("frag_shader.frag"));
    gl::enableDepthRead();
    gl::enableDepthWrite();
}

void Day53aApp::loadObj(const DataSourceRef &dataSource)
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
    mBatch = gl::Batch::create(*mMesh >> geom::Scale(20., 20.,20.) >> geom::Rotate(angleAxis(toRadians(-90.f), vec3(1.,0.,0.))), mGlsl);
    
}

void Day53aApp::setupLights()
{
    auto shader = gl::ShaderDef().color();
    
    auto light = geom::Sphere().subdivisions(16);
    
    mLight = gl::Batch::create(light >> geom::Scale (0.1, 0.1,0.1) , gl::getStockShader(shader));
    
}


void Day53aApp::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 361) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "53"/ ("53-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
}


CINDER_APP( Day53aApp, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(500,500);
           })
