//
// C++ Implementation: robotfactory
//
// Description: 
//
//
// Author: Valdur Kaldvee, Margus Ernits <margus.ernits@itcollege.ee>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "robotfactory.h"
#include <string>
#include <iostream> //TODO remove after debug

using namespace std;

/*RobotFactory::RobotFactory()
{
}


RobotFactory::~RobotFactory()
{
}


*/

/*!
    \fn RobotFactory::getRobot(const * name)
 */
AbstractRobot  *RobotFactory::getRobot(const char * name)
{
    if (name!=NULL){
        string str_name(name);
        if(str_name=="neve"){
            return new Neve;
        }else if(str_name=="nevetest"){
            return new NeveTest;
        }

    }
      return new Neve;
}
