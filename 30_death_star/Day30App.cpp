#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Rand.h"
#include "cinder/CameraUi.h"

#include "cinder/Utilities.h"

const int NUM_VERTICES = 4096;

const float RADIUS = 30.f;

using namespace ci;
using namespace ci::app;
using namespace std;

class Day30App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void saveGif();
    
    std::vector<vec3>   createVertices();
    
    std::vector<vec3>   vertices;
    std::vector<vec3>   scales;
    
    gl::BatchRef        mBatch;
    
    CameraPersp         mCam;
    CameraUi            mCamUi;
    
    float angleC = 0.f;
};

void Day30App::setup()
{
    
    setFrameRate(30);
    vertices = createVertices();
    mCam.lookAt(vec3(0,0,150), vec3(0));
    mCam.setPerspective(45.f, getWindowAspectRatio(), 1.f, 1000.f);
    
    mCamUi = CameraUi(&mCam, getWindow());
    
    cout << vertices[0] << endl;
    
    auto scale = geom::Scale(2,2,2);
    auto lambert = gl::ShaderDef().lambert().color();
    mBatch = gl::Batch::create(geom::Cube() >> scale , gl::getStockShader(lambert));
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    gl::clear();
}

void Day30App::mouseDown( MouseEvent event )
{
}

void Day30App::update()
{
    angleC += 0.0523;
}

void Day30App::draw()
{
    gl::setMatrices(mCam);
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::color(Color(0,0,0));
    auto cube = geom::Sphere();
    
    auto scale = geom::Scale(30,30,30);
    
    auto btch = gl::Batch::create(cube >> scale, gl::getStockShader(gl::ShaderDef().color()));
    
    //btch->draw();
    
    
    
    gl::pushModelMatrix();
    
//    gl::rotate(angleAxis(toRadians(90.f), vec3(0,1,0)));
//    
//    gl::rotate(angleAxis(angleC, vec3(0,0,1)));
    
    for (int i = 0; i < vertices.size(); i++)
    {
        

        gl::color(ColorAf(1.,1.,1.,1.));
        float rel = i / vertices.size();
        
      //  gl::drawLine(vec3(0), vertices[i]);
        
        gl::ScopedModelMatrix scpMtx;
        vec3 n = normalize(vertices[i]);
        
       // gl::rotate(angleAxis(angleA, vec3(0,1,0)));
        
        gl::translate(vertices[i]);
        
        float angleA = atan2f(n.z,n.x);
        gl::rotate(angleAxis (angleA * sin(angleC * rel*i), vec3(0,1,0)));
        
        float angleB = atan2f(n.y, n.x);
        gl::rotate(angleAxis(angleB, vec3(0,0,1)));
        
        gl::scale(scales[i]);
        mBatch->draw();

       // cout << vertices[i] << endl;
    }
    gl::popModelMatrix();
  //  saveGif();
}

std::vector<vec3> Day30App::createVertices()
{
    std::vector<vec3> result;
    
    //GENERATE RANDOMLY DISTRIBUTED POINTS ON THE SURFACE OF A SPHERE
    for (int i = 0; i < NUM_VERTICES; i++)
    {
        
        float a=0, b=0, c=0, d=0, k=99;
        while (k >= 1.0)
        {
            a = randFloat (-1.0, 1.0);
            b = randFloat (-1.0, 1.0);
            c = randFloat (-1.0, 1.0);
            d = randFloat (-1.0, 1.0);
            k = a*a +b*b +c*c +d*d;
        }
        k = k / RADIUS;
        
        vec3 node = vec3 ( 2*(b*d + a*c) / k , 2*(c*d - a*b) / k , (a*a + d*d - b*b - c*c) / k);
        
        result.push_back(node);
        
        scales.push_back(vec3(randFloat(0, 5),randFloat(0, 5),randFloat(0, 5)));

    }
    return result;
}

void Day30App::saveGif()
{
    
    if (getElapsedFrames() % 4 == 0 && getElapsedFrames() < 681) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "30"/ ("30-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
}

CINDER_APP( Day30App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(500,500);
           })
