#pragma once

#include "ofMain.h"
#include "ofxLeapMotion.h"
#include "ofxStrip.h"
#include "Particle.h"
#include "ofx3DModelLoader.h"


class testApp : public ofBaseApp{

  public:
    void setup();
    void update();
    void draw();
	
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void exit();
		
	ofxLeapMotion leap;
	vector <ofxLeapMotionSimpleHand> simpleHands;
    
	vector <int> fingersFound;
	ofEasyCam cam;
	ofLight l1;
	ofLight l2;
    ofLight l3;
    ofLight l4;

	ofMaterial m1;
    
    ofSpherePrimitive sphere;
	
	map <int, ofPolyline> fingerTrails;
    
    ofBoxPrimitive box;
    float x_pos, y_pos, z_pos;
    float boxx, boxy, boxz;
    float boxxp, boxyp, boxzp;
    
    ofBoxPrimitive box2;
    ofx3DModelLoader treemodel1, treemodel2, treemodel3;
    
    ofImage particleImage;
    
    ofVec3f lltop, lldown, lbtop, lbdown, rbtop, rbdown, rrtop, rrdown;
    
    ofTexture backtexture, window, present;
    
    ofMesh mesh1, mesh2, mesh3, mesh4;

    ofSoundPlayer bgmsound, shinesound;
    
    int icount;

    ofTrueTypeFont font;
};
