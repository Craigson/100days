#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Rand.h"
#include "cinder/Utilities.h"


using namespace ci;
using namespace ci::app;
using namespace std;

const int NUM_CUBES = 1000;

class Day26App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    void saveGif();
    
    gl::BatchRef        mBatch;
    gl::GlslProgRef     mGlsl;
    CameraPersp         mCam;
    
    glm::vec3 cubePositions[10] = {
        vec3( 1.0f,  0.0f,  -3.0f),
        vec3( 2.0f,  5.0f, 15.0f),
        vec3(-1.5f, -2.2f, -2.5f),
        vec3(-3.8f, -2.0f, 12.3f),
        vec3( 2.4f, -0.4f, -3.5f),
        vec3(-1.7f,  3.0f, 7.5f),
        vec3( 1.3f, -2.0f, -2.5f),
        vec3( 1.5f,  2.0f, -2.5f),
        vec3( 1.5f,  0.2f, 1.5f),
        vec3(-1.3f,  1.0f, -1.5f)
    };
    
    vector<vec3>    mCubes;
    
    float angle = 0.0f;
    
    float angleA = 0.0f;
};

void Day26App::setup()
{
    gl::clear();
    mCam.lookAt(vec3(0,0,0), vec3(0));
    mCam.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 100.f);
    
    auto cube = geom::Cube();
    
    for (int i = 0; i< NUM_CUBES; i++)
    {
        vec3 temp(Rand::randFloat(-20.f, 20.f),Rand::randFloat(-20.f, 20.f),Rand::randFloat(-20.f, 20.f));
        mCubes.push_back(temp);
    }
    
    setFrameRate(60);

    //CREATE A SHADER, WITH A UNIFORM MAT4 CALLED "TRANSFORM" THAT WE'LL MANIPULATE ON THE CPU AND PASS TO THE SHADER
    
    mGlsl = gl::GlslProg::create(gl::GlslProg::Format().
                         vertex(CI_GLSL(150,
                                        in vec3 ciPosition;
                                        in vec3 ciColor;
                                        in mat4 ciModelViewProjection;
                                        
                                        
                                        out vec3 ourColour;
                                        
                                        //uniform mat4 transform
                                        
                                        uniform mat4 model;
                                        uniform mat4 view;
                                        uniform mat4 projection;
                                        
                                        void main(void)
                                        {
                                            //gl_Position = ciModelViewProjection * vec4(ciPosition, 1.0f);
                                            gl_Position = projection * view * model * vec4(ciPosition, 1.0f);
                                            
                                            ourColour = ciColor;
                                        }
                                        ))
                         .fragment(CI_GLSL(150,
                                           in vec3 ourColour;
                                           out vec4 oColor;
                                           
                                           void main(void)
                                           {
                                               oColor = vec4(ourColour, 1.0f);
                                           })));
    
    
    mBatch = gl::Batch::create(cube, mGlsl);
    
    gl::enableAlphaBlending();
    
}

void Day26App::mouseDown( MouseEvent event )
{
}

void Day26App::update()
{
    /*  UNCOMMENT THE BELOW CODE TO ROTATE A SINGLE CUBE
     
    //UPDATE THE ROTATION ANGLE
    angle = getElapsedSeconds();
    
    //CREATE A VECTOR WITH HOMOGENOUS COORDINATE OF 1.0F
    vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    
    //CREATE THE IDENTITY MATRIX
    mat4 trans;
    
    //CREATE A TRANSFORMATION MATRIX BY PASSING THE IDENTITY MATRIX AND TRANSLATION AND ROTATION VECTORS TO THE ROTATE AND TRANSLATE FUNCTIONS.
    
     //trans = scale(trans, glm::vec3(0.5, 0.5, 0.5));
    trans = rotate(trans, angle, glm::vec3(0.0, 0.0, 1.0));
    trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    
   
    
    //MULTIPLY THE ORIGINAL VECTOR BY THE TRANSFORMATION MATRIX
    vec= trans * vec;
    cout << vec.x << vec.y << vec.z << endl;
    
    mGlsl->uniform("transform", trans);
    
     */
    angleA += 0.00523/2;
    
    cout << angleA << endl;
}

void Day26App::draw()
{
    gl::setMatrices(mCam);
	//gl::clear( ColorAf( 0, 0, 0,0.05 ) );
    gl::color(ColorAf(1.f, 1.f, 1.f, 0.2f));
    
    //mBatch->draw();
    
    float counter = 0;
    
    mat4 projection;
    mat4 view;
    
    view = rotate(view, angleA, vec3(0.5f,1.0f,0.0f));
    view = rotate(view, angleA, vec3(0.f,.1f,0.3f));
    view = translate(view, glm::vec3(0.0f, 0.0f, .0f));

    projection = glm::perspective(45.0f, getWindowAspectRatio(), 0.1f, 100.0f);
    
    //CREATE A VECTOR WITH HOMOGENOUS COORDINATE OF 1.0F
    vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    
    for (vec3 i : mCubes)
    {
        mat4 model;
        
        model = translate(model, i);
        GLfloat angle = 20.0f * counter;
        model = rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
        
        vec = model * vec;
        
        mGlsl->uniform("model", model);
        mGlsl->uniform("view", view);
        mGlsl->uniform("projection", projection);
        
        mBatch->draw();
        
        counter++;
    }
    
    saveGif();
}


void Day26App::saveGif()
{
    
    if (getElapsedFrames() % 4 == 0 && getElapsedFrames() < 481) writeImage( getDocumentsDirectory() / "Cinder" / "screenshots"/ "100days" / "26"/ ("26-0" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
    cout << "saving image" << endl;
}

CINDER_APP( Day26App, RendererGl(RendererGl::Options().msaa(16)),
           [&](App::Settings *settings){
               settings->setHighDensityDisplayEnabled();
               settings->setWindowSize(500,500);
           })
