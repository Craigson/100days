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
const float RADIUS = 50.0f;

class Day27aApp : public App {
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

void Day27aApp::setup()
{
    mPositions = createVertices();
    
    // getPositions();
    
    mPerlin = Perlin( );
    
    mPerlin.setSeed( clock() );
    
    setFrameRate(30);
    
    mOctaves = 4;
    mTime = 200.0f;
    mFrequency = 1 / 400.0f;
    
    for (int i = 0; i < mPositions.size(); i++)
    {
        
        
        float r = Rand::randFloat(0.5,1.5f);
        mScales.push_back(r);
        
        
    }
    
    mCam.lookAt(vec3(0,0,250), vec3(0));
    mCam.setPerspective( 35.0f, 1.0f, 1.f, 1000.0f );
    mCamUi = CameraUi(&mCam, getWindow());
    
    auto lambert = gl::ShaderDef().color();
    
    mGlsl = gl::getStockShader(lambert);
    
    auto sphere = geom::Sphere().subdivisions(32);
    
   // cout << mNormals[0] << endl;
    
    mBatch = gl::Batch::create(sphere, mGlsl);
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
}

void Day27aApp::mouseDown( MouseEvent event )
{
    record = !record;
}

void Day27aApp::keyPress (KeyEvent event )
{
    if (event.getChar() == 's')
        record = true;
}

void Day27aApp::update()
{
    mTime += 4.0f;
    
    angle += 0.00523;
    angleA += 0.0523;
}

void Day27aApp::draw()
{
    
    float n = mPerlin.noise(getElapsedSeconds() * 5.f);
    
    gl::setMatrices(mCam);
    
    gl::clear( Color( 0.1, 0.1, 0.1 ) );
    
    auto sphere = geom::Sphere().subdivisions(32);
    
    auto lambert = gl::ShaderDef().color();
    
    auto shader = gl::getStockShader(lambert);
    
    gl::color(Color(0.1,0.1,0.1));
    
    auto scale = geom::Scale(RADIUS,RADIUS,RADIUS);
    
    auto btch = gl::Batch::create(sphere >> scale, shader);
    
    btch->draw();
    
    gl::color(Color(1.,1.0,1.0));
    
    gl::rotate(angleAxis(sin(angleA/2)/4, vec3(1,0,0)));
    gl::rotate(angleAxis(cos(angleA/4)/4.f, vec3(0,1,0)));
    
    for (int i = 0; i < mPositions.size(); i++)
    {
        
        gl::color(Color(1.,1.,1.));
        
        
        float rel = i / float(mPositions.size());
        
        gl::ScopedModelMatrix scpMtx;
        
        
        //          vec3 cheese = mPerlin.dfBm( vec3( mPositions[i].x+= 0.05,mPositions[i].y+=0.05, mTime) * mFrequency );
        
        
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
        
        /*
        gl::lineWidth(2);
        if (i > 0) gl::drawLine(mPositions[i], mPositions[i-1]);
        if (i < mPositions.size() - 16) gl::drawLine(mPositions[i], mPositions[i+15]);
         */
        
        gl::translate(mPositions[i]);
        
        gl::scale(vec3(mScales[i] + fabs(sin(mSeeds[i] + angleA))*2 ));
        
        //gl::rotate(angleAxis(angle, vec3(0,1,0)));
        

        
        mBatch->draw();
        
    }
    
  //  saveGif();
    
    //    if (record) saveGif();
    //if (getElapsedFrames() > 150) saveGif();
}


std::vector<vec3> Day27aApp::createVertices()
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
        result.push_back( vec3 ( 2*(b*d + a*c) / k , 2*(c*d - a*b) / k , (a*a + d*d - b*b - c*c) / k) );
        
    }
    
    //CHECK THE DISTANCE BETWEEN EACH NODE AND EVERY OTHER NODE, IF THE DISTANCE IS BELOW A THRESHOLD, REMOVE IT FROM THE STACK
    
    float threshold = 5.0f;
    for (int i = 0; i < result.size(); i++)
    {
        for (int j = 0; j < result.size(); j++)
        {
                if ( i != j)
                {
                    float l = length(vec3 (result[i] - result[j]));
                    if (l < threshold ) result.erase(result.begin()+j);
                }
        }
    }
    for (int i = 0; i < result.size(); i++)
    {
        float s = randFloat(0.f, 1000.f);
        mSeeds.push_back(s);
    }
    return result;
}

void Day27aApp::saveGif()
{
    if (getElapsedFrames() % 4 == 0 && counter < 121) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "27a"/ ("27a-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
   // counter++;
}


CINDER_APP( Day27aApp, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(500,500);
           })
