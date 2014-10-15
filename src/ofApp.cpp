#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    float cellSide = 10.f;
    
    columns = ofGetWidth() / cellSide;
    rows = ofGetHeight() / cellSide;
    mouseInfluenceSize = 50.f;
    mouseInfluenceScalar = 1.f;
    timeStamp = ofGetElapsedTimef();
    
    // create cells
    for (int x = 0; x < columns; ++x){
        for (int y = 0; y < rows; ++y){
            
            ofVec3f position = ofVec3f(cellSide * x, cellSide * y, 0);
            PointMass pm = PointMass(position);
            
            // Pinned if on border
            if ((x * y * (x - columns - 1) * (y - rows - 1)) == 0)
                pm.pinTo(position);
            
            pointMasses.push_back(pm);
        }
    }
    
    // link params
    float restingDistance = cellSide;
    float stiffness = 0.9f;
    
    // create links
    for (int x = 0; x < columns - 1; ++x){
        for (int y = 0; y < rows - 1; ++y){
            int index = y * rows + x;
            // attach to left node
            pointMasses[index].attachTo(&pointMasses[index + 1], restingDistance, stiffness);
            // attach to bottom node
            pointMasses[index].attachTo(&pointMasses[index + columns], restingDistance, stiffness);
        }
    }
    
    #ifdef TARGET_OPENGLES
    // While this will will work on normal OpenGL as well, it is
    // required for OpenGL ES because ARB textures are not supported.
    // If this IS set, then we conditionally normalize our
    // texture coordinates below.
    ofEnableNormalizedTexCoords();
    #endif
    
	img.loadImage("cover.png");
	
	// OF_PRIMITIVE_TRIANGLES means every three vertices create a triangle
	mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    int imgWidth = img.getWidth();
	int imgHeight = img.getHeight();
    ofVec2f imageSize(imgWidth, imgHeight);
    
    float dx = imgWidth / columns;
    float dy = imgHeight / rows;
    
    // Build mesh
    // TODO: Refactor: put in the same loop that link creation?
    for (int x = 0; x < columns - 1; ++x){
        for (int y = 0; y < rows - 1; ++y){
            
			/*
			 To construct a mesh, we have to build a collection of quads made up of
			 the current pixel, the one to the right, to the bottom right, and
			 beneath. These are called nw, ne, se and sw. To get the texture coords
			 we need to use the actual image indices.
			 */
            
            // add face
			ofVec3f nw = getVertexFromImg(img, x * cellSide, y * cellSide);
			ofVec3f ne = getVertexFromImg(img, (x + 1) * cellSide, y * cellSide);
			ofVec3f sw = getVertexFromImg(img, x * cellSide, (y + 1) * cellSide);
			ofVec3f se = getVertexFromImg(img, (x + 1) * cellSide, (y + 1) * cellSide);
            
            addFace(mesh, nw, ne, se, sw);
            
            // add texture coordinates
			ofVec2f nwi(x * cellSide, y * cellSide);
			ofVec2f nei((x + 1) * cellSide, y * cellSide);
			ofVec2f swi(x * cellSide, (y + 1) * cellSide);
			ofVec2f sei((x + 1) * cellSide, (y + 1) * cellSide);
			
			// Normalize our texture coordinates if normalized
            // texture coordinates are currently enabled.
            if(ofGetUsingNormalizedTexCoords()) {
                nwi /= imageSize;
                nei /= imageSize;
                sei /= imageSize;
                swi /= imageSize;
            }
            
            addTexCoords(mesh, nwi, nei, sei, swi);
		}
	}
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // update physics
    /*
    float elapsedTime = ofGetElapsedTimef()
    float timeStep = elapsedTime - timeStamp;
    timeStamp = elapsedTime;
    
    for(vector<PointMasse>::iterator it = pointMasses.begin(); it != pointMasses.end(); ++it) {
        it->updatePhysics(timeStep);
    }
    */
    // update vbo
    
    // TMP: start with random animation so we know we're able to animate vertices
    vector<ofVec3f>& vertices = mesh.getVertices();
    
    for(vector<ofVec3f>::iterator it = vertices.begin(); it != vertices.end(); ++it) {
        it->x += ofRandom(2.0);
        it->y += ofRandom(2.0);
        it->z += ofRandom(2.0);
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackgroundGradient(ofColor(64), ofColor(0));
	
    // bind texture
    img.bind();
    // draw mesh
	mesh.draw();
	img.unbind();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
    // TODO: interact with grid
    

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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