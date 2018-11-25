//
// C++ Interface: robotfactory
//
// Description: 
//
//
// Author: Margus Ernits <margus.ernits@itcollege.ee>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ROBOTFACTORY_H
#define ROBOTFACTORY_H
#include "abstractrobot.h"
#include "robots/neve/neve.h"
#include "robots/neve/nevetest.h"


/**
	@author Margus Ernits <margus.ernits@itcollege.ee>
*/
class RobotFactory{
public:
   // RobotFactory();

   // ~RobotFactory();
  static AbstractRobot *getRobot(const char *);

};

#endif
