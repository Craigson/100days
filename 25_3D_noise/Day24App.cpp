#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "cinder/Utilities.h"

#include "cinder/Perlin.h"

#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Day24App : public App {
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void keyPress(KeyEvent event) ;
    void update() override;
    void draw() override;
    
    void saveGif();
    
    std::vector<vec3> getPositions();
    std::vector<vec3> getNormals();
    
    CameraUi            mCamUi;
    CameraPersp         mCam;
    gl::BatchRef        mBatch;
    gl::GlslProgRef     mGlsl;
    
    std::vector<vec3>   mPositions;
    std::vector<vec3>   mNormals;
    std::vector<float>  mScales;
    
    int						mSeed;
    int						mOctaves;
    float                   mFrequency;
    float					mTime;
    Perlin					mPerlin;
    
    float angle = 0;
    float counter = 0;
    
    bool record = false;
};

void Day24App::setup()
{
    mPositions = getPositions();
    
   // getPositions();
    
    mPerlin = Perlin( );
    
    mPerlin.setSeed( clock() );
    
    mOctaves = 4;
    mTime = 200.0f;
    mFrequency = 1 / 400.0f;
    
    for (int i = 0; i < mPositions.size(); i++)
    {
        
        
        float r = Rand::randFloat(0.2,5);
        mScales.push_back(r);
        
        
    }
    
    mCam.lookAt(vec3(0,0,250), vec3(0));
    mCam.setPerspective( 35.0f, 1.0f, 1.f, 1000.0f );
    mCamUi = CameraUi(&mCam, getWindow());
    
    auto lambert = gl::ShaderDef().lambert().color();
    
    mGlsl = gl::getStockShader(lambert);
    
    auto sphere = geom::Sphere().subdivisions(32);
    
    cout << mNormals[0] << endl;
    
    mBatch = gl::Batch::create(sphere, mGlsl);
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
}

void Day24App::mouseDown( MouseEvent event )
{
    record = !record;
}

void Day24App::keyPress (KeyEvent event )
{
    if (event.getChar() == 's')
        record = true;
}

void Day24App::update()
{
    mTime += 4.0f;
    
    angle += 0.0523;
}

void Day24App::draw()
{
    
    float n = mPerlin.noise(getElapsedSeconds());
    
    gl::setMatrices(mCam);
    
    gl::clear( Color( 0.1, 0.1, 0.1 ) );
    
    gl::color(Color(.9,.9,.9));
    
    gl::rotate(angleAxis(angle, vec3(1,0,0)));
    gl::rotate(angleAxis(angle/2, vec3(0,1,0)));
    
    for (int i = 0; i < mPositions.size(); i++)
    {
        float rel = i / float(mPositions.size());
        
        gl::ScopedModelMatrix scpMtx;
        
      vec3 cheese = mPerlin.dfBm( vec3( mPositions[i].x/i,mPositions[i].y*i, mTime ) * mFrequency ) * 5.f;
        
//          vec3 cheese = mPerlin.dfBm( vec3( mPositions[i].x+= 0.05,mPositions[i].y+=0.05, mTime) * mFrequency );
        
      //  cout << cheese << endl;
        
        gl::translate(mPositions[i] * cheese);
        
        gl::scale(vec3(mScales[i]));
    
        mBatch->draw();
        
    }

    //    if (record) saveGif();
        if (getElapsedFrames() > 150) saveGif();
}

std::vector<vec3> Day24App::getPositions()
{
    
//    TriMesh sphere = geom::Sphere().subdivisions(128);
//    
//    vec3 *positions = sphere.getPositions<3>();
    
    std::vector<vec3> result;
    
    auto sphere = geom::Sphere().subdivisions(64);
    
    std::vector<vec3> mPos;
    
    auto lambert = gl::ShaderDef().lambert();
    
    auto shader = gl::getStockShader(lambert);
    
    auto scale = geom::Scale(20.,20.,20);
    
    auto btch = gl::Batch::create(sphere >> scale, shader);
    
    auto msh = btch->getVboMesh();
    
    auto mappedPosAttrib = msh->mapAttrib3f( geom::Attrib::POSITION, false );
    auto mappedNormalAttrib = msh->mapAttrib3f( geom::Attrib::NORMAL, false );
    
    for( int i = 0; i < msh->getNumVertices(); i++ ) {
        
        vec3 &pos = *mappedPosAttrib;
        vec3 &normal = *mappedNormalAttrib;
        vec3 cheese = mPerlin.dfBm( vec3( Rand::randFloat(0.5,1.5),Rand::randFloat(0.5,1.5), Rand::randFloat(10,30) ) );
        
        //pos *= cheese;
        result.push_back(pos);
        mNormals.push_back(normal);
        
        ++mappedPosAttrib;
        ++mappedNormalAttrib;
    }
    mappedPosAttrib.unmap();
    mappedNormalAttrib.unmap();
    
    return result;
}

void Day24App::saveGif()
{
    if (getElapsedFrames() % 4 == 0 && counter < 361) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "24"/ ("24-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
    counter++;
}


CINDER_APP( Day24App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(500,500);
           })
