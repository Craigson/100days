#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"


#define NUM_INITIAL_PARTICLES 800

using namespace ci;
using namespace ci::app;
using namespace std;



//------------------------------------ Particle ------------------------------------------

class Particle {
public:
    Particle();
    Particle(ci::vec3 pos, ci::vec3 vel, bool followed);
    void pullToCenter(const ci::vec3 &center);
    void update(bool flatten);
    void draw();
    void drawTail();
    void limitSpeed();
    void addNeighborPos(ci::vec3 pos);
    void drawConnections();
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
};

Particle::Particle()
{
}

Particle::Particle(vec3 pos, vec3 vel, bool followed)
{
    mPos = pos;
    mTailPos = pos;
    mVel = vel;
    mVelNormal = vec3(0., 1., 0);
    mAcc = vec3(0.);
    
    mNeighborPos = vec3(0.);
    mNumNeighbors = 0;
    mMaxSpeed = Rand::randFloat(2.5f, 4.0f);
    mMaxSpeedSqrd = mMaxSpeed * mMaxSpeed;
    mMinSpeed = Rand::randFloat(1.0f, 1.5f);
    mMinSpeedSqrd = mMinSpeed * mMinSpeed;
    
    mColor = ColorA(1.0f, 1.0f, 1.0f, 1.0f);
    
    mDecay = 0.99f;
    mRadius = 1.0f;
    mLength = 5.0f;
    mFear = 1.0f;
    mCrowdFactor = 1.0f;
    
    mIsDead = false;
    mFollowed = followed;
}

void Particle::pullToCenter(const vec3 &center)
{
    vec3 dirToCenter = mPos - center;
    //float distToCenter = dirToCenter.length();
    float distToCenter = length(dirToCenter);
    float distThresh = 350.0f;
    
    if (distToCenter > distThresh){
        //dirToCenter.normalize();
        normalize(dirToCenter);
        float pullStrength = 0.0025f;
        mVel -= dirToCenter * ((distToCenter - distThresh) * pullStrength);
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
    
    mFear -= (mFear - 0.0f) * 0.2f;
    
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
    //glColor4f(mColor);
    gl::color(Colorf::white());
    gl::drawVector(mPos - mVelNormal * mLength, mPos - mVelNormal * mLength * 0.75f, mLength * 0.7f, mRadius);
    //gl::drawSphere(mPos,2);
    //drawTail();
}

void Particle::drawTail()
{
    //glColor4f(mColor);
    gl::color(Colorf(mColor));
    gl::vertex(mPos);
    gl::color(ColorA(mColor.r, mColor.g, mColor.b, 0.01f));
    gl::vertex(mTailPos);
}

void Particle::addNeighborPos(vec3 pos)
{
    mNeighborPos += pos;
    mNumNeighbors++;
}

//------------------------------------ Particle Controller -------------------------------


class ParticleController {
public:
    ParticleController();
    void applyForceToParticles(float zoneRadius, float lowerThresh, float higherThresh, float attractStrength, float repelStrength, float orientStrength);
    void applyForceToPredators(float zoneRadius, float lowerThresh, float higherThresh);
    void pullToCenter(const ci::vec3 &center);
    void update(bool flatten);
    void draw();
    void addParticles(int amt);
    void addPredators(int amt);
    void removeParticles(int amt);
    void drawConnections();
    ci::vec3 getPos();
    
    ci::Perlin mPerlin;
    
    std::list<Particle>	mParticles;
    //std::list<Predator> mPredators;
    ci::vec3 mParticleCentroid;
    int mNumParticles;
};


ParticleController::ParticleController()
{
    mPerlin = Perlin(4);
}

void ParticleController::applyForceToParticles(float zoneRadius, float lowerThresh, float higherThresh, float attractStrength, float repelStrength, float alignStrength)
{
    float twoPI = M_PI * 2.0f;
    mParticleCentroid = vec3(0.);
    mNumParticles = mParticles.size();
    
    for (list<Particle>::iterator p1 = mParticles.begin(); p1 != mParticles.end(); ++p1){
        
        list<Particle>::iterator p2 = p1;
        for (++p2; p2 != mParticles.end(); ++p2) {
            vec3 dir = p1->mPos - p2->mPos;
            //float distSqrd = dir.lengthSquared();
            float distSqrd = length2(dir);
            float zoneRadiusSqrd = zoneRadius * p1->mCrowdFactor * zoneRadius * p2->mCrowdFactor;
            
            if (distSqrd < zoneRadiusSqrd){		// Neighbor is in the zone
                float per = distSqrd / zoneRadiusSqrd;
                p1->addNeighborPos(p2->mPos);
                p2->addNeighborPos(p1->mPos);
                
                if (per < lowerThresh){			// Separation
                    float F = (lowerThresh / per - 1.0f) * repelStrength;
                    //dir.normalize();
                    normalize(dir);
                    dir *= F;
                    
                    p1->mAcc += dir;
                    p2->mAcc -= dir;
                }
                else if (per < higherThresh){	// Alignment
                    float threshDelta = higherThresh - lowerThresh;
                    float adjPer = (per - lowerThresh) / threshDelta;
                    float F = (1.0f - (cos(adjPer * twoPI) * -0.5f + 0.5f)) * alignStrength;
                    
                    p1->mAcc += p2->mVelNormal * F;
                    p2->mAcc += p1->mVelNormal * F;
                    
                }
                else {							// Cohesion (prep)
                    float threshDelta = 1.0f - higherThresh;
                    float adjPer = (per - higherThresh) / threshDelta;
                    float F = (1.0f - (cos(adjPer * twoPI) * -0.5f + 0.5f)) * attractStrength;
                    
                    //dir.normalize();
                    normalize(dir);
                    dir *= F;
                    
                    p1->mAcc -= dir;
                    p2->mAcc += dir;
                }
            }
        }
        
        mParticleCentroid += p1->mPos;
        
         if( p1->mNumNeighbors > 0 ){ // Cohesion
         vec3 neighborAveragePos = ( p1->mNeighborPos/(float)p1->mNumNeighbors );
         p1->mAcc += ( neighborAveragePos - p1->mPos ) * attractStrength;
         }
         
        
        // ADD PERLIN NOISE INFLUENCE
        float scale = 0.002f;
        float multi = 0.01f;
        vec3 perlin = mPerlin.dfBm(p1->mPos * scale) * multi;
        p1->mAcc += perlin;
        
        
        // CHECK WHETHER THERE IS ANY PARTICLE/PREDATOR INTERACTION
        float eatDistSqrd = 50.0f;
        float predatorZoneRadiusSqrd = zoneRadius * zoneRadius * 5.0f;
        
        /*
         for (list<Predator>::iterator predator = mPredators.begin(); predator != mPredators.end(); ++predator) {
         
         vec3 dir = p1->mPos - predator->mPos[0];
         //float distSqrd = dir.lengthSquared();
         float distSqrd = length2(dir);
         
         if (distSqrd < predatorZoneRadiusSqrd){
         if (distSqrd > eatDistSqrd){
         float F = (predatorZoneRadiusSqrd / distSqrd - 1.0f) * 0.1f;
         p1->mFear += F * 0.1f;
         //dir = dir.normalized() * F;
         dir = normalize(dir) * F;
         p1->mAcc += dir;
         if (predator->mIsHungry)
         predator->mAcc += dir * 0.04f * predator->mHunger;
         }
         else {
         p1->mIsDead = true;
         predator->mHunger = 0.0f;
         predator->mIsHungry = false;
         }
         }
         }
         */
        
    }
    mParticleCentroid /= (float)mNumParticles;
}

/*
 void ParticleController::applyForceToPredators(float zoneRadius, float lowerThresh, float higherThresh)
 {
 float twoPI = M_PI * 2.0f;
 for (list<Predator>::iterator P1 = mPredators.begin(); P1 != mPredators.end(); ++P1){
 
 list<Predator>::iterator P2 = P1;
 
 for (++P2; P2 != mPredators.end(); ++P2) {
 vec3 dir = P1->mPos[0] - P2->mPos[0];
 //float distSqrd = dir.lengthSquared();
 float distSqrd = length2(dir);
 float zoneRadiusSqrd = zoneRadius * zoneRadius * 4.0f;
 
 if (distSqrd < zoneRadiusSqrd){		// Neighbor is in the zone
 float per = distSqrd / zoneRadiusSqrd;
 if (per < lowerThresh){			// Separation
 float F = (lowerThresh / per - 1.0f) * 0.01f;
 normalize(dir);
 dir *= F;
 
 P1->mAcc += dir;
 P2->mAcc -= dir;
 }
 else if (per < higherThresh){	// Alignment
 float threshDelta = higherThresh - lowerThresh;
 float adjPer = (per - lowerThresh) / threshDelta;
 float F = (1.0f - cos(adjPer * twoPI) * -0.5f + 0.5f) * 0.3f;
 
 P1->mAcc += P2->mVelNormal * F;
 P2->mAcc += P1->mVelNormal * F;
 
 }
 else {							// Cohesion
 float threshDelta = 1.0f - higherThresh;
 float adjPer = (per - higherThresh) / threshDelta;
 float F = (1.0f - (cos(adjPer * twoPI) * -0.5f + 0.5f)) * 0.1f;
 
 //dir.normalize();
 normalize(dir);
 dir *= F;
 
 P1->mAcc -= dir;
 P2->mAcc += dir;
 }
 }
 }
 }
 }
 
 */


void ParticleController::pullToCenter(const ci::vec3 &center)
{
    for (list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p){
        p->pullToCenter(center);
    }
    
    /*
     for (list<Predator>::iterator p = mPredators.begin(); p != mPredators.end(); ++p){
     p->pullToCenter(center);
     }
     */
}

void ParticleController::update(bool flatten)
{
    for (list<Particle>::iterator p = mParticles.begin(); p != mParticles.end();){
        if (p->mIsDead){
            p = mParticles.erase(p);
        }
        else {
            p->update(flatten);
            ++p;
        }
    }
    
    /*
     for (list<Predator>::iterator p = mPredators.begin(); p != mPredators.end(); ++p){
     p->update(flatten);
     }
     */
}

void ParticleController::draw()
{
    
    /*
     // DRAW PREDATOR ARROWS
     for (list<Predator>::iterator p = mPredators.begin(); p != mPredators.end(); ++p){
     float hungerColor = 1.0f - p->mHunger;
     gl::color(ColorA(1.0f, hungerColor, hungerColor, 1.0f));
     p->draw();
     }
     */
    // DRAW PARTICLE ARROWS
    gl::color(ColorA(1.0f, 1.0f, 1.0f, 1.0f));
    //glBegin( GL_LINES );
    for (list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p){
        p->draw();
    }
    //glEnd();
}

void ParticleController::drawConnections()
{
    for (list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p){
        for (list<Particle>::iterator q = mParticles.begin(); q != mParticles.end(); ++p){
            if (p != q)
            {
                vec3 dir = p->mPos - q->mPos;
                float dist = length(dir);
                
                if (dist < 10.) gl::drawLine(p->mPos, q->mPos);
            }
        }
    }
}

void ParticleController::addPredators(int amt)
{
    for (int i = 0; i<amt; i++)
    {
        vec3 pos = Rand::randVec3() * Rand::randFloat(500.0f, 750.0f);
        vec3 vel = Rand::randVec3();
        //mPredators.push_back(Predator(pos, vel));
    }
}

void ParticleController::addParticles(int amt)
{
    for (int i = 0; i<amt; i++)
    {
        vec3 pos = Rand::randVec3() * Rand::randFloat(100.0f, 200.0f);
        vec3 vel = Rand::randVec3();
        
        bool followed = false;
        if (mParticles.size() == 0) followed = true;
        
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

//---------------------------------------------------------------------------


class Day57App : public App {
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
    
    void saveGif();
    
    // CAMERA
    CameraPersp			mCam;
    quat				mSceneRotation;
    vec3				mEye, mCenter, mUp;
    float				mCameraDistance;
    
    ParticleController	mParticleController;
    float				mZoneRadius;
    float				mLowerThresh, mHigherThresh;
    float				mAttractStrength, mRepelStrength, mOrientStrength;
    
    bool				mCentralGravity;
    bool				mFlatten;
    bool				mSaveFrames;
    bool				mIsRenderingPrint;
};


void Day57App::setup()
{
    Rand::randomize();
    
    mCenter = vec3(getWindowWidth() * 0.5f, getWindowHeight() * 0.5f, 0.0f);
    mCentralGravity = false;
    mFlatten = false;
    mSaveFrames = false;
    mIsRenderingPrint = false;
    mZoneRadius = 80.0f;
    mLowerThresh = 0.5f;
    mHigherThresh = 0.8f;
    mAttractStrength = 0.004f;
    mRepelStrength = 0.01f;
    mOrientStrength = 0.01f;
    
    // SETUP CAMERA
    mCameraDistance = 350.0f;
    mEye = vec3(0.0f, 0.0f, mCameraDistance);
    mCenter = vec3(0.);
    mUp = vec3(0., 1., 0.);
//    mCam.lookAt(vec3(0.,0.,20.), vec3(0.));
    mCam.setPerspective(75.0f, getWindowAspectRatio(), 5.0f, 5000.0f);
    
    
    // CREATE PARTICLE CONTROLLER
    mParticleController.addParticles(NUM_INITIAL_PARTICLES);
    
}

void Day57App::mouseDown( MouseEvent event )
{
}

void Day57App::update()
{
    if (mLowerThresh > mHigherThresh) mHigherThresh = mLowerThresh;
    
    //mParticleController.applyForceToPredators(mZoneRadius, 0.4f, 0.7f);
    mParticleController.applyForceToParticles(mZoneRadius, mLowerThresh, mHigherThresh, mAttractStrength, mRepelStrength, mOrientStrength);
    if (mCentralGravity) mParticleController.pullToCenter(mCenter);
    mParticleController.update(mFlatten);
    
    mEye = vec3(0.0f, 0.0f, mCameraDistance);
    mCam.lookAt(mEye, mCenter, mUp);
    gl::setMatrices(mCam);
    gl::rotate(mSceneRotation);
}

void Day57App::draw()
{
    gl::clear(Color(0, 0, 0), true);
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    gl::color(ColorA(1.0f, 1.0f, 1.0f, 1.0f));
    mParticleController.draw();
    //	mParticleController.drawConnections();
}
void Day57App::saveGif()
{
    if (getElapsedFrames() % 2 == 0 && getElapsedFrames() < 241) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "56a"/ ("56a-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
}


CINDER_APP( Day57App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(500, 500);
           })
