//
// C++ Interface: objecttype
//
// Description: 
//
//
// Author: Valdur Kaldvee, Margus Ernits <margus.ernits@itcollege.ee>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OBJECTTYPE_H
#define OBJECTTYPE_H

/**
	@author Valdur Kaldvee, Margus Ernits <margus.ernits@itcollege.ee>
*/
class ObjectType{
public:
    ObjectType();

    ~ObjectType();
    int hMin;
    int hMax;
    int sMin;
	int sMax;
    int lMin;
    int lMax;

    void setHMin(int value){
        this->hMin = value;
    }

    void setHMax(int value){
        this->hMax = value;
    }

    int getHMin(){
        return this->hMin;
    }

    int getHMax(){
        return this->hMax;
    }
};

#endif
