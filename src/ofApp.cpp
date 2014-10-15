#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    // TODO: refactor flag
    mouseMovedFlag = false;
    mouseZ = 20.f;
    
    float cellSide = 40.f;
    
    columns = (int) (ofGetWidth() / cellSide);
    rows = (int) (ofGetHeight() / cellSide);
    mouseInfluenceSize = 50.f;
    mouseInfluenceScalar = .1f;
    timeStamp = ofGetElapsedTimef();
    
    // create cells
    
    for (int y = 0; y < rows; y++){
        for (int x = 0; x < columns; x++){
            
            ofVec3f position = ofVec3f(cellSide * x, cellSide * y, 0);
            PointMass pm = PointMass(position);
            
            // Pinned if on border
            if ((x * y * (columns - x - 1) * (rows - y - 1)) == 0)
                pm.pinTo(position);
            
            pointMasses.push_back(pm);
        }
    }
    
    // link params
    float stiffness = 0.2f;
    
    // create links
    for (int y = 0; y < rows - 1; y++){
        for (int x = 0; x < columns - 1; x++){
            int index = y * columns + x;
            // attach to left node
            pointMasses[index].attachTo(&pointMasses[index + 1], cellSide, stiffness);
            // attach to bottom node
            pointMasses[index].attachTo(&pointMasses[index + columns], cellSide, stiffness);
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
    
    //float dx = imgWidth / columns;
    //float dy = imgHeight / rows;
    
    // Build mesh
    // TODO: Refactor: put in the same loop that link creation?
    for (int y = 0; y < rows; y++){
        for (int x = 0; x < columns; x++){
            
			/*
			 To construct a mesh, we have to build a collection of quads made up of
			 the current pixel, the one to the right, to the bottom right, and
			 beneath. These are called nw, ne, se and sw. To get the texture coords
			 we need to use the actual image indices.
			 */
            
            // add face
			ofVec3f nw = ofVec3f(x * cellSide, y * cellSide, 0);
			ofVec3f ne = ofVec3f((x + 1) * cellSide, y * cellSide, 0);
			ofVec3f sw = ofVec3f(x * cellSide, (y + 1) * cellSide, 0);
			ofVec3f se = ofVec3f((x + 1) * cellSide, (y + 1) * cellSide, 0);
            
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
    float elapsedTime = ofGetElapsedTimef();
    float timeStep = elapsedTime - timeStamp;
    timeStamp = elapsedTime;
    
    for(int i = 0, len = pointMasses.size(); i < len; ++i) {
        pointMasses[i].updatePhysics(timeStep);
        pointMasses[i].solveConstraints();
        if (mouseMovedFlag) pointMasses[i].updateInteractions(prevMousePosition, mousePosition, mouseInfluenceSize, mouseInfluenceScalar);
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
    
    ofBackgroundGradient(ofColor(64), ofColor(0));
	
    ofSetColor(255, 255, 255);
    // bind texture
    img.bind();
    // draw mesh
	mesh.draw();
	img.unbind();
    
    ofSetColor(255, 0, 0);
    
    for(int i = 0, len = pointMasses.size(); i < len; ++i) {
        ofCircle(pointMasses[i].position.x, pointMasses[i].position.y, 2.f);
    }
    
    

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    mouseMovedFlag = true;
    mousePosition.set(x, y, mouseZ);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    mouseMovedFlag = true;
    mousePosition.set(x, y, mouseZ);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    mouseMovedFlag = true;
    mousePosition.set(x, y, mouseZ);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    mouseMovedFlag = true;
    mousePosition.set(x, y, mouseZ);
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