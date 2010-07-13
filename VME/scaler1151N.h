// Interface for LeCroy 1151N Scaler

#ifndef SCALER1151N_H 
#define SCALER1151N_H

#include "CAENVMElib.h"
#include <map>

/*! \class Scaler
 * \brief Class defining LeCroy 1151N Scaler
 *
 *  This class initializes LeCroy 1151N Scaler on the CAEN V1718 VME bridge
 */
 
class scaler1151N {
 public:
 /*!
  *  \brief Constructor
  *
  *  Scaler class Constructor
  *  \param[in] bhandle handle that identifies the bridge
  *  \param[in] baseaddr physical VME base address of the module
  */
  scaler1151N(int32_t bhandle,int baseaddr);
  ~scaler1151N();
 /*!
  *  \brief Read channel
  *
  *  Read channel (1-16) of the scaler module
  *  \param[in] channel channel number (1-16)
  */
  int readChannel(int channel);
 /*!
  * \brief Reset all channel
  * 
  * Reset all channel (1-16) of the scaler module
  */ 
  int resetAll();
 private:
  int baseaddr;
  int32_t bhandle;
  CVAddressModifier am; // Address modifier
  std::map<int,int> read; // channel -> address
  std::map<int,int> readreset; // channel -> address
  std::map<int,int> reset; // channel -> address
};

#endif // SCALER1151_H
