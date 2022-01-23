#ifndef CDH_H
#define CDH_H
#include "arduino.h"

class cdh
{
public:
  cdh(int heater, bool enabled=false);
  void DOSETUP();
  void DOMAINLOOP();
  
  void HEATERON();
  void HEATEROFF();
  void HEATERTEMPUP();
  void HEATERTEMPDOWN();

  void SETTEMP();
  void DODEFAULTWIDGETS();
  
  void READTEMPHUMSENSOR();
  void CHECKAUTOSTART();
  void SENDSENSOR();

  void CLIMATAUTO();
  void AUTOTEMPSETTING();
  void DOORALARM();
  
  void CHECKDOOR();

  float _H;
  float _T;
  int _h1enabled;
  int _h2enabled;
  int _h2offled;
  float _curTempSet = 20;

  unsigned long int _timenow = 0;
  long int _autostartshutdowntime = 60; //min till autostart auto shutdown

  int _doorbell;
  bool _doorsensor = false;
  const int _doortrigpin = D2;

  int _autostarttemp;
  bool _autostartenable = false;
  bool _autostartactive = false;
  unsigned long int _autostarttime = 0;

  int _heater;

  
};



#endif
