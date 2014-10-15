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
    damping = 20.f;
    pinned = false;
}

void PointMass::updatePhysics(float timeStep_){
    
    // applyForce(0, mass * gravity, 0); // start without gravity
    
    float velX = x - lastX;
    float velY = y - lastY;
    
    ofVec3f velocity = (position - lastPosition) * 0.99;
    
    float timeStepSq = timeStep_ * timeStep_;
    
    // calculate the next position using Verlet Integration
    ofVec3f nextPosition = position + velocity + (0.5 * timeStepSq) * acceleration;
    
    // reset variables
    lastPosition.set(position);
    position.set(nextPosition);
    acceleration.set(0, 0, 0);
}

void PointMass::updateInteractions(
    ofVec3f prevMousePosition,
    ofVec3f mousePosition,
    float mouseInfluenceSize,
    float mouseInfluenceScalar){
    
    float distance = Util.distPointToSegment(prevMousePosition, mousePosition, position);
    
    if (distanceSquared < mouseInfluenceSize) {
        // To change the velocity of our PointMass, we subtract that change from the lastPosition.
        // When the physics gets integrated (see updatePhysics()), the change is calculated
        // Here, the velocity is set equal to the cursor's velocity
        lastPosition = position - (mousePosition - prevMousePosition) * mouseInfluenceScalar;
    }
}

void PointMass::solveConstraints(){
    
    for(vector<Link>::iterator it = links.begin(); it != links.end(); ++it) {
        it->solve();
    }
    
    if (pinned) position.set(pinPosition);
}

void PointMass::attachTo(PointMass pm_, float restingDist_, float stiff_, float tearSensitivity_){
    Link link = new Link(this, pm_, restingDist_, stiff_, tearSensitivity_);
    links.push_back(link);
}

void PointMass::removeLink(Link link){
    Link* ptr = find(links.begin(), links.end(), link);
    myvector.erase(ptr);
}

void PointMass::applyForce(ofVec3f force_){
    acceleration += force / mass;
}

void PointMass::pinTo(ofVec3f position_){
    pinned = true;
    pinPosition.set(position_);
}
