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

const int NUM_VERTICES  = 512;
const float RADIUS = 35.0f;

class Day29aApp : public App {
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void keyPress(KeyEvent event) ;
    void update() override;
    void draw() override;
    
    
    
    void saveGif();
    
    std::vector<vec3> getPositions();
    std::vector<vec3> getNormals();
    std::vector<vec3> createVertices();
    
    
    CameraUi            mCamUi;
    CameraPersp         mCam;
    gl::BatchRef        mBatch;
    gl::GlslProgRef     mGlsl;
    
    std::vector<vec3>   mPositions;
    std::vector<vec3>   mNormals;
    std::vector<float>  mScales;
    std::vector<float>  mSeeds;
    
    int						mSeed;
    int						mOctaves;
    float                   mFrequency;
    float					mTime;
    Perlin					mPerlin;
    
    float angle = 0;
    float angleA = 0;
    
    float counter = 0;
    
    bool record = false;
};

void Day29aApp::setup()
{
    mPositions = createVertices();
    
    // getPositions();
    
    mPerlin = Perlin( );
    
    mPerlin.setSeed( clock() );
    
    setFrameRate(60);
    
    mOctaves = 4;
    mTime = 200.0f;
    mFrequency = 1 / 400.0f;
    
    for (int i = 0; i < mPositions.size(); i++)
    {
        float r = Rand::randFloat(25.f,50.f);
        mScales.push_back(r);
    }
    
    mCam.lookAt(vec3(0,0,350), vec3(0));
    mCam.setPerspective( 35.0f, 1.0f, 1.f, 1000.0f );
    mCamUi = CameraUi(&mCam, getWindow());
    
    auto lambert = gl::ShaderDef().color();
    
    mGlsl = gl::getStockShader(lambert);
    
    auto sphere = geom::Cylinder();
    
    mBatch = gl::Batch::create(sphere, mGlsl);
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    gl::clear();
    
    gl::disableAlphaBlending();
}

void Day29aApp::mouseDown( MouseEvent event )
{
    record = !record;
}

void Day29aApp::keyPress (KeyEvent event )
{
    if (event.getChar() == 's')
        record = true;
}

void Day29aApp::update()
{
    mTime += 4.0f;
    
    angle += 0.00001;
    angleA += 0.00523;
}

void Day29aApp::draw()
{
    
    float n = mPerlin.noise(getElapsedSeconds() * 5.f);
    
    gl::setMatrices(mCam);
    
    gl::clear( ColorAf( 0.1, 0.1, 0.1, 0.005 ) );
    
    auto sphere = geom::Sphere().subdivisions(32);
    
    auto lambert = gl::ShaderDef().color();
    
    auto shader = gl::getStockShader(lambert);
    
    gl::color(Color(0.1,0.1,0.1));
    
    auto scale = geom::Scale(RADIUS,RADIUS,RADIUS);
    
    auto btch = gl::Batch::create(sphere >> scale, shader);
    
    btch->draw();
    
    gl::color(Color(1.,1.0,1.0));
    
   gl::rotate(angleAxis(angleA/4, vec3(1,0,0)));
    gl::rotate(angleAxis(angleA/2.f, vec3(0,1,0)));
    
    for (int i = 0; i < mPositions.size(); i++)
    {
        
        gl::color(Color(1.,1.,1.));
        
        
        float rel = i / float(mPositions.size());
        
        gl::ScopedModelMatrix scpMtx;
        
        if (i < 136 && i > 17 )
        {
            vec3 dir = mPositions[i] - mPositions[i-15];
            //            float l = length(dir);
            normalize(dir);
            dir *= n;
            // gl::translate(mPositions[i] + dir);
        } else {
            //gl::translate(mPositions[i]);
        }
        
        
        
        gl::lineWidth(5.0f);
        
        gl::drawLine(vec3(0), vec3(mPositions[i]) * mScales[i]);
        
        mScales[i] = sin (angle * mSeeds[i]) * 50.f;
        
    }
    
   //saveGif();
    
    //    if (record) saveGif();
    //if (getElapsedFrames() > 150) saveGif();
}


std::vector<vec3> Day29aApp::createVertices()
{
    
    std::vector<vec3> result;
    
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
        vec3 dir = node - vec3(0);
        vec3 n = normalize(dir);
        
        result.push_back(n);
        
        cout << length(n) << endl;
        
    }
    
    for (int i = 0; i < result.size(); i++)
    {
        float s = randFloat(0.f, 1000.f);
        mSeeds.push_back(s);
    }
    return result;
}

void Day29aApp::saveGif()
{
    if (getElapsedFrames() % 4 == 0 && getElapsedFrames() < 801) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "29"/ ("29-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
    // counter++;
}


CINDER_APP( Day29aApp, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(400,400);
           })
