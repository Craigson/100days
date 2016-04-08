#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define SKY_BOX_SIZE 500

class Day72App : public App {
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
    void saveGif();
    
    gl::TextureCubeMapRef       mSkyBoxTex;
    gl::GlslProgRef             mGlsl;
    CameraPersp                 mCam;
    CameraUi                    mCamUi;
    gl::BatchRef                mSkyBoxBatch;
    gl::BatchRef                mSphere, mTorus, mIcos, mCube;
    vec3                        mCamPos;
    std::vector<vec3>           mPositions;
    mat4                        mRotMatrix;
    float theta = 0;
};


void Day72App::setup()
{
    mGlsl = gl::GlslProg::create(loadAsset("vertex.vert"), loadAsset("fragment.frag"));
    mCamPos = vec3(0,0,4);
    mCam.lookAt(mCamPos, vec3(0));
    mCam.setPerspective(60.f, getWindowAspectRatio(), 1.f, 1000.f);
    mCamUi = CameraUi(&mCam, getWindow());
    
    mSkyBoxTex = gl::TextureCubeMap::create(loadImage(loadAsset("boxy.jpg")));
    auto cube = geom::Cube();
    mSkyBoxBatch = gl::Batch::create(cube >> geom::Scale(SKY_BOX_SIZE,SKY_BOX_SIZE,SKY_BOX_SIZE), mGlsl);
    
    auto sphere = geom::Sphere().subdivisions(64);
    auto c = geom::Cube();
    auto torus = geom::Torus().subdivisionsAxis(64).subdivisionsHeight(32);
    auto icos = geom::Icosahedron();
    
    mSphere = gl::Batch::create(sphere, mGlsl);
    mTorus = gl::Batch::create(torus >> geom::Scale(1.3,1.3,1.3), mGlsl);
    mIcos = gl::Batch::create(icos >> geom::Scale(0.8,0.8,0.8), mGlsl);
    mCube = gl::Batch::create(c, mGlsl);
    
    //SET THE LOCATION OF THE TEXTURE
    mSkyBoxBatch->getGlslProg()->uniform("uCubeMapTex", 0);
    
    mSkyBoxTex->bind();
    
    gl::enableDepth();
    
}

void Day72App::mouseDown( MouseEvent event )
{
}

void Day72App::update()
{
    mGlsl->uniform("uCamPos", vec3(mCam.getViewMatrix() * vec4(mCam.getEyePoint(),1.f)));
    theta += 0.0523;
}

void Day72App::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    gl::setMatrices(mCam);
    gl::color(Color(0.2,0.2,0.2));
    
    gl::rotate(angleAxis(theta/2, vec3(0.,1.,0.)));
    gl::rotate(angleAxis(cos(theta/2), normalize(vec3(1., 0., 0.4))));
    
    int i = getElapsedFrames();
    
    if ( i <= 80 ) mIcos->draw();
    else if ( i > 80 && i <= 160) mTorus->draw();
    else mCube->draw();
    
    
    saveGif();
}

void Day72App::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 241) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "72"/ ("72-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}


CINDER_APP( Day72App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(500, 500);
           }
           )
