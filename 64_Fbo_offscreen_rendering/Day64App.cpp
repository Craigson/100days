#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Perlin.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define NUM_INITIAL_PARTICLES 200
#define FBO_DIMENSIONS 512
//------------------------------------ Particle ------------------------------------------

class Particle {
public:
    Particle();
    Particle(ci::vec3 pos, ci::vec3 vel, bool followed);
    Particle(ci::vec3 pos, ci::vec3 vel, bool followed, int index_);
    ~Particle();
    void pullToCenter(const ci::vec3 &center);
    void update(bool flatten);
    virtual void draw();
    void drawTail();
    void limitSpeed();
    void addNeighborPos(ci::vec3 pos);
    void drawConnections();
    
    void saveGif();
    
    vec3 getPos();
    
    ci::vec3	mPos;
    ci::vec3	mTailPos;
    ci::vec3	mVel;
    ci::vec3	mVelNormal;
    ci::vec3	mAcc;
    
    ci::vec3	mNeighborPos;
    int			mNumNeighbors;
    
    ci::Color	mColor;
    
    float		mDecay;
    float		mRadius;
    float		mLength;
    float		mMaxSpeed, mMaxSpeedSqrd;
    float		mMinSpeed, mMinSpeedSqrd;
    float		mFear;
    float		mCrowdFactor;
    
    bool		mIsDead;
    bool		mFollowed;
    
    
    
    int index;
};

Particle::Particle()
{
}

Particle::~Particle(){}

Particle::Particle(vec3 pos, vec3 vel, bool followed)
{
    mPos = pos;
    //mTailPos = pos;
    mVel = vel;
    mVelNormal = vec3(0., 1., 0);
    mAcc = vec3(0.);
    
    mNeighborPos = vec3(0.);
    mNumNeighbors = 0;
    mMaxSpeed = Rand::randFloat(0.5f, 2.0f);
    mMaxSpeedSqrd = mMaxSpeed * mMaxSpeed;
    mMinSpeed = Rand::randFloat(.1f, 0.5f);
    mMinSpeedSqrd = mMinSpeed * mMinSpeed;
    
    mColor = ColorA(1.0f, 1.0f, 1.0f, 1.0f);
    
    mDecay = 0.95f;
    mRadius = 1.0f;
    mLength = 5.0f;
    mFear = .8f;
    mCrowdFactor = 1.0f;
    
    mIsDead = false;
    mFollowed = followed;
    
    index = 99;
}

Particle::Particle(vec3 pos, vec3 vel, bool followed, int index_)
{
    mPos = pos;
    mTailPos = pos;
    mVel = vel;
    mVelNormal = vec3(0., 1., 0);
    mAcc = vec3(0.);
    
    mNeighborPos = vec3(0.);
    mNumNeighbors = 0;
    mMaxSpeed = Rand::randFloat(0.5f, 2.0f);
    mMaxSpeedSqrd = mMaxSpeed * mMaxSpeed;
    mMinSpeed = Rand::randFloat(.1f, 0.5f);
    mMinSpeedSqrd = mMinSpeed * mMinSpeed;
    
    mColor = ColorA(1.0f, 1.0f, 1.0f, 1.0f);
    
    mDecay = 0.95f;
    mRadius = 1.0f;
    mLength = 5.0f;
    mFear = .8f;
    mCrowdFactor = 1.0f;
    
    mIsDead = false;
    mFollowed = followed;
    
    index = index_;
}


void Particle::pullToCenter(const vec3 &center)
{
    vec3 dirToCenter = mPos - center;
    //float distToCenter = dirToCenter.length();
    float distToCenter = length(dirToCenter);
    float distThresh = .0f;
    
    normalize(dirToCenter);
    
    if (distToCenter > distThresh){
        //dirToCenter.normalize();
        normalize(dirToCenter);
        float pullStrength = 0.00025f;
        //mVel -= dirToCenter * ((distToCenter - distThresh) * pullStrength);
        dirToCenter *= pullStrength;
        mVel *= -dirToCenter;
    }
}

vec3 Particle::getPos()
{
    return mPos;
}


void Particle::update(bool flatten)
{
    
    
    mCrowdFactor -= (mCrowdFactor - (1.0f - mNumNeighbors * 0.02f)) * 0.1f;
    mCrowdFactor = constrain(mCrowdFactor, 0.5f, 1.0f);
    
    mFear -= (mFear - 0.0f) * 0.5f;
    
    if (flatten)
        mAcc.z = 0.0f;
    
    mVel += mAcc;
    //mVelNormal = mVel.normalized();
    mVelNormal = normalize(mVel);
    
    limitSpeed();
    
    
    mPos += mVel;
    if (flatten)
        mPos.z = 0.0f;
    
    mTailPos = mPos - (mVelNormal * mLength);
    mVel *= mDecay;
    
    float c = math<float>::min(mNumNeighbors / 50.0f, 1.0f);
    mColor = ColorA(CM_HSV, 1.0f - c, c, c * 0.5f + 0.5f, 1.0f);
    
    mAcc = vec3(0.);
    mNeighborPos = vec3(0.);
    mNumNeighbors = 0;
    
    //if (mPos.x > getWindowWidth() || mPos.x < 0) mVel.x *= -1;
    //if (mPos.y > getWindowHeight() || mPos.y < 0) mVel.y *= -1;
}

void Particle::limitSpeed()
{
    float maxSpeed = mMaxSpeed + mCrowdFactor;
    float maxSpeedSqrd = maxSpeed * maxSpeed;
    
    //float vLengthSqrd = mVel.lengthSquared();
    float vLengthSqrd = length2(mVel);
    if (vLengthSqrd > maxSpeedSqrd){
        mVel = mVelNormal * maxSpeed;
        
    }
    else if (vLengthSqrd < mMinSpeedSqrd){
        mVel = mVelNormal * mMinSpeed;
    }
    mVel *= (1.0 + mFear);
}

void Particle::draw()
{
    gl::color(ColorAf(1.0f, 1.0f, 1.0f, 1.0f));
    gl::drawSphere(mPos, 1.5);
}


void Particle::addNeighborPos(vec3 pos)
{
    mNeighborPos += pos;
    mNumNeighbors++;
}


//--------------------------------------- D A N C E R  C L A S S ---------------------------------------------------


class Dancer : public Particle {
public:
    
    using Particle::Particle;
    Dancer();
    Dancer(vec3 pos, vec3 vel, bool followed, int index_);
    void draw(int index_);
    void update(vec3 pos_, int w_, int h_, int l_, CameraPersp *mCam);
    
    vec3 prevPos;
    
};

/*
 Dancer::Dancer(vec2 loc_) : Particle(loc_){
	location_ = loc_;
 };
 */

Dancer::Dancer(vec3 pos, vec3 vel, bool followed, int index_)
{
    mPos = pos;
    prevPos = vec3(0.);
    mTailPos = pos;
    mVel = vel;
    mVelNormal = vec3(0., 1., 0);
    mAcc = vec3(0.);
    
    mNeighborPos = vec3(0.);
    mNumNeighbors = 0;
    mMaxSpeed = Rand::randFloat(0.5f, 2.0f);
    mMaxSpeedSqrd = mMaxSpeed * mMaxSpeed;
    mMinSpeed = Rand::randFloat(.1f, 0.5f);
    mMinSpeedSqrd = mMinSpeed * mMinSpeed;
    
    mColor = ColorA(1.0f, 1.0f, 1.0f, 1.0f);
    
    mDecay = 0.95f;
    mRadius = 1.0f;
    mLength = 5.0f;
    mFear = .8f;
    mCrowdFactor = 1.0f;
    
    mIsDead = false;
    mFollowed = followed;
    
    index = index_;
}

void Dancer::draw(int index_) {
    
    switch (index_){
        case 0:
            gl::color(ColorAf(1.0f, 0.f, 0.f, 1.f));
            break;
        case 1:
            gl::color(ColorAf(1.0f, 0.f, 0.f, 1.f));
            break;
        case 2:
            gl::color(ColorAf(1.0f, 0.f, 0.f, 1.f));
            break;
        case 3:
            gl::color(ColorAf(1.0f, 0.f, 0.f, 1.f));
            break;
        case 4:
            gl::color(ColorAf(1.0f, 0.f, 0.f, 1.f));
            break;
        case 5:
            gl::color(ColorAf(1.0f, 0.f, 0.f, 1.f));
            break;
        default:
            gl::color(Colorf::white());
            break;
            
            
    }
    
    gl::drawSphere(mPos, 2.0);
    
}


void Dancer::update(vec3 pos_, int w_, int h_, int l_, CameraPersp *mCam)
{
    
    if (mPos.x < -w_ / 4 || mPos.x > w_ / 4) mVel.x *= -1;
    if (mPos.y < -h_ / 4 || mPos.y > h_ / 4) mVel.y *= -1;
    if (mPos.z < -l_ / 4 || mPos.z > l_ / 4) mVel.z *= -1;
    //mCrowdFactor -= (mCrowdFactor - (1.0f - mNumNeighbors * 0.02f)) * 0.1f;
    //mCrowdFactor = constrain(mCrowdFactor, 0.5f, 1.0f);
    
    
    mPos.x = lmap(pos_.x, 0.f, 1.f, -50.f, 50.f);
    mPos.y = lmap(pos_.y, 0.f, 1.f, -30.f, 30.f);
    mPos.z = lmap(pos_.z, 0.f, 1.f, 300.f, 400.f);
    
    
    mAcc.x = lmap(pos_.x, 0.f, 1.f, -0.002f, 0.002f);
    mAcc.y = lmap(pos_.y, 0.f, 1.f, -0.002f, 0.002f);
    //mAcc.y = 0.f;
    mAcc.z = 0.f;
    
    mVel += mAcc;
    
    mVel.x = constrain(mVel.x, -100.f, 100.f);
    //mVel.y = constrain(mVey.y, -50., 50.f)
    //vec3 temp = mCam->getEyePoint();
    //mCam->setEyePoint(vec3(mPos.x, mPos.y, mPos.z));
    
    mPos += mVel;
    
    //limitSpeed();
    
    
    //mFear -= (mFear - 0.0f) * 0.5f;
    vec3 currentPos = pos_ * (float)(w_/2);
    
    
    vec3 dirOfTravel = currentPos - prevPos;
    
    
    float l = length2(dirOfTravel);
    normalize(dirOfTravel);
    
    mVel += mAcc;
    //mVelNormal = mVel.normalized();
    //mVelNormal = normalize(mVel);
    
    mAcc = dirOfTravel * 0.1f;
    
    //mAcc.z = 0.f;
    //mAcc.y = 0.f;
    
    
    
    mVel += mAcc;
    
    mVel *= mDecay;
    
    
    //limitSpeed();
    
    mVel.y = 0;
    //mVel.y = 0.;
    
    
    mPos += mVel;
    
    //mTailPos = mPos - (mVelNormal * mLength);
    
    
    
    float c = math<float>::min(mNumNeighbors / 50.0f, 1.0f);
    mColor = ColorA(CM_HSV, 1.0f - c, c, c * 0.5f + 0.5f, 1.0f);
    
    
    
    mAcc = vec3(0.);
    mNeighborPos = vec3(0.);
    mNumNeighbors = 0;
    
    //if (mPos.x > getWindowWidth() || mPos.x < 0) mVel.x *= -1;
    //if (mPos.y > getWindowHeight() || mPos.y < 0) mVel.y *= -1;
    prevPos = mPos;
    
    
}





//------------------------------------ Particle Controller -------------------------------


class ParticleController {
public:
    ParticleController();
    void applyForceToParticles(float zoneRadius, float lowerThresh, float higherThresh, float attractStrength, float repelStrength, float orientStrength);
    void applyForceToPredators(float zoneRadius, float lowerThresh, float higherThresh);
    void pullToCenter(const ci::vec3 &center);
    void update(bool flatten, int w_, int h_, int l_);
    void draw();
    void drawDancer(int index_);
    void addParticles(int amt, int w_, int h_, int l_);
    void addPredators(int amt);
    void removeParticles(int amt);
    void drawConnections(int &numConnections_, int maxConnections_, int connectionThreshold_);
    ci::vec3 getPos();
    
    ci::Perlin mPerlin;
    
    std::vector<Particle>	mParticles;
    std::vector<Dancer>   mDancers;
    
    //std::list<Predator> mPredators;
    ci::vec3 mParticleCentroid;
    int mNumParticles;
};


ParticleController::ParticleController()
{
    mPerlin = Perlin(4);
    
}


void ParticleController::pullToCenter(const ci::vec3 &center)
{
    for (vector<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p){
        p->pullToCenter(center);
    }
    
    
}

void ParticleController::update(bool flatten, int w_, int h_, int l_)
{
    for (vector<Particle>::iterator p = mParticles.begin(); p != mParticles.end();){
        p->update(false);
        ++p;
    }

}

void ParticleController::draw()
{
    for (vector<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p)
    {
        p->draw();
    }
}





void ParticleController::drawConnections(int &numConnections_, int maxConnections_, int connectionThreshold_)
{
    for (int i = 0; i < mParticles.size(); i++)
    {
        
        if (numConnections_ > maxConnections_) continue;
        
        for (int j = 0; j < mParticles.size(); j++){
            if (i != j)
            {
                vec3 dir = mParticles[i].mPos - mParticles[j].mPos;
                float dist = length(dir);
                gl::lineWidth(1.5);
                if (dist < connectionThreshold_)
                {
                    numConnections_++;
                    gl::begin(GL_LINES);
                    gl::color(Colorf::white());
                    gl::vertex(mParticles[i].mPos);
                    gl::vertex(mParticles[j].mPos);
                    gl::end();
                }
            }
        }
    }
}




void ParticleController::addParticles(int amt,int w_, int h_, int l_)
{
    for (int i = 0; i<amt; i++)
    {
        vec3 pos = randVec3() * randFloat(-600., 600);
        console() << pos << endl;
        vec3 vel = Rand::randVec3() * randFloat(-.2, 0.2);
        
        bool followed = false;
        if (mParticles.size() == 0) followed = false;
        
        mParticles.push_back(Particle(pos, vel, followed));
    }
}


void ParticleController::removeParticles(int amt)
{
    for (int i = 0; i<amt; i++)
    {
        mParticles.pop_back();
    }
}

vec3 ParticleController::getPos()
{
    return mParticles.begin()->mPos;
}


//-------------------------------------------- M A I N

class Day64App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void saveGif();
    
    // CAMERA
    CameraPersp			mCam;
    
    void updateFbo();
    
    gl::BatchRef        mBatch;
    gl::GlslProgRef     mGlsl;
    gl::FboRef          mFbo;

    
    quat				mSceneRotation;
    vec3				mEye, mCenter, mUp;
    float				mCameraDistance;
    
    //PARTICLE SYSTEM
    ParticleController	mParticleController;
    float				mZoneRadius;
    float				mLowerThresh, mHigherThresh;
    float				mAttractStrength, mRepelStrength, mOrientStrength;
    
    bool				mCentralGravity;
    bool				mFlatten;
    bool				mSaveFrames;
    bool				mIsRenderingPrint;
    
    int w; //stores the width of the window
    int h; //stores the height of the window;
    int l;
    
    int maxConnections; //set the maximum number of connections
    int connectionThreshold;
    bool limitConnections; //boolean to limit the number of connections
    
    int count = 0;
    bool record;
    bool drawLines;
    bool attractToCenter;
    
    float theta = 0;
    
};

void Day64App::setup()
{
    Rand::randomize();
    
    //mCenter = vec3(0.f);
    mCentralGravity = true;
    mFlatten = false;
    mSaveFrames = false;
    mIsRenderingPrint = false;
    mZoneRadius = 200.0f;
    mLowerThresh = 0.5f;
    mHigherThresh = 0.8f;
    mAttractStrength = 0.00004f;
    mRepelStrength = 0.01f;
    mOrientStrength = 0.01f;
    
    // SETUP CAMERA
    mCameraDistance = 350.0f;
    mEye = vec3(0.0f, 0.0f, mCameraDistance);
    
    mUp = vec3(0., 1., 0.);
    mCam.lookAt(vec3(0., 0., 30.), vec3(0));
    mCam.setPerspective(60.0f, getWindowAspectRatio(), 5.0f, 5000.0f);
    
    record = false;
    maxConnections = 40;
    connectionThreshold = 60;
    limitConnections = false;
    drawLines = false;
    attractToCenter = false;
    
    
    
    
    // CREATE PARTICLE CONTROLLER
    mParticleController.addParticles(NUM_INITIAL_PARTICLES, w, h, l);
    
    w = getWindowWidth();
    h = getWindowHeight();
    l = 100;
    
    mCenter = vec3(0.);
    
    //SETUP THE FBO
    gl::Fbo::Format format;
    format.setSamples( 16 ); // SET ANTIALIASING
    mFbo = gl::Fbo::create(FBO_DIMENSIONS, FBO_DIMENSIONS, format.depthTexture());
    
    
    theta += 0.0523;
    
    gl::enableDepth();
}

void Day64App::mouseDown( MouseEvent event )
{
}

void Day64App::updateFbo()
{
    //THIS METHOD RENDERS WHATEVER WE'RE DRAWING INTO THE FBO
    
    gl::ScopedFramebuffer fbScp( mFbo ); //CREATING A SCOPED BUFFER MEANS THAT THE DEFAULT FRAMEBUFFER BINDING WILL BE RESTORED WHEN WE LEAVE THE SCOPE OF THIS FUNCTION
    
    gl::clear(Color(0.f, 0.f, 0.f));
    
    //SET THE VIEWPORT TO MATCH THE DIMENSIONS OF THE FBO - 1ST ARGUMENT IS THE POSITION )
    gl::ScopedViewport scpVp ( ivec2(0), mFbo->getSize());
    
    //SET UP A TEMPORARY CAMERA - 3RD ARGUMENT IS FOV
    CameraPersp     tempCam(mFbo->getWidth(), mFbo->getHeight(), 60.f);
    tempCam.setPerspective(60.f, mFbo->getAspectRatio(), 1.f, 1000.f);
    tempCam.lookAt(vec3(0.,0.,500.), vec3(0));
    gl::setMatrices(tempCam);
//    gl::color(ColorAf(1.,1.,1.,1.));
//    gl::rotate(angleAxis(theta, vec3(0.,1.,0)));
   
    ////DRAW THE PARTICLE SYSTEM OFFSCREEN
    int numConnected = 0;
    gl::color(ColorA(1.0f, 1.0f, 1.0f, 1.0f));
    mParticleController.draw();
    gl::lineWidth(0.5f);
    
    mParticleController.drawConnections(numConnected, maxConnections, connectionThreshold);
    
    
    
}

void Day64App::update()
{
    mParticleController.update(mFlatten, w, h, l);
    updateFbo();
    theta += 0.0523;
}

void Day64App::draw()
{
	gl::clear( Color( 0.1, 0.1, 0.1 ) );
    gl::setMatrices(mCam);
    
    // use the scene we rendered into the FBO as a texture
    mFbo->bindTexture();
    
    //gl::rotate(angleAxis(theta/2, vec3(0.,1.,0.)));
    
    // draw a cube textured with the FBO
    {
       // gl::rotate(angleAxis(theta/2, normalize(vec3(0., 1., 0.))));
        gl::rotate(angleAxis(theta/4, normalize(vec3(1., 0., 0.))));
        //gl::ScopedGlslProg shaderScp( gl::getStockShader( gl::ShaderDef().texture() ) );
        mBatch = gl::Batch::create(geom::Torus().subdivisionsAxis(64).subdivisionsHeight(16) >> geom::Scale(10.,10.,10.), gl::getStockShader( gl::ShaderDef().texture() ) );
        //gl::drawCube( vec3( 0 ), vec3(15));
        mBatch->draw();
    }
    
    saveGif();
}

void Day64App::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && count< 241) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "64"/ ("64-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
    count++;
}

CINDER_APP( Day64App, RendererGl (RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(500,500);
           })
