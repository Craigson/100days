#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/ObjLoader.h"
#include "cinder/CameraUi.h"


using namespace ci;
using namespace ci::app;
using namespace std;

class Day49App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void setupGlsl();
    void loadObj(const DataSourceRef &dataSource); //THE DATASOURCE IS THE OBJ FILE FILE THAT WE'LL EXTRACT VERTEX DATA ETC FROM
    
    gl::GlslProgRef             mGlsl;
    CameraPersp                 mCam;
    CameraUi                    mCamUi;
    gl::BatchRef                mBatch;
    TriMeshRef                  mMesh; //THIS WILL STORE THE OBJ'S DATA
    
    float theta = 0;
    
};

void Day49App::setup()
{
    mCam.lookAt(vec3(0,5,20), vec3(0));
    mCam.setPerspective(45.f, getWindowAspectRatio(), 1.f, 1000.f);
    
    mCamUi = CameraUi(&mCam, getWindow());
    
    auto shader = gl::ShaderDef().color();
    
    mGlsl = cinder::gl::getStockShader(shader);
    
    
    //LOAD THE OBJ FILE THAT I'VE PLACED IN THE ASSETTS FOLDER
    loadObj(loadAsset("me_low_poly.obj"));
    
    
    gl::enableWireframe();
    

    
}

void Day49App::mouseDown( MouseEvent event )
{
}

void Day49App::update()
{
    theta += 0.0523;
}

void Day49App::draw()
{
    gl::setMatrices(mCam);
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::ScopedMatrices scp;
    gl::rotate(angleAxis(theta / 2, vec3(0.,1.,0.)));
        mBatch->draw();
}

void Day49App::loadObj(const DataSourceRef &dataSource)
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
    
    //CREATE A BATCH THAT BINDS THE GEOMETRY WITH THE STOCK SHADER (WHICH WE'LL USE TO CREATE A WIREFRAME MODEL) - NOTE, THE CO-ORDINATE AXES ARE SETUP WEIRDLY IN THE MODEL, SO WE NEED TO BAKE IN A FEW TRANSFORMATIONS TO PRESENT THE MODEL CORRECTLY (THIS SHOULD BE FIXED BE RE-EXPORTING THE OBJ WITH THE CORRECT AXES)
    mBatch = gl::Batch::create(*mMesh >> geom::Scale(30., 30.,30.) >> geom::Rotate(angleAxis(toRadians(-90.f), vec3(1.,0.,0.))) >> geom::Translate(0., -10., 0.), mGlsl);
    
}



CINDER_APP( Day49App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(500,500);
           })
