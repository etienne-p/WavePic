//
//  Link.cpp
//  WavePic
//
//  Created by Etienne on 2014-10-15.
//
//

#include "Link.h"

Link::Link(PointMass p1_, PointMass p2_, float restingDist_, float stiff_, float tearSensitivity_){
    p1 = p1_;
    p2 = p2_;
    restingDistance = restingDist_;
    stiffness = stiff_;
    tearSensitivity = tearSensitivity_;
}

void Link::solve(){

    // calculate the distance between the two PointMass
    ofVec3f diff = p1 - p2;
    float d = diff.length();

    // find the difference, or the ratio of how far along the restingDistance the actual distance is.
    float difference = (restingDistance - d) / d;
    
    // if the distance is more than curtainTearSensitivity, the cloth tears
    if (d > tearSensitivity)
        p1.removeLink(this);
    
    // Inverse the mass quantities
    float im1 = 1 / p1.mass;
    float im2 = 1 / p2.mass;
    float scalarP1 = (im1 / (im1 + im2)) * stiffness;
    float scalarP2 = stiffness - scalarP1;
    
    // Push/pull based on mass
    // heavier objects will be pushed/pulled less than attached light objects
    p1 += diff * (scalarP1 * difference);
    p2 -= diff * (scalarP2 * difference);
}