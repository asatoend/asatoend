#include "testApp.h"
#include "math.h"
#define NUM 300 //円の数を表す定数NUMを300と定義
#define Vexterx3d Vector3D


/* Note on OS X, you must have this in the Run Script Build Phase of your project. 
where the first path ../../../addons/ofxLeapMotion/ is the path to the ofxLeapMotion addon. 

cp -f ../../../addons/ofxLeapMotion/libs/lib/osx/libLeap.dylib "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/MacOS/libLeap.dylib"; install_name_tool -change ./libLeap.dylib @executable_path/libLeap.dylib "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/MacOS/$PRODUCT_NAME";

   If you don't have this you'll see an error in the console: dyld: Library not loaded: @loader_path/libLeap.dylib
*/

float loc_x[NUM]; //円のx座標
float loc_y[NUM]; //円のy座標
float loc_z[NUM]; //円のz座標
float speed_x[NUM]; //x軸方向のスピード
float speed_y[NUM]; //y軸方向のスピード
float speed_z[NUM]; //z軸方向のスピード
int red[NUM]; //Red成分
int green[NUM]; //Green成分
int blue[NUM]; //Blue成分

float ipos[NUM];//玉が箱に入っているかの判定用フラグ

//--------------------------------------------------------------
void testApp::setup(){

    ofSetFrameRate(60);
    ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);

    ofEnableDepthTest();
    ofEnableSmoothing();
    
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    
    ofBackground(0);

    ofEnableNormalizedTexCoords(); //テクスチャ貼るために必要

	leap.open();
    leap.setupGestures();   // we enable our gesture detection here

    //ライト位置
	l1.setPosition(500, 500, 500);
	l2.setPosition(-200, -200, 50);
    
    l3.enable();
    l3.setSpotlight();
    l3.setPosition(100,100,100);
    
	cam.setOrientation(ofPoint(-5, 45, 0));    //カメラ位置

	glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    
    //中央ボックスに入っている玉の跳ね返り位置調整
    x_pos = 50; //X軸方向ここから
    y_pos = -150; //Y軸方向ここから
    z_pos = 50; //Z軸方向ここから
    boxx = 100; //X軸方向調整
    boxy = 100; //Y軸方向調整
    boxz = 100; //Z軸方向調整
    boxxp = x_pos + boxx; //X軸方向ここまで
    boxyp = y_pos + boxy; //Y軸方向ここまで
    boxzp = z_pos + boxz; //Z軸方向ここまで
    
    ofSetCircleResolution(64); //円の解像度設定
    ofEnableAlphaBlending(); //アルファチャンネルを有効に
    
    leap.iGestures = defaultComponentAnyFlags;
    
    //NUMの数だけ初期値の生成を繰り返す
    for(int i = 0; i < NUM; i++){
        loc_x[i] = ofRandom(-ofGetWidth()/2, ofGetWidth()/2); //円のx座標初期位置
        loc_y[i] = ofRandom(-ofGetHeight()/2, ofGetHeight()/2); //円のy座標初期位置
        loc_z[i] = ofRandom(-800, 800); //円のz座標初期位置
        speed_x[i] = ofRandom(-5, 5); //x軸方向スピード初期値
        speed_y[i] = ofRandom(-5, 5); //y軸方向スピード初期値
        speed_z[i] = ofRandom(-5, 5); //z軸方向スピード初期値
        red[i] = 255;
        green[i] = 255;
        blue[i] = 255;
        
        icount = 0;
        ipos[i] = 0;
    }
    
    
    ofLoadImage(present, "present.png");

    
    sphere.setRadius(5);
    
    //外枠
    mesh1.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    mesh1.enableColors();
    
    mesh2.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    mesh2.enableColors();
    
    mesh3.setMode(OF_PRIMITIVE_LINE_LOOP);
    mesh3.enableColors();
    
    mesh4.setMode(OF_PRIMITIVE_LINE_LOOP);
    mesh4.enableColors();
    
    //外枠の位置調整（頂点8箇所）
    ofVec3f lltop(-800, 400, 800); //左手前上（X,Y,Z)
    ofVec3f lldown(-800, -430, 800); //左手前下（X,Y,Z)
    ofVec3f lbtop(-800, 700, -800); //左奥上（X,Y,Z)
    ofVec3f lbdown(-800, -800, -800); //左奥下（X,Y,Z)
    ofVec3f rbtop(800, 400, -800); //右奥上（X,Y,Z)
    ofVec3f rbdown(800, -500, -800); //右奥下（X,Y,Z)
    ofVec3f rrtop(800, 800, 800); //右手前上（X,Y,Z)
    ofVec3f rrdown(800, -800, 800); //右手前下（X,Y,Z)

    //左側面
    mesh1.addVertex(lltop);
    mesh1.addVertex(lldown);
    mesh1.addVertex(lbtop);
    mesh1.addVertex(lbdown);
    mesh1.setupIndicesAuto();
    
    //背面
    mesh2.addVertex(lbtop);
    mesh2.addVertex(lbdown);
    mesh2.addVertex(rbtop);
    mesh2.addVertex(rbdown);
    mesh2.setupIndicesAuto();
    
    //右側面
    mesh3.addVertex(rbtop);
    mesh3.addVertex(rbdown);
    mesh3.addVertex(rrdown);
    mesh3.addVertex(rrtop);
    mesh3.setupIndicesAuto();

    //下面
    mesh4.addVertex(lbdown);
    mesh4.addVertex(rbdown);
    mesh4.addVertex(lldown);
    mesh4.addVertex(rrdown);
    mesh4.setupIndicesAuto();


    //木のモデル読み込み
    treemodel1.loadModel("CL20a.3ds", 2);
    treemodel1.setRotation(0, -90, 1, 0, 0);
    
    treemodel2.loadModel("CL20m.3ds", 2);
    treemodel2.setRotation(0, -90, 1, 0, 0);

    treemodel3.loadModel("CL20y.3ds", 4);
    treemodel3.setRotation(0, -90, 1, 0, 0);

    
    bgmsound.loadSound("o13.mp3"); //サウンドファイルの読込み
    bgmsound.setLoop(true); //ループ再生をONに
    bgmsound.play(); //サウンド再生開始
    
    shinesound.loadSound("shine1.mp3"); //サウンドファイルの読込み
    
    font.loadFont("Arial.ttf", 15); //フォントの読込

}


//--------------------------------------------------------------
void testApp::update(){

	fingersFound.clear();
	
    vector <Hand> hands = leap.getLeapHands();
    
	if( leap.isFrameNew() && hands.size() ){

		//leap returns data in mm - lets set a mapping to our world space.
		//you can get back a mapped point by using ofxLeapMotion::getMappedofPoint with the Leap::Vector that tipPosition returns
		leap.setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2);
		leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
		leap.setMappingZ(-150, 150, -200, 200);
				
		for(int i = 0; i < hands.size(); i++){
            for(int j = 0; j < hands[i].fingers().count(); j++){
				ofPoint pt;
			
				const Finger & finger = hands[i].fingers()[j];
								
				//here we convert the Leap point to an ofPoint - with mapping of coordinates
				//if you just want the raw point - use ofxLeapMotion::getofPoint
				pt = leap.getMappedofPoint( finger.tipPosition() );
                
				//lets get the correct trail (ofPolyline) out of our map - using the finger id as the key
				ofPolyline & polyline = fingerTrails[finger.id()];
				
				//if the distance between the last point and the current point is too big - lets clear the line
				//this stops us connecting to an old drawing
				if( polyline.size() && (pt-polyline[polyline.size()-1] ).length() > 50 ){
					polyline.clear();
				}
				
				//add our point to our trail
				polyline.addVertex(pt);
				
				//store fingers seen this frame for drawing
				fingersFound.push_back(finger.id());
			}
		}
	}
    
    leap.updateGestures();  // check for gesture updates
    
    //NUMの数だけ座標の更新を繰り返す
    for(int i = 0; i < NUM; i++){
    
    //玉が箱に入っていない場合
    if(ipos[i]!=1){
        //もしリープジェスチャーがサークルなら玉が集まってくる
        if(leap.iGestures == 9 or leap.iGestures == 10){
            shinesound.play();
            //手の現在位置から円のスピードを再計算
            speed_x[i] = (leap.circleCenter.x - loc_x[i] + ofRandom(-3000, 3000)) / 200.0; //手のx座標と円のx座標の距離の1/8だけ接近
            speed_y[i] = (leap.circleCenter.y - loc_y[i] + ofRandom(-3000, 3000)) / 200.0; //手のy座標と円のy座標の距離の1/8だけ接近
            speed_z[i] = (leap.circleCenter.z - loc_z[i] + ofRandom(-3000, 3000)) / 200.0; //手のz座標と円のz座標の距離の1/8だけ接近
            loc_x[i] = loc_x[i] + speed_x[i]; //円のx座標を更新
            loc_y[i] = loc_y[i] + speed_y[i]; //円のy座標を更新
            loc_z[i] = loc_z[i] + speed_z[i]; //円のz座標を更新
            //もし外枠に当たったら跳ね返る
            if(loc_x[i] < -ofGetWidth()/2){
                speed_x[i] = speed_x[i] * -1;
            }
            if(loc_x[i] > ofGetWidth()/2){
                speed_x[i] = speed_x[i] * -1;
            }
            if(loc_y[i] < -ofGetHeight()/2){
                speed_y[i] = speed_y[i] * -1;
            }
            if(loc_y[i] > ofGetHeight()/2){
                speed_y[i] = speed_y[i] * -1;
            }
            if(loc_z[i] < -800){
                speed_z[i] = speed_z[i] * -1;
            }
            if(loc_z[i] > 800){
                speed_z[i] = speed_z[i] * -1;
            }
            //もし箱の中に入ったら入った判定フラグを立てる
            if(loc_x[i] > x_pos and loc_x[i] < boxxp and loc_y[i] > y_pos and loc_y[i] < boxyp and loc_z[i] > z_pos and loc_z[i] < boxzp){
                ipos[i] = 1;
                icount = icount + 1;
            }
        }
        //リープジェスチャーがサークルでない場合
        else{
            speed_x[i] = speed_x[i]; //手のx座標と円のx座標の距離の1/8だけ接近
            speed_y[i] = speed_y[i]; //手のy座標と円のy座標の距離の1/8だけ接近
            speed_z[i] = speed_z[i]; //手のz座標と円のz座標の距離の1/8だけ接近
            loc_x[i] = loc_x[i] + speed_x[i]; //円のx座標を更新
            loc_y[i] = loc_y[i] + speed_y[i]; //円のy座標を更新
            loc_z[i] = loc_z[i] + speed_z[i]; //円のz座標を更新
            //もし外枠に当たったら跳ね返る
            if(loc_x[i] < -ofGetWidth()/2){
                speed_x[i] = speed_x[i] * -1;
            }
            if(loc_x[i] > ofGetWidth()/2){
                speed_x[i] = speed_x[i] * -1;
            }
            if(loc_y[i] < -ofGetHeight()/2){
                speed_y[i] = speed_y[i] * -1;
            }
            if(loc_y[i] > ofGetHeight()/2){
                speed_y[i] = speed_y[i] * -1;
            }
            if(loc_z[i] < -800){
                speed_z[i] = speed_z[i] * -1;
            }
            if(loc_z[i] > 800){
                speed_z[i] = speed_z[i] * -1;
            }
            //箱に当たったら跳ね返る
            if(loc_x[i] > x_pos && loc_x[i] < boxxp && loc_y[i] > y_pos && loc_y[i] < boxyp && loc_z[i] > z_pos && loc_z[i] < boxzp){
                speed_x[i] = speed_x[i] * -1;
                speed_y[i] = speed_y[i] * -1;
                speed_z[i] = speed_z[i] * -1;
            }
        }
    }
    //玉が箱に入っている場合
    else if(ipos[i] == 1){
            speed_x[i] = speed_x[i]; //x軸方向スピード初期値
            speed_y[i] = speed_y[i]; //y軸方向スピード初期値
            speed_z[i] = speed_z[i]; //z軸方向スピード初期値
            loc_x[i] = loc_x[i] + speed_x[i]; //円のx座標を更新
            loc_y[i] = loc_y[i] + speed_y[i]; //円のy座標を更新
            loc_z[i] = loc_z[i] + speed_z[i]; //円のz座標を更新
            //箱に当たったら中で跳ね返る
            if(loc_x[i] < boxxp){
                speed_x[i] = speed_x[i] * -1;
            }
            if(loc_x[i] > x_pos){
                speed_x[i] = speed_x[i] * -1;
            }
            if(loc_y[i] < boxyp){
                speed_y[i] = speed_y[i] * -1;
            }
            if(loc_y[i] > y_pos){
                speed_y[i] = speed_y[i] * -1;
            }
            if(loc_z[i] < boxzp){
                speed_z[i] = speed_z[i] * -1;
            }
            if(loc_z[i] > z_pos){
                speed_z[i] = speed_z[i] * -1;
            }
        }
    }
    
    //キータップの動作で速度初期化
    for(int i = 0; i < NUM; i++){
        if(leap.iGestures == 2){
        speed_x[i] = ofRandom(-5, 5); //x軸方向スピード初期値
        speed_y[i] = ofRandom(-5, 5); //y軸方向スピード初期値
        speed_z[i] = ofRandom(-5, 5); //z軸方向スピード初期値
        }
    }
    
    //IMPORTANT! - tell ofxLeapMotion that the frame is no longer new.
    leap.markFrameAsOld();
    
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetColor(200);
    font.drawString("Let's put white balls into a red box : " + ofToString(icount) + " / " + ofToString(NUM), 150, 30);

	cam.begin();
	
	ofEnableLighting();
	l1.enable();
	l2.enable();
    l3.enable();
	
	m1.begin();
	m1.setShininess(0.6);
	
    //手の軌跡の描画
	for(int i = 0; i < fingersFound.size(); i++){
		ofxStrip strip;
		int id = fingersFound[i];
        ofPolyline & polyline = fingerTrails[id];
        strip.generate(polyline.getVertices(), 30, ofPoint(0, 0.5, 0.5) );
        ofSetColor(255, 255, 224, 200);
		strip.getMesh().draw();
	}
	
    l2.disable();
    
    for(int i = 0; i < simpleHands.size(); i++){
        simpleHands[i].debugDraw();
    }

	m1.end();

    //黒背景で外枠の描画
    ofPushMatrix();
        ofFill();
        ofSetColor(0);
        mesh1.draw();
        mesh2.draw();
        mesh3.draw();
        mesh4.draw();
    ofPopMatrix();
    
    //NUMの数だけ円を描画する
    for(int i = 0; i < NUM; i++){
        if(ipos[i] == 1){
            ofSetColor(184, 128, 125);
        }
        else{
            ofSetColor(red[i], green[i], blue[i], 127); //描画色の設定
        }
        l4.enable();
        l4.setAmbientColor(150);
        l4.setPosition(loc_x[i], loc_y[i], loc_z[i]);
        l4.setAmbientColor(ofFloatColor(0.3,0.8,0.5,1.0));
        l4.setDiffuseColor(ofFloatColor(0.2,0.2,1.0));
        l4.setSpecularColor(ofFloatColor(1.0,1.0,1.0));
        sphere.setPosition(loc_x[i], loc_y[i], loc_z[i]);
        sphere.draw();
    }
    
    //プレゼントボックスの描画
    ofPushMatrix();
        present.bind();
        ofFill();
        ofSetColor(100,100,100,180.0);
        box.set(100, 100, 100);
        box.setPosition(100, -100, 100);
        box.draw();
        present.unbind();
    ofPopMatrix();

    //木の描画
    ofPushMatrix();
        ofSetColor(10, 100, 10);
        treemodel1.setPosition(-305, -275, 100);
        glScalef(1.0f, 1.0f, 1.0f);
        treemodel1.draw();
    ofPopMatrix();
    
    ofPushMatrix();
        ofSetColor(10, 100, 10);
        treemodel2.setPosition(380, -370, -400);
        glScalef(0.7f, 0.7f, 0.7f);
        treemodel2.draw();
    ofPopMatrix();
    
    ofPushMatrix();
        ofSetColor(10, 100, 10);
        treemodel3.setPosition(140, -415, -400);
        glScalef(0.7f, 0.7f, 0.7f);
        treemodel3.draw();
    ofPopMatrix();
    
    
    string msg;
    
    switch (leap.iGestures) {
        case 1:
            msg = "Screen Tap";
            msg += "\nPosition: "+ofToString(leap.screenTapPosition);
            break;
            
        case 2:
            msg = "Key Tap";
            msg += "\nPosition: " + ofToString(leap.keyTapPosition);
            break;
            
        case 3:
            msg = "Swipe Right";
            msg += "\nSwipe Speed mm/s: " + ofToString(leap.swipeSpeed) + "\nDuration sec's: "+ofToString(leap.swipeDurationSeconds);
            break;
            
        case 4:
            msg = "Swipe Left";
            msg += "\nSwipe Speed mm/s: " + ofToString(leap.swipeSpeed) + "\nDuration sec's: "+ofToString(leap.swipeDurationSeconds);
            break;
            
        case 5:
            msg = "Swipe Down";
            msg += "\nSwipe Speed mm/s: " + ofToString(leap.swipeSpeed) + "\nDuration sec's: "+ofToString(leap.swipeDurationSeconds);
            break;
            
        case 6:
            msg = "Swipe Up";
            msg += "\nSwipe Speed mm/s: " + ofToString(leap.swipeSpeed) + "\nDuration sec's: "+ofToString(leap.swipeDurationSeconds);
            break;
            
        case 7:
            msg = "Swipe Forward";
            msg += "\nSwipe Speed mm/s: " + ofToString(leap.swipeSpeed) + "\nDuration sec's: "+ofToString(leap.swipeDurationSeconds);
            break;
            
        case 8:
            msg = "Swipe Backwards";
            msg += "\nSwipe Speed mm/s: " + ofToString(leap.swipeSpeed) + "\nDuration sec's: "+ofToString(leap.swipeDurationSeconds);
            break;
            
        case 9:
            msg = "Circle Left";
            msg += "\nCircle Radius: " + ofToString(leap.circleRadius) + "\nCircle Center: "+ofToString(leap.circleCenter);
            break;
            
        case 10:
            msg = "Circle Right";
            msg += "\nCircle Radius: " + ofToString(leap.circleRadius) + "\nCircle Center: "+ofToString(leap.circleCenter);
            break;
            
        case 11:
            msg = "Waiting for hand movement...";
            break;
            
        default:
            msg = "Waiting for hand movement...";
            break;

    }

	cam.end();
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    //rキーで全て初期化
    switch (key) {
        case 'r':
            for(int i = 0; i < NUM; i++){
                loc_x[i] = ofRandom(-ofGetWidth()/2, ofGetWidth()/2); //円のx座標初期位置
                loc_y[i] = ofRandom(-ofGetHeight()/2, ofGetHeight()/2); //円のy座標初期位置
                loc_z[i] = ofRandom(-800, 800); //円のz座標初期位置
                speed_x[i] = ofRandom(-5, 5); //x軸方向スピード初期値
                speed_y[i] = ofRandom(-5, 5); //y軸方向スピード初期値
                speed_z[i] = ofRandom(-5, 5); //z軸方向スピード初期値
                red[i] = 255;
                green[i] = 255;
                blue[i] = 255;
                
                icount = 0;
                ipos[i] = 0;
            }

            break;
            
    }
    
  
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
    
}

//--------------------------------------------------------------
void testApp::exit(){
    // let's close down Leap and kill the controller
    leap.close();
}
