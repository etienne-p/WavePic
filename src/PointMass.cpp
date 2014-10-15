//
//  PointMass.cpp
//  WavePic
//
//  Created by Etienne on 2014-10-15.
//
//

#include "PointMass.h"

PointMass::PointMass(ofVec3f position_){
    position.set(position_);
    lastPosition.set(position_);
    acceleration.set(0, 0, 0);
    mass = 1.f;
    pinned = false;
}

void PointMass::updatePhysics(float timeStep_){
    
    // applyForce(0, mass * gravity, 0); // start without gravity
    
     ofVec3f velocity = (position - lastPosition) * 0.99;
    
    float timeStepSq = timeStep_ * timeStep_;
    
    // calculate the next position using Verlet Integration
    ofVec3f nextPosition = position + velocity + (0.5 * timeStepSq) * acceleration;
    
    // reset variables
    lastPosition.set(position);
    position.set(nextPosition);
    acceleration.set(0, 0, 0);
}

void PointMass::updateInteractions(ofVec3f prevMousePosition, ofVec3f mousePosition, float mouseInfluenceSize, float mouseInfluenceScalar) {
    
    float distance = Util::distPointToSegment(prevMousePosition, mousePosition, position);
    
    if (distance < mouseInfluenceSize) {
        // To change the velocity of our PointMass, we subtract that change from the lastPosition.
        // When the physics gets integrated (see updatePhysics()), the change is calculated
        // Here, the velocity is set equal to the cursor's velocity
        lastPosition = position - (mousePosition - prevMousePosition) * mouseInfluenceScalar;
    }
}

void PointMass::solveConstraints(){
    
    for (int i = 0, len = links.size(); i < len; ++i){
        solveLink(&links[i]);
    }

    if (pinned) position.set(pinPosition);
}

void PointMass::attachTo(PointMass * pm_, float restingDist_, float stiff_){
    Link link = {this, pm_, restingDist_, stiff_};
    links.push_back(link);
}

void PointMass::solveLink(Link * link){
    
    PointMass* p1 = link->p1;
    PointMass* p2 = link->p2;
    
    // calculate the distance between the two PointMass
    ofVec3f diff = p1->position - p2->position;
    
    float d = diff.length();
    
    // find the difference, or the ratio of how far along the restingDistance the actual distance is.
    float difference = (link->restingDistance - d) / d;
    
    // Inverse the mass quantities
    float im1 = 1 / p1->mass;
    float im2 = 1 / p2->mass;
    float scalarP1 = (im1 / (im1 + im2)) * link->stiffness;
    float scalarP2 = link->stiffness - scalarP1;
    
    // Push/pull based on mass
    // heavier objects will be pushed/pulled less than attached light objects
    p1->position += diff * (scalarP1 * difference);
    p2->position -= diff * (scalarP2 * difference);
}

void PointMass::applyForce(ofVec3f force_){
    acceleration += (force_ / mass);
}

void PointMass::pinTo(ofVec3f position_){
    pinned = true;
    pinPosition.set(position_);
}
