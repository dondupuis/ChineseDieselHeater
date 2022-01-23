#include "cdh.h"
#include "arduino.h"
//******************************************************************
//   Create Class-Objects
//******************************************************************
cdh CDH(1, false);
cdh CDH2(2, false);

void setup()
{
  CDH.DOSETUP();
}

void loop()
{
  CDH.DOMAINLOOP();
  //timer.setInterval(60000L, CDH.CHECKAUTOSTART);
  //checkDoor();
  CDH.CHECKDOOR();
  CDH.SENDSENSOR();
  //Blynk.run();
  //timer.run();
}
