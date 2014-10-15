//
//  PointMass.h
//  WavePic
//
//  Created by Etienne on 2014-10-15.
//
//

#ifndef __WavePic__PointMass__
#define __WavePic__PointMass__

#include "ofMain.h"
#include "Link.h"

class PointMass {
    
public:
    
    float mass, damping;
    
    void updatePhysics(float timeStep_);
    
    void updateInteractions();
    
    void solveConstraints();
    
    void attachTo(PointMass P_, float restingDist_, float stiff_, float tearSensitivity_);
    
    void removeLink();
    
    void applyForce(ofVec3f force_);
    
    void pinTo(ofVec3f position_);
    
    PointMass(ofVec3f position_);
    
    // ~PointMass();

private:
    
    ofVec3f lastPosition, position, acceleration, pinPosition;
    
    bool pinned;
    
    vector <Link> links;
    
    
    
    
};

#endif /* defined(__WavePic__PointMass__) */
