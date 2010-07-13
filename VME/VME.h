//! \file VME.h
//! \brief Master file for VME Process 
//! \author
//! \version
//! \date 15 july 2009

#ifndef VME_H 
#define VME_H 

#include "Bridge.h"
#include "Scaler.h"
#include "CFD.h"

#include "../commons/Queue.h"
#include "../commons/ShareMem.h"
#include "../commons/const.h"
#include "../commons/data_format.h"

#include <string>
#include <iostream>
#include <fstream> // file manipulation
#include <cmath>

// FIXME C++ Style !
#include <string.h>

/*! \class VME
 * \brief Class defining CAEN V1718 VME module
 *
 *  This class creates CAEN V1718 VME bridge, scaler module and CFD's object.
 */

class VME {
 public:
/*!
 * \brief Constructor
 *
 * VME class constructor
 *
 */
  VME();
/*!
 * \brief Destructor
 *
 * VME class Destructor
 *
 */
  ~VME();
/*!
 * \brief Messages dispatcher
 *
 * Dispatches messages from control and GUI modules to CFDs, TDCs, etc.
 * 
 */
  int Run();
 private:
/*!
 * \brief Read channel on Scaler
 *
 * Obsolete method used to read values on each channel for the Scaler module.
 * \deprecated Obsolete method!
 */
  void ReadChannel(int opts);
/*!
 * \brief Reset channels on Scaler
 *
 * Obsolete method used to reset values on each channel for the Scaler module.
 * \deprecated Obsolete method!
 */
  void ResetAll();
/*!
 * \brief Read local CFDs configuration
 *
 * Fetches configuration data from config files and emits them to each CFD.
 * 
 */
  void ReadAllCFDConfig();

  int32_t bhandle; //Move in Bridge.cpp
  Bridge *bridge;
  Scaler *scaler;
  CFD *cfd1;
  CFD *cfd2;
  CFD *cfd3;
  CFD *cfd4;
  int panel;
  Queue *toVME;
  Queue *toGUI;
  Queue *toControl;
  ShareMem *data;
  ShareMem *control;
};


#endif /* VME_H */
