#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"

#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Day19bApp : public App {
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void keyDown (KeyEvent event) override;
    void update() override;
    void draw() override;
    
    void setColours();
    
    void saveGif();
    
    void setupGlsl();
    
    bool record = false;
    
    float counter = 0;
    
    //SETUP THE CAMERA
    CameraPersp        mCam;
    CameraUi           mCamUi;
    
    gl::VboMeshRef     mMesh;
    
    gl::GlslProgRef    mGlsl;
    gl::BatchRef       mBatch;
    
};

void Day19bApp::setup()
{
    
    setupGlsl();
    
    setFrameRate(60);
    
    //CREATE SOME GEOMETRY FOR A GRID USING A STANDARD PLANE
    auto plane = geom::Plane().size(vec2(100,100)).subdivisions(vec2(100,100));
    
    //SPECIFIY A BUFFER TO HOLD THE POSITIONS OF THE VERTICES
    vector<gl::VboMesh::Layout> bufferLayout = {
        gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::POSITION, 3),
        gl::VboMesh::Layout().usage(GL_STATIC_DRAW).attrib(geom::Attrib::COLOR, 3)
    };
    
    //CREATE A VBOMESH USING THE PLANE'S GEOMETTRY AND THE DEFINED BUFFER LAYOUT
    mMesh = gl::VboMesh::create( plane, bufferLayout);
    
    //CREATE BATCH TO COMBINE GEOMETRY AND SHADER
    mBatch = gl::Batch::create(mMesh, mGlsl);
    
    //SETUP CAMERA
    mCam.lookAt(vec3(0,0,100), vec3(0));
    mCam.setPerspective( 35.0f, 1.0f, 1.f, 1000.0f );
    mCamUi = CameraUi(&mCam, getWindow());
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    //SET THE INITIAL HSV COLOURS FOR THE GRID
    setColours();
    
    
}

void Day19bApp::keyDown(KeyEvent event)
{
    if( event.getChar() == 'w' )
        gl::setWireframeEnabled( ! gl::isWireframeEnabled() );
    
    if (event.getChar() == 's') record = !record;
}

void Day19bApp::mouseDown( MouseEvent event )
{
}

void Day19bApp::update()
{
    //    UNCOMMENT TO MANIPULATE VERTICES
    
    float offset = getElapsedFrames() / 5.;
    float counter = 0;
    
    //CREATE POINTERS ( I THINK ) TO THE ATTRIBUTES FOR COLOR AND POSITION
    auto mappedColAttrib = mMesh->mapAttrib3f( geom::Attrib::COLOR, false );
    auto mappedPosAttrib = mMesh->mapAttrib3f( geom::Attrib::POSITION, false );
    
    for( int i = 0; i < mMesh->getNumVertices(); i++ ) {
        
        float rel = counter / mMesh->getNumVertices();
        
       // vec3 &col = *mappedColAttrib;
        vec3 &pos = *mappedPosAttrib;
        
        float x1 = pos.x;
        float z1 = pos.z;
        
        float dist = distance(vec3(x1,0,z1), vec3(0,0,0) );
        
        //ADJUST THE Z VALUE OF THE VERTEX
        float z = pow(2.7182817, -dist/100) * sin( (dist * rel) - offset)* 3.0;
        
       // float inverse = lmap(z, 0.f, 1.f, 1.f, 0.f);
        
        mappedColAttrib->b = z;
        //mappedColAttrib->b = inverse;
        
        mappedPosAttrib->y = z;
        
        ++mappedPosAttrib;
        ++mappedColAttrib;
        
        counter++;
    }
    mappedPosAttrib.unmap();
    mappedColAttrib.unmap();
    
}

void Day19bApp::draw()
{
    gl::clear( Color( ));
    gl::color( Color(1,1,1));
    
    gl::setMatrices( mCam );
    
    mBatch->draw();
    
    if(record) saveGif();
};

void Day19bApp::setColours()
{
    //SET THE INITIAL HSV VALUE BASED ON HTE DISTANCE FROM THE CORNER
    auto mappedColAttrib = mMesh->mapAttrib3f( geom::Attrib::COLOR, false );
    auto mappedPosAttrib = mMesh->mapAttrib3f( geom::Attrib::POSITION, false );
    for( int i = 0; i < mMesh->getNumVertices(); i++ ) {
        
        vec3 &col = *mappedColAttrib;
        vec3 &pos = *mappedPosAttrib;
        
        float dist = distance(vec3(-50.,0.,-50.), pos);
        
        float d = lmap(dist, 0.f, 150.f, 0.f, 1.f);
        
        Color hsv = Color(CM_HSV, vec3(1., 1., 1.));
        
        col = hsv.get(CM_HSV);
        
        cout << pos << endl;
        
        ++mappedColAttrib;
      ++mappedPosAttrib;
    }
    mappedColAttrib.unmap();
    mappedPosAttrib.unmap();
}

void Day19bApp::setupGlsl()
{
    //UNCOMMENT TO USE STANDARD SHADER
    mGlsl = gl::getStockShader( gl::ShaderDef().color() );
    
    mGlsl = gl::GlslProg::create(gl::GlslProg::Format()
                                 .vertex(CI_GLSL(150,
                                                 
                                                 uniform mat4 ciModelViewProjection;
                                                 in vec4 ciPosition;
                                                 in vec3 ciColor;
                                                 out vec3 ourColor;
                                                 
                                                 void main()
                                                 {
                                                     gl_Position = ciModelViewProjection * ciPosition;
                                                     ourColor = ciColor;
                                                 }
                                                 
                                                 ))
                                 .fragment(CI_GLSL(150,
                                                   
                                                   in vec3 ourColor;
                                                   out vec4 color;
                                                   
                                                   void main()
                                                   {
                                                       //color = vec4(1.0f,1.0f,1.0f, 1.0f);
                                                       color = vec4(ourColor, 1.0f);
                                                   }
                                                   
                                                   ))
                                 );
    
}

void Day19bApp::saveGif()
{
    if (counter == 60) gl::setWireframeEnabled(true);
    
    if (getElapsedFrames() % 4 == 0 && counter < 121) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "19"/ ("19-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
        counter++;
    cout << "saving image" << endl;
}


CINDER_APP( Day19bApp, RendererGl ( RendererGl::Options().msaa( 16 ) ),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(500,500);
           })
