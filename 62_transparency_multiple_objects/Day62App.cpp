#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "cinder/Utilities.h"
#include "cinder/Easing.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define NUM_WINDOWS 50

class Day62App : public App {
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
    
    void loadImage();
    
    void setupGlsl();
    void updateLights();
    
    void saveGif();
    
    CameraPersp mCam;
    CameraUi       mUi;
    
    gl::BatchRef        mBatch, mSolidSphere;
    gl::GlslProgRef     mGlsl;
    gl::Texture2dRef    mTex;
    
    float rotation = 0;
    
    float theta = 0;
    
    vec3 mPos;
    
    std::vector<vec3> mPositions;
    std::vector<float> mRotations;
    
    //CREATE A CONTAINER FOR AUTOMATICALLY SORTING THE WINDOWS BASED ON THEIR DISTANCE FROM THE CAMERA
    std::map<float, glm::vec3> sorted;
};

void Day62App::setup()
{
    mCam.lookAt(vec3(0,0,15), vec3(0));
    mCam.setPerspective( 35.0f, 1.0f, 1.f, 1000.0f );
    
    setupGlsl();
    
    mUi = CameraUi(&mCam, getWindow());
    
    setFrameRate(30);
    
    auto scale = geom::Scale(2.,2.,2.);
    auto plane = geom::Plane();
    
    auto rot = geom::Rotate(angleAxis(toRadians(90.f),vec3(1.,0.,0.)));
    
    mBatch = gl::Batch::create(plane >> rot, mGlsl);
    
    loadImage();
    
    
    mPos = vec3(0.);
    
    //CREATE RANDOM WINDOW POSITIONS
    for (int i = 0; i < NUM_WINDOWS; i++)
    {
        mPositions.push_back(vec3(randFloat(-5,5), randFloat(-5,5), randFloat(-5,5)));
        mRotations.push_back(randFloat(0,180));
    }
}

void Day62App::mouseDown( MouseEvent event )
{
}

void Day62App::update()
{
    //INCREASE ANGLE OF ROTATION
    theta += 0.0523;
    
    //SORT THE WINDOWS BASED ON THEIR DISTANCE FROM THE CAMERA AND PLACE THEM INTO THE MAP, USING THE DISTANCE AS THE INDEX (WILL BE SORTED AUTOMATICALLY BASED ON SIZE)
    for (int i = 0; i < mPositions.size(); i++)
    {
        GLfloat distance = glm::length(mCam.getEyePoint() - mPositions[i]);
        sorted[distance] = mPositions[i];
    }
}

void Day62App::draw()
{
    gl::setMatrices(mCam);
    gl::clear( Color( 0.0, 0.0, 0.0 ) );
    
    //ROTATE THE CAMERA AROUND THE CENTRE OF THE SCENE
    mCam.setEyePoint(vec3(15 * cos(theta), 0., 15*sin(theta)));
    mCam.lookAt(vec3(0));
    
    int i = 0;
    for(std::map<float,glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
    {
        gl::ScopedMatrices push;
        gl::translate(it->second);
        //gl::rotate(angleAxis(toRadians(mRotations[i]), vec3(0.,1.,0)));
        mBatch->draw();
        i++;
    }
    
    saveGif();
  
}

void Day62App::setupGlsl()
{
    mGlsl = gl::GlslProg::create(loadAsset("vertex.vert"), loadAsset("fragment.frag"));
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    // WE USE gl::ScopedBlendAlpha() AS THE CONSTRUCTOR INTIALIZES WITH gl::ScopedBlend( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ), WHICH MEANS WE TAKE THE ALPHA OF THE SOURCE COLOUR (EG. 0.6), WHICH MEANS THE REMAINING COLOUR NEEDS TO BE 0.4 (IE 1 - SRC_ALPHA);
    gl::ScopedBlendAlpha();
    
    //ENABLE BLENDING FOR THE TRANSPARENCIES TO TAKE EFFECT
    gl::enable(GL_BLEND);
    
}

void Day62App::loadImage()
{
    auto pic = cinder::loadImage(loadAsset("window.png"));
    gl::Texture2d::Format fmt;
    
    //SET THE INTERNAL FORMAT TO RGBA SO THAT WE CAN TEST THE ALPHA VALUE IN THE FRAG SHADER
    fmt.internalFormat(GL_RGBA);
    fmt.wrap(GL_CLAMP_TO_EDGE);
    //    fmt.wrapS(GL_REPEAT);
    //    fmt.wrapT(GL_REPEAT);
    //    fmt.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    //    fmt.setMagFilter(GL_LINEAR);
    
    mTex = gl::Texture2d::create(pic, fmt);
    
    mTex->bind();
}


void Day62App::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 121) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "62"/ ("62-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}
CINDER_APP( Day62App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setWindowSize(500,500);
               settings->setHighDensityDisplayEnabled();
           })