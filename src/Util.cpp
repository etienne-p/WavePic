//
//  Util.cpp
//  WavePic
//
//  Created by Etienne on 2014-10-15.
//
//

#include "Util.h"

/* 
 
 from: http://geomalgorithms.com/a02-_lines.html
 
 // dist_Point_to_Segment(): get the distance of a point to a segment
 //     Input:  a Point P and a Segment S (in any dimension)
 //     Return: the shortest distance from P to S
 float
 dist_Point_to_Segment( Point P, Segment S)
 {
 Vector v = S.P1 - S.P0;
 Vector w = P - S.P0;
 
 double c1 = dot(w,v);
 if ( c1 <= 0 )
 return d(P, S.P0);
 
 double c2 = dot(v,v);
 if ( c2 <= c1 )
 return d(P, S.P1);
 
 double b = c1 / c2;
 Point Pb = S.P0 + b * v;
 return d(P, Pb);
 }
 
*/

float Util::distPointToSegment(ofVec3f segP0, ofVec3f segP1, ofVec3f point){

    ofVec3f v = segP1 - segP0;
    ofVec3f w = point - segP0;
    
    float c1 = w.dot(v);
    if ( c1 <= 0 )
        return point.distance(segP0);
    
    float c2 = v.dot(v);
    if ( c2 <= c1 )
        return point.distance(segP1);
    
    float b = c1 / c2;
    ofVec3f Pb = segP0 + b * v;
    return point.distance(Pb);
}
