#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetVerticalSync(true);
	importImage("images/iceberg.png");
    mouseInteract = false;
    animateFlag = true;
    timeStamp = ofGetElapsedTimef();
	mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    // grid
    columns = 10;
    rows = 10;
    resetGrid();
    gui.addSlider("COLUMNS", 1.f, 100.f, columns);
    gui.addSlider("ROWS", 1.f, 100.f, rows);
    
    linkStiffnessMul = .01f;
    gui.addSlider("LINK_STIFFNESS_FACTOR", 0, 1.f, linkStiffnessMul);
    
    // mouse
    mouseInfluenceSize = 100.f;
    gui.addSlider("MOUSE_INFLUENCE_SIZE", 0, 400.f, mouseInfluenceSize);
    mouseInfluenceScalar = .8f;
    gui.addSlider("MOUSE_INFLUENCE_SCALAR", 0, 2.f, mouseInfluenceScalar);
    mouseZ = -20.f;
    gui.addSlider("MOUSE_Z", -100.f, 100.f, mouseZ);
    
    // light
    light.setPointLight();
    
    // light smoothing
    ofSetSmoothLighting(true);
    gui.addToggle("SMOOTH_LIGHT", true);
    
    // light position
    lightPosition = ofVec3f(0, 200, 200);
    light.setPosition(lightPosition);
    
    float dLightPos = 1000.f;
    gui.addSlider("LIGHT_POSITION_X", -dLightPos, dLightPos, lightPosition.x);
    gui.addSlider("LIGHT_POSITION_Y", -dLightPos, dLightPos, lightPosition.y);
    gui.addSlider("LIGHT_POSITION_Z", -dLightPos, dLightPos, lightPosition.z);
    
    // light direction
    lightDirection = ofVec3f(0, 1, 0);
    light.setOrientation(lightDirection);
    
    float dLightDir = 1.f;
    gui.addSlider("LIGHT_DIRECTION_X", -dLightDir, dLightDir, lightDirection.x);
    gui.addSlider("LIGHT_DIRECTION_Y", -dLightDir, dLightDir, lightDirection.y);
    gui.addSlider("LIGHT_DIRECTION_Z", -dLightDir, dLightDir, lightDirection.z);
    
    // load / save pic
    gui.addButton("IMPORT", false);
    gui.addButton("EXPORT", false);
    
    gui.autoSizeToFitWidgets();
    //gui->loadSettings("settings.xml");
    ofAddListener(gui.newGUIEvent, this, &ofApp::guiEvent);
}

void ofApp::guiEvent(ofxUIEventArgs &e) {
    
    // TODO: dry this!!!
    
    string name = e.getName();
    
    // grid
    if (name == "ROWS"){
        ofxUISlider *slider = e.getSlider();
        rows = (int) floorf(slider->getScaledValue());
        resetGrid();
    } else if (name == "COLUMNS"){
        ofxUISlider *slider = e.getSlider();
        columns = (int) floorf(slider->getScaledValue());
        resetGrid();
    }
    
    else if (name == "LINK_STIFFNESS_FACTOR"){
        ofxUISlider *slider = e.getSlider();
        linkStiffnessMul = slider->getScaledValue();
    }
    
    // mouse interaction
    else if (name == "MOUSE_INFLUENCE_SIZE"){
        ofxUISlider *slider = e.getSlider();
        mouseInfluenceSize = slider->getScaledValue();
    } else if (name == "MOUSE_INFLUENCE_SCALAR"){
        ofxUISlider *slider = e.getSlider();
        mouseInfluenceScalar = slider->getScaledValue();
    } else if (name == "MOUSE_Z"){
        ofxUISlider *slider = e.getSlider();
        mouseZ = slider->getScaledValue();
    }
    
    // light smoothing
    else if(name == "SMOOTH_LIGHT") {
        ofxUIToggle *toggle = e.getToggle();
        ofSetSmoothLighting(toggle->getValue());
    }
    
    // light position
    else if (name == "LIGHT_POSITION_X"){
        ofxUISlider *slider = e.getSlider();
        lightPosition.x = slider->getScaledValue();
        light.setPosition(lightPosition);
    } else if (name == "LIGHT_POSITION_Y"){
        ofxUISlider *slider = e.getSlider();
        lightPosition.y = slider->getScaledValue();
        light.setPosition(lightPosition);
    } else if (name == "LIGHT_POSITION_Z"){
        ofxUISlider *slider = e.getSlider();
        lightPosition.z = slider->getScaledValue();
        light.setPosition(lightPosition);
    }
    
    // light direction
    else if (name == "LIGHT_DIRECTION_X"){
        ofxUISlider *slider = e.getSlider();
        lightDirection.x = slider->getScaledValue();
        light.setOrientation(lightDirection);
    } else if (name == "LIGHT_DIRECTION_Y"){
        ofxUISlider *slider = e.getSlider();
        lightDirection.y = slider->getScaledValue();
        light.setOrientation(lightDirection);
    } else if (name == "LIGHT_DIRECTION_Z"){
        ofxUISlider *slider = e.getSlider();
        lightDirection.z = slider->getScaledValue();
        light.setOrientation(lightDirection);
    }
        
    // import
    else if (name == "IMPORT"){
        ofxUIButton *btn = e.getButton();
        if (btn->getValue()) importImage();
    }
    
    // export
    else if (name == "EXPORT"){
        ofxUIButton *btn = e.getButton();
        if (btn->getValue()) exportImage();
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if (!animateFlag) return;
    
    // update physics
    float elapsedTime = ofGetElapsedTimef();
    float timeStep = elapsedTime - timeStamp;
    timeStamp = elapsedTime;
    
    for(int i = 0, len = pointMasses.size(); i < len; ++i) {
        pointMasses[i].updatePhysics(timeStep);
        pointMasses[i].solveConstraints(linkStiffnessMul);
        if (mouseInteract) pointMasses[i].updateInteractions(prevMousePosition, mousePosition, mouseInfluenceSize, mouseInfluenceScalar);
    }
    
    prevMousePosition.set(mousePosition);
   
    // update vertices
    vector<ofVec3f>& vertices = mesh.getVertices();

    // iterate over cells
    for (int y = 0; y < rows - 1; y++){
        for (int x = 0; x < columns - 1; x++){
     
            int pmIndex = y * columns + x; // top left pointMass index
            
            int vIndex = pmIndex * 6; // 3 vertices / tri, 6 vertices / cell
            
            //nw
            vertices[vIndex].set(pointMasses[pmIndex].position);
            vertices[vIndex + 3].set(pointMasses[pmIndex].position);
            //ne
            vertices[vIndex + 1].set(pointMasses[pmIndex + 1].position);
            //se
            vertices[vIndex + 2].set(pointMasses[pmIndex + columns + 1].position);
            vertices[vIndex + 4].set(pointMasses[pmIndex + columns + 1].position);
            //sw
            vertices[vIndex + 5].set(pointMasses[pmIndex + columns].position);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    // draw mesh on fbo
    ofSetColor(255, 255, 255);
    
    fbo.begin();
    
    ofClear(255,255,255, 0);
    
    ofEnableAlphaBlending();
    ofEnableLighting();
    
    texture.bind();
    light.enable();
    mesh.draw();
    light.disable();
    texture.unbind();
    
    ofDisableLighting();
    ofDisableAlphaBlending();
    
    fbo.end();
    
    // draw fbo, scaled to fit window
    float sw = ofGetWidth();
    float sh = ofGetHeight();
    float sRatio = sw / sh;
    
    float imw = texture.getWidth();
    float imh = texture.getHeight();
    float imRatio = imw / imh;
    
    float scaleFactor;
    
    if (imRatio > sRatio){
        // fit based on width
        scaleFactor = sw / imw;
    } else {
        // fit based on height
        scaleFactor = sh / imh;
    }
    
    ofPushMatrix();
    ofTranslate((sw - imw * scaleFactor) * 0.5, (sh - imh * scaleFactor) * 0.5);
    ofScale(scaleFactor, scaleFactor);
    
    fbo.draw(0, 0);
    
    ofPopMatrix();
}

// lets us update grid diemnsion and resolution
void ofApp::resetGrid(){
    
    float cellWidth = texture.getWidth() / (float) (columns - 1);
    float cellHeight = texture.getWidth() / (float) (rows - 1);
    
    // create cells
    
    pointMasses.clear();
    
    for (int y = 0; y < rows; y++){
        for (int x = 0; x < columns; x++){
            
            ofVec3f position = ofVec3f(cellWidth * x, cellHeight * y, 0);
            PointMass pm = PointMass(position);
            
            // Pinned if on border
            if ((x * y * (columns - x - 1) * (rows - y - 1)) == 0)
                pm.pinTo(position);
            
            pointMasses.push_back(pm);
        }
    }
    
    // link params
    float stiffness = 1.f;
    float cellDiag = sqrt(cellWidth * cellWidth + cellHeight * cellHeight);
    // create links
    for (int y = 0; y < rows - 1; y++){
        for (int x = 0; x < columns - 1; x++){
            int index = y * columns + x;
            // attach to left node
            pointMasses[index].attachTo(&pointMasses[index + 1], cellWidth, stiffness);
            // attach to bottom node
            pointMasses[index].attachTo(&pointMasses[index + columns], cellHeight, stiffness);
        }
    }
    
    // Build mesh
    mesh.clear();
    
    ofVec3f normal(0, 0, 1);
    
    for (int y = 0; y < rows; y++){
        for (int x = 0; x < columns; x++){
            
			// add face
			ofVec3f nw = ofVec3f(x * cellWidth, y * cellHeight, 0);
			ofVec3f ne = ofVec3f((x + 1) * cellWidth, y * cellHeight, 0);
			ofVec3f sw = ofVec3f(x * cellWidth, (y + 1) * cellHeight, 0);
			ofVec3f se = ofVec3f((x + 1) * cellWidth, (y + 1) * cellHeight, 0);
                       
            addFace(mesh, nw, ne, se, sw);
            
            // add texture coordinates
			ofVec2f nwi(x * cellWidth, y * cellHeight);
			ofVec2f nei((x + 1) * cellWidth, y * cellHeight);
			ofVec2f swi(x * cellWidth, (y + 1) * cellHeight);
			ofVec2f sei((x + 1) * cellWidth, (y + 1) * cellHeight);
			       
            addTexCoords(mesh, nwi, nei, sei, swi);
            
            for (int k = 0; k < 6; k++) mesh.addNormal(normal);
		}
	}
    
    mesh.enableNormals();
}

void ofApp::importImage(){

    cout << "IMPORT!" << endl;
    
    ofFileDialogResult dfr = ofSystemLoadDialog();
    cout << "dfr.getName(): " << dfr.getName() << endl;
    cout << "dfr.getPath(): " << dfr.getPath() << endl;
    
    importImage(dfr.getPath());
}

void ofApp::importImage(string path){
    
    texture.clear();
    pixels.clear();
    
    ofLoadImage(texture, path);
    fbo.allocate(texture.getWidth(), texture.getHeight(), GL_RGBA); // [fbo.destroy] is called in [fbo.allocate]
    pixels.allocate(texture.getWidth(), texture.getHeight(), OF_IMAGE_COLOR);
}

void ofApp::exportImage(){
    
    gui.saveSettings("settings.xml");
    
    cout << "EXPORT!" << endl;
    
    ofFileDialogResult dfr = ofSystemSaveDialog("export.png", "Export image.");
    cout << "dfr.getName(): " << dfr.getName() << endl;
    cout << "dfr.getPath(): " << dfr.getPath() << endl;
    
    pixels.clear();
    fbo.readToPixels(pixels);
    ofSaveImage(pixels, dfr.getPath());    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 32) animateFlag = !animateFlag;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    //mousePosition.set(x, y, mouseZ);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    // TODO: DRY THIS
    ofVec3f mouse(x * texture.getWidth() / ofGetWidth(), y * texture.getHeight() / ofGetHeight(), mouseZ);
    mousePosition.set(mouse);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    mouseInteract = true;
    ofVec3f mouse(x * texture.getWidth() / ofGetWidth(), y * texture.getHeight() / ofGetHeight(), mouseZ);
    mousePosition.set(mouse);
    prevMousePosition.set(mousePosition);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    mouseInteract = false;
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

/*
 from vboExample project
 -----------------------
 These functions are for adding quads and triangles to an ofMesh -- either
 vertices, or texture coordinates.
 */
//--------------------------------------------------------------
void ofApp::addFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c) {
	mesh.addVertex(a);
	mesh.addVertex(b);
	mesh.addVertex(c);
}

//--------------------------------------------------------------
void ofApp::addFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c, ofVec3f d) {
	addFace(mesh, a, b, c);
	addFace(mesh, a, c, d);
}

//--------------------------------------------------------------
void ofApp::addTexCoords(ofMesh& mesh, ofVec2f a, ofVec2f b, ofVec2f c) {
	mesh.addTexCoord(a);
	mesh.addTexCoord(b);
	mesh.addTexCoord(c);
}

//--------------------------------------------------------------
void ofApp::addTexCoords(ofMesh& mesh, ofVec2f a, ofVec2f b, ofVec2f c, ofVec2f d) {
	addTexCoords(mesh, a, b, c);
	addTexCoords(mesh, a, c, d);
}

//--------------------------------------------------------------
ofVec3f ofApp::getVertexFromImg(ofImage& img, int x, int y) {
	return ofVec3f(x - img.getWidth() / 2, y - img.getHeight() / 2, 0);
}