#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const int NUM_TEXS = 16;

class Day6App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void saveGif();
    
    float angle = 0;
    
    int tally = 0;
    
    
    CameraPersp         mCam;
    
    gl::BatchRef        mSphere;
    
    gl::GlslProgRef     mGlsl;
    
    gl::Texture2dRef    mTextures [NUM_TEXS];
};

void Day6App::setup()
{
    
    setFrameRate(30);
    
    mCam.lookAt(vec3(3,2,4), vec3(0));
    
    //CREATE A COUNTER FOR LOOPING THROUGH THE IMAGE FILE NAMES
    int counter = 4;
    
    //LOAD THE IMAGES INTO TEXTURES
    for (int i = 0; i < NUM_TEXS; i++)
    {
        string current = toString(counter);
        auto img = loadImage(loadAsset(current + ".png"));
        mTextures[i] = gl::Texture2d::create(img);
        counter += 4;
    }

    //MAKE mTex THE ACTIVE TEXTURE
    mTextures[0]->bind();
    
    auto shader = gl::ShaderDef().texture().lambert();
    
    mGlsl = gl::getStockShader(shader);
    
    auto scale = geom::Scale(2.f,2.f,2.f);
    
    auto cube = geom::Cube();
    
    mSphere = gl::Batch::create(cube >> scale, mGlsl);
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
}

void Day6App::mouseDown( MouseEvent event )
{
}

void Day6App::update()
{
    
    //RESET THE TEXTURES TO START THE ANIMATION LOOPING AGAIN
    if (tally > 15) tally = 0;
    
    //BIND A NEW TEXTURE EVERY 2ND FRAME
    if (getElapsedFrames() % 2 == 0 )
    {
        mTextures[tally]->bind();
        tally++;
    }
    
    //INCREASE ANGLE OF ROTATION
    angle += 0.00523;
}

void Day6App::draw()
{
    gl::clear(Color(0.05f,0.05f,0.05f));
    
    gl::setMatrices(mCam);
    
    //ROTATE THE MATRIX AND DRAW THE CUBE
    gl::pushModelMatrix();
    gl::rotate(angle, vec3(0,1,0));
    gl::rotate(angle, vec3(1,0,0));
    mSphere->draw();
    gl::popModelMatrix();
    
    saveGif();
}

void Day6App::saveGif()
{
    if (getElapsedFrames() % 4 == 0 && getElapsedFrames() < 481) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "6"/ ("6-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}

CINDER_APP( Day6App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setWindowSize(600,600);
               settings->setHighDensityDisplayEnabled();
           })
