#ifndef CFD_H 
#define CFD_H

#define ChannelNumber 16

#include "CAENVMElib.h"
#include <map>

#include <iostream>

// Interface for CAEN V812 CFD

typedef enum {
  CH0TO7  =0,
  CH8TO15 =1,
} channel_range;

/*! \class CFD
 * \brief Class defining CAEN V812 CFDs
 *
 *  This class initializes CAEN V812 CFD on the CAEN V1718 VME bridge.
 */
class CFD {
 public:
  // baseaddr : VME base address of the module
 /*!\brief Constructor
  *
  * CFD class constructor
  * \param[in] bhandle handle that identifies the bridge
  * \param[in] baseaddr physical address of the CFD module
  */
  CFD(int32_t bhandle,int baseaddr);
 /*!\brief Set the threshold
  *
  * Sets the threshold value on a channel
  * \param[in] channel channel number on the module (0-15)
  * \param[in] new value for the threshold (0-255)
  * \return 0 on success
  * \return -1 on error
  */ 
  int setThreshold(int channel, int value);
  /*!\brief Set POI
  *
  * Sets the pattern of inhibit to define enabled/disabled channels on the module
  * \param[in] word 16-bit word stating the enabled/disabled channels
  * \return 0 on success
  * \return -1 on error
  */  
  int setActiveChannels(int word); // Set channels to be activated
 /*!\brief Set output width
  *
  * Sets the output width for each channel range
  * \param[in] channelRange channel range (CH0TO7 or CH8TO15)
  * \param[in] width final width (0-255)
  * \return 0 on success
  * \return -1 on error
  */ 
  //FIXME put values in ns in a global struct (commons??)
  int setOutWidth(channel_range channelRange, int width);
   /*!\brief Set dead time
  *
  * Sets the dead time for each channel range
  * \param[in] channelRange channel range (CH0TO7 or CH8TO15)
  * \param[in] deadTime final deadTime (0-255)
  * \return 0 on success
  * \return -1 on error
  */ 
  int setDeadTime(channel_range channelRange, int deadTime);
 /*!\brief Get module info
  *
  * Gets CFD module informations (serial number, manufacturer number)
  * \param[in] opts Which information
  * \return data on success
  * \return -1 on error
  */ 
  int getInfos(int);
  int setWidth(int width, int group);
private:
  int baseaddr;
  int32_t bhandle;
  CVAddressModifier am; // Address modifier
  int number;
  int mask[ChannelNumber];
  int masked[ChannelNumber];
  int maskedZero[ChannelNumber];
  int maskedOne[ChannelNumber];
};

#endif /* CFD_H */
