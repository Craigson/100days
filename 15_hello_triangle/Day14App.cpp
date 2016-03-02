#include "cinder/app/App.h"

#include "cinder/app/RendererGl.h"

#include "cinder/gl/gl.h"

#include "cinder/CameraUi.h"

#include "cinder/Utilities.h"

using namespace ci;

using namespace ci::app;

using namespace std;

const int NUM_TRIS = 1024;

class Day14App : public App {
    
public:
    
    void setup() override;
    
    void mouseDown( MouseEvent event ) override;
    
    void update() override;
    
    void draw() override;
    
    
    
    void saveGif();
    
    float angle = 0;
    
    CameraPersp     mCam;
    CameraUi		mCamUi;
    
    
    gl::VboRef           mVbo;
    
    gl::VboMeshRef       mTriangle;
    
    gl::VaoRef           mVao;
    
    gl::BatchRef         mBatch;
    
    gl::GlslProgRef     mGlsl;
    
    vector < vec3 >        mPositions;     //VERTEX POSITIONS
    vector < vec3 >        mNormals;
    vector < uint32_t >    mIndices;       //INDICES
    
};



void Day14App::setup()
{
    
    setFrameRate(30);
    
    mCam.lookAt( vec3( 0, 0, 10 ), vec3( 0 ) );
    mCamUi = CameraUi( &mCam, getWindow() );
    
    mPositions.push_back( vec3( -0.5, -0.5, 0.0 ) );
    mPositions.push_back( vec3( 0.5, -0.5, 0.0 ) );
    mPositions.push_back( vec3( 0.0, 0.5, 0.0 ) );
    
    auto normal = vec3( 0, 0, 1 );
    mNormals.push_back( normal );
    mNormals.push_back( normal );
    mNormals.push_back( normal );
    
    mIndices.push_back( 0 );
    mIndices.push_back( 2 );
    mIndices.push_back( 1 );
    
    vector < gl::VboMesh::Layout > bufferLayout = {
        gl::VboMesh::Layout().usage( GL_STATIC_DRAW ).attrib( geom::Attrib::POSITION , 3 ),
        gl::VboMesh::Layout().usage( GL_STATIC_DRAW ).attrib( geom::Attrib::NORMAL , 3 )
    };
    
    mTriangle = gl::VboMesh::create( mPositions.size(), GL_TRIANGLES, bufferLayout, mIndices.size(), GL_UNSIGNED_INT );
    
    mTriangle->bufferAttrib( geom::Attrib::POSITION, mPositions );
    mTriangle->bufferAttrib( geom::Attrib::NORMAL, mNormals );
    mTriangle->bufferIndices( mIndices.size() * sizeof( uint32_t ), mIndices.data() );
    
    auto scale = geom::Scale(3,3,3);
    mGlsl = gl::getStockShader( gl::ShaderDef().lambert().color() );
    mBatch = gl::Batch::create( mTriangle , mGlsl );
}



void Day14App::mouseDown( MouseEvent event )

{
    
}



void Day14App::update()
{
    angle += 0.0523;
}



void Day14App::draw()
{
    gl::clear( Color::gray( 0.2f ) );
    
    //gl::enableWireframe();
    
    gl::setMatrices( mCam );
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    gl::scale(vec3(3));
    
    gl::rotate(angleAxis(angle, vec3(0,1,0)));
    
        
    gl::color( ColorAf( 1.0, 1.0, 1.0, 0.5 ) );
            
    mBatch->draw();
  
    
    saveGif();
    
}

void Day14App::saveGif()
{
    if (getElapsedFrames() % 4 == 0 && getElapsedFrames() < 121) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "14"/ ("14-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}

CINDER_APP( Day14App, RendererGl( RendererGl::Options().msaa( 16 ) ),
           
           [&]( App::Settings *settings ) {
               
               settings->setHighDensityDisplayEnabled();
               
               settings->setWindowSize( 500, 500 );
               
           } )