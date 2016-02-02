#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Day4App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void saveGif();
    
    CameraPersp mCam;
    
    gl::BatchRef mShapes [3][3];
    
    gl::BatchRef mIcos;
    
    float angle = 0;
    float gridSize = 3;
};

void Day4App::setup()
{
    auto lambert = gl::ShaderDef().lambert();
    gl::GlslProgRef shader = gl::getStockShader(lambert);
    
    auto capsule = geom::Capsule().subdivisionsAxis(10).subdivisionsHeight(10);
    mShapes[0][0] = gl::Batch::create(capsule, shader);
    
    auto sphere = geom::Sphere().subdivisions(30);
    mShapes[0][1] = gl::Batch::create(sphere, shader);
    
    auto cylinder = geom::Cylinder().subdivisionsAxis(40).subdivisionsHeight(2);
    mShapes[0][2] = gl::Batch::create(cylinder, shader);
    
    auto cube = geom::Cube();
    mShapes[1][0] = gl::Batch::create(cube, shader);
    
    auto cone = geom::Cone();
    mShapes[1][1] = gl::Batch::create(cone, shader);
    
    auto torus = geom::Torus();
    mShapes[1][2] = gl::Batch::create(torus, shader);
    
    auto helix = geom::Helix().subdivisionsAxis(20).subdivisionsHeight(10);
    mShapes[2][0] = gl::Batch::create(helix, shader);
    
    auto icosahedron = geom::Icosahedron() >> geom::Scale(0.15f);
    mShapes[2][1] = gl::Batch::create(icosahedron, shader);
    
    auto teapot = geom::Teapot() >> geom::Scale(1.5f);
    mShapes[2][2] = gl::Batch::create(teapot, shader);
    
    mIcos = gl::Batch::create(icosahedron, shader);
    
    //POSITION THE CAMERA AND SET LOOKAT POINT (ORIGIN)
    mCam.lookAt(vec3(5,5,5), vec3(0));
    
}

void Day4App::mouseDown( MouseEvent event )
{
}

void Day4App::update()
{
    angle += 0.0523;
}

void Day4App::draw()
{
    gl::clear();
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    gl::setMatrices(mCam);
    
    gridSize = fabs(sin(angle/2));
    
    //VARY THE SIZE OF THE GRID
    //gridSize = sin(angle);
    
    //CONSTRUCT THE 3 X 3 CUBE ARRAY
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            for (int k = 0; k < 10; k++)
            {
                float x = ( -0.1f + i / 2.0f) * gridSize;
                float y = ( -0.1f + j / 2.0f) * gridSize;
                float z = ( -0.1f + k / 2.0f) * gridSize;
                
                gl::ScopedModelMatrix scpModelMatrix;
                
                //APPLY TRANSLATION TO THE MATRIX TO GET THE OBJECTS POSITION
                gl::translate(x,y,z);
                
                //APPLY ROTATATION FOR EACH OBJECT
                gl::rotate(angle, vec3(0,1,0));
                
                gl::color(255,255,0);
                
                mIcos->draw();
                
            }

        }
    }
    
    saveGif();
    
    /* TESTING DIFFERENT SHAPES
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            float x = (-0.5f + i / 2.0f ) * gridSize;
            float z = (-0.5f + j / 2.0f ) * gridSize;
            
            gl::ScopedModelMatrix scpModelMatrix;
            gl::translate(x,1,z);
            gl::color(i / 2.0f, 1 - i * j, j / 2.0f);
            mShapes[i][j]->draw();
        }
    }
    */
}

void Day4App::saveGif()
{
    if (getElapsedFrames() % 4 == 0 && getElapsedFrames() < 241) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "4"/ ("4-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}
CINDER_APP( Day4App, RendererGl(RendererGl::Options().msaa( 16 ) ),
           [&]( App::Settings *settings ) {
               settings->setWindowSize( 600, 600 );
               settings->setHighDensityDisplayEnabled();
           } )
