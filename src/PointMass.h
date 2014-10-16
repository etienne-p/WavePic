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
#include "Util.h"


class PointMass {
    
    typedef struct {
        PointMass * p1;
        PointMass * p2;
        float restingDistance;
        float stiffness;
    } Link;

    
public:
    
    float mass;
    
     ofVec3f position;
    
    void updatePhysics(float timeStep_);
    
    void updateInteractions(ofVec3f prevMousePosition, ofVec3f mousePosition, float mouseInfluenceSize, float mouseInfluenceScalar);
    
    void solveConstraints(float stiffnessMul);
    
    void attachTo(PointMass* P_, float restingDist_, float stiff_);
    
    void solveLink(Link* link, float stiffnessMul);
    
    void applyForce(ofVec3f force_);
    
    void pinTo(ofVec3f position_);
    
    PointMass(ofVec3f position_);
    
    ~PointMass();

private:
    
    ofVec3f lastPosition, acceleration, pinPosition;
    
    bool pinned;
    
    vector <Link> links;
    
};

#endif /* defined(__WavePic__PointMass__) */
