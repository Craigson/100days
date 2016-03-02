#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Utilities.h"
#include "cinder/Easing.h"
#include "cinder/CameraUi.h"

const int NUM_IMAGES = 6;

using namespace ci;
using namespace ci::app;
using namespace std;

class Day22App : public App {
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
    
    void loadImages();
    
    void setupGlsl();
    
    void saveGif();
    
    CameraPersp mCam;
    CameraUi       mUi;
    
    gl::BatchRef        mBatch;
    gl::GlslProgRef     mGlsl;
    //gl::Texture2dRef    mTextures [NUM_IMAGES];
    
    std::vector<gl::Texture2dRef> mTextures;
    
    float angle = 0;
    
    int tally = 0;
    
    bool turnComplete = false;
    bool reverse = false;
    
    //TIME IN SECONDS FOR ONE SLICE TO ROTATE
    const float rotationTime = 1.5f;
    
    float rotation = 0;
};

void Day22App::setup()
{
    mCam.lookAt(vec3(0,0,10), vec3(0));
    mCam.setPerspective( 35.0f, 1.0f, 1.f, 1000.0f );
    
    setupGlsl();
    
    mUi = CameraUi(&mCam, getWindow());

    setFrameRate(30);
    
    auto scale = geom::Scale(2.,2.,2.);
    auto sphere = geom::Sphere().subdivisions(64);
    //auto rotate = geom::Rotate(angleAxis(210.f, vec3(0,1,0)));
    mBatch = gl::Batch::create(sphere >> scale, mGlsl);
    
    loadImages();
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
}

void Day22App::mouseDown( MouseEvent event )
{
}

void Day22App::update()
{
    if (turnComplete)
    {
        //RESET THE TEXTURES TO START THE ANIMATION LOOPING AGAIN
        if (tally > 4){
            reverse = true;
        } else if (tally < 1){
            reverse = false;
        }
        
        //BIND A NEW TEXTURE EVERY 2ND FRAME
        if (getElapsedFrames() % 4 == 0 )
        {
            
            
            if (!reverse) tally++;
            else tally--;
            cout << tally << endl;
            mTextures[tally]->bind();
        }
    }
    
    //INCREASE ANGLE OF ROTATION
    angle += 0.00523;
}

void Day22App::draw()
{
    
    //EASE FUNCTION ON ROTATION, THEN CONVERT TO RADIANS
    float angleA = easeInOutQuint(angle) * M_PI;
    
    if (angleA > M_PI){
        angleA = M_PI;
        turnComplete = true;
    }
    gl::clear( Color( 0.2, 0.2, 0.2 ) );
    
    ColorAf color(1.,.9,.2);
    
    gl::color(Color(1.,.9,.2));
    
    gl::setMatrices(mCam);
    
    gl::ScopedBlend scpBlend(false);
    
    gl::rotate(angleA, vec3(0,1,0));
    
    cout << angleA << endl;
    
    //ROTATE THE MATRIX AND DRAW THE CUBE
    gl::pushModelMatrix();

//    gl::rotate(angle, vec3(1,0,0));
    mBatch->draw();
    gl::popModelMatrix();
    
    if(getElapsedFrames() > 90) saveGif();
    
    
}

void Day22App::setupGlsl()
{
    
    
    auto shader = gl::ShaderDef().lambert().color().texture();
    mGlsl = gl::getStockShader(shader);
    
}

void Day22App::loadImages()
{
    for (int i = 1; i < NUM_IMAGES+1; i++)
    {
        gl::Texture2d::Format fmt;
        
        fmt.setMinFilter(GL_NEAREST);
        fmt.setMagFilter(GL_LINEAR);
        
        string num = toString(i);
        auto img = loadImage(loadAsset(num + ".png"));
        auto tex = gl::Texture2d::create(img, fmt);
        
        mTextures.push_back(tex);
    }
    mTextures[0] ->bind();
}

void Day22App::saveGif()
{
    if (getElapsedFrames() % 4 == 0 && getElapsedFrames() < 241) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "22"/ ("22-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}
CINDER_APP( Day22App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setWindowSize(500,500);
               settings->setHighDensityDisplayEnabled();
           })
