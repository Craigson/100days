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
#define NUM_SPHERES 500

class Day71App : public App {
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
    gl::BatchRef                mSphere;
    vec3                        mLookPoint;
    std::vector<vec3>           mPositions;
    float theta = 0;
};


void Day71App::setup()
{
    mGlsl = gl::GlslProg::create(loadAsset("vertex.vert"), loadAsset("fragment.frag"));
    mLookPoint = vec3(0,0,-5);
    mCam.lookAt(vec3(0,0,0), mLookPoint);
    mCam.setPerspective(60.f, getWindowAspectRatio(), 1.f, 1000.f);
    mCamUi = CameraUi(&mCam, getWindow());
    
    mSkyBoxTex = gl::TextureCubeMap::create(loadImage(loadAsset("boxy.jpg")));
    auto cube = geom::Cube();
    mSkyBoxBatch = gl::Batch::create(cube >> geom::Scale(SKY_BOX_SIZE,SKY_BOX_SIZE,SKY_BOX_SIZE), mGlsl);
    
    auto sp = geom::Sphere();
    auto lambert = gl::ShaderDef().lambert().color();
    
    mSphere = gl::Batch::create(sp >> geom::Scale(0.4,0.4,0.4), gl::getStockShader(lambert));
    
    //SET THE LOCATION OF THE TEXTURE
    mSkyBoxBatch->getGlslProg()->uniform("uCubeMapTex", 0);
    
    mSkyBoxTex->bind();
    
    gl::enableDepth();
    
    for (int i = 0; i < NUM_SPHERES; i++)
    {
        mPositions.push_back(vec3(randFloat(-50.,50),randFloat(-50.,50.),randFloat(-50.,50)));
    }
    
     glDepthFunc(GL_LESS);
    
}

void Day71App::mouseDown( MouseEvent event )
{
}

void Day71App::update()
{
    theta += 0.0523;
   // mCam.lookAt(vec3(0), vec3(5 * cos(theta), 5 * sin(theta), 5 * cos(theta/2)));
}

void Day71App::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    gl::setMatrices(mCam);
    gl::color(Color(0.2,0.2,0.2));
    
    gl::rotate(angleAxis(theta/2, vec3(0.,1.,0.)));
    gl::rotate(angleAxis(cos(theta/2), normalize(vec3(0.5, 3.5, 1.5))));
    
    for (int i = 0; i < NUM_SPHERES; i++)
    {
        gl::ScopedMatrices push;
        gl::translate(mPositions[i]);
        mSphere->draw();
    }

    
    mSkyBoxBatch->draw();

    
   // saveGif();
}

void Day71App::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 241) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "71"/ ("71-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    
    cout << "saving image" << endl;
}


CINDER_APP( Day71App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(400, 400);
           }
)
