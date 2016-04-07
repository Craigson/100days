#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"

#define NUM_DICE 100

using namespace ci;
using namespace ci::app;
using namespace std;

class Day70App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    void saveGif();
    
    gl::TextureCubeMapRef           mDice;
    gl::TextureData                 mTexData;
    std::vector<DataSourceRef>      mFaces;
    ImageSourceRef                  mImages[6]; //ARRAY OF IMAGE SOURCES HOLDS TEXTURES FOR EACH FACE OF THE CUBE
    gl::GlslProgRef                 mGlsl;
    gl::BatchRef                    mBatch;
    CameraPersp                     mCam;
    CameraUi                        mCamUi;
    
    std::vector<vec3>               mPositions;
    std::vector<float>              mOffsets;
    std::vector<vec3>               mAxes;
    
    float theta = 0;
    
};

void Day70App::setup()
{
    
    mGlsl = gl::GlslProg::create(loadAsset("vertex.vert"), loadAsset("fragment.frag"));
    mCam.lookAt(vec3(0,0,5), vec3(0));
    mCam.setPerspective(60.f, getWindowAspectRatio(), 1.f, 1000.f);
    mCamUi = CameraUi(&mCam, getWindow());
    
    auto cube = geom::Cube();
    mBatch = gl::Batch::create(cube >> geom::Scale(.4,.4,.4), mGlsl);
    mBatch->getGlslProg()->uniform( "uCubeMap", 0 );

    //LOAD THE 6 TEXTURES THAT WILL MAKE UP THE FACES OF THE DICE
    mImages[0] = (loadImage(loadAsset("one.jpg")));
    mImages[1] = (loadImage(loadAsset("two.jpg")));
    mImages[2] = (loadImage(loadAsset("three.jpg")));
    mImages[3] = (loadImage(loadAsset("four.jpg")));
    mImages[4] = (loadImage(loadAsset("five.jpg")));
    mImages[5] = (loadImage(loadAsset("six.jpg")));
    
    //SPECIFY WRAPPING AND FILTERING METHODS WITH A FORMAT
    gl::TextureCubeMap::Format fmt;
    fmt.mipmap();
    fmt.setMagFilter(GL_LINEAR);
    fmt.setMinFilter(GL_LINEAR);
    fmt.wrap(GL_CLAMP_TO_EDGE);
    
    //CREATE THE TextureCubeMap, PASSING const ImageSourceRef images[6] AND Format AS ARGUMENTS
    mDice = gl::TextureCubeMap::create(mImages, fmt);
    
    mDice->bind();
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    for (int i = 0; i < NUM_DICE; i++)
    {
        mPositions.push_back(vec3(randFloat(-5.,5.),randFloat(-3.,3.),randFloat(-3.,3.)));
        mAxes.push_back(vec3(randFloat(-5.,5.),randFloat(-5.,5.),randFloat(-5.,5.)));
        mOffsets.push_back(randFloat(0., 1000.));
    }
    
}

void Day70App::mouseDown( MouseEvent event )
{
}

void Day70App::update()
{
    for (int i = 0; i < NUM_DICE; i++)
    {
        if (mPositions[i].y < -3) mPositions[i].y = 3;
        mPositions[i].y -= 0.05;
    }
    
    theta += 0.0523;
}

void Day70App::draw()
{
    gl::setMatrices(mCam);
	gl::clear( Color( 0.05, 0.05, 0.05 ) );
    
    //BIND THE CUBE MAP TEXTURE
    for (int i = 0; i < NUM_DICE; i++)
    {
        gl::ScopedMatrices push;
        gl::translate(mPositions[i]);
        gl::rotate(angleAxis(mOffsets[i] + theta, normalize(mAxes[i])));
        mBatch->draw();
    }
    
    
 saveGif();
}

void Day70App::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 181) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "70"/ ("70-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}


CINDER_APP( Day70App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings* settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(500, 500);
           })
