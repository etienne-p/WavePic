//
//  Link.h
//  WavePic
//
//  Created by Etienne on 2014-10-15.
//
//

#ifndef __WavePic__Link__
#define __WavePic__Link__

#include "ofMain.h"
#include "PointMass.h"

class Link {
    
public:
    
    float restingDistance, stiffness, tearSensitivity;
    
    void solve();
    
    Link(PointMass p1_, PointMass p2_, float restingDist_, float stiff_, float tearSensitivity_);
    
private:
    
    PointMass p1, p2;
    
};

#endif /* defined(__WavePic__Link__) */
