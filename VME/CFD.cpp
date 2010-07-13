#include <math.h> // pow
#include "CFD.h"

CFD::CFD(int32_t bhandle, int baseaddr) {
  this->bhandle = bhandle;
  this->baseaddr = baseaddr;
  this->am = cvA32_U_DATA; // Address modifier FIXME move to Bridge class !
}


int CFD::setThreshold(int channel, int value){ // Set threshold values on each channel
  /*if (channel == ChannelNumber) { // set threshold on all channels
    int addr[ChannelNumber];
    for (int i = 0; i < ChannelNumber; ++i) {
      addr[i] = baseaddr + i*0x02;
      if (CAENVME_WriteCycle(bhandle,addr[i],&value,am,cvD16) != cvSuccess) {
	      std::cout << "[VME] <CFD::setThreshold> ERROR: Write at VME address " << std::hex << addr << std::endl;
	      return -1;
      }
    }
#ifdef DEBUG
    std::cout << "[VME] <CFD::setThreshold> Debug: setThreshold (all channels, value " << value << ")" << std::endl;
#endif
  }*/
  //else { // set threshold on one channel
    int addr = baseaddr + channel*0x02;
    if (CAENVME_WriteCycle(bhandle,addr,&value,am,cvD16) != cvSuccess) {
      std::cout << "[VME] <CFD::setThreshold> ERROR: Write at VME address " << std::hex << addr << std::endl;
      return -1;
    }
#ifdef DEBUG
    std::cout << "[VME] <CFD::setThreshold> Debug: setThreshold (channel " << channel << ", value " << value << ")" << std::endl;
#endif
  //}
  return 0;
}


int CFD::setActiveChannels(int word){ // Set channels to be activated
  int addr = baseaddr + 0x4A; // Pattern of inhibit register on CFD module
  number = word; // FIXME: must be an integer -> Conditions on argument
  if ((number > pow(2,ChannelNumber)-1) || (number < 0)){
    std::cout << "[VME] <CFD::setActiveChannels> ERROR: Invalid set of channels!" << std::endl;
    return -1;
  }
#ifdef DEBUG
  // Displays enabled and disabled channels on debug mode
  for(int i = 0; i < ChannelNumber; ++i){
    mask[i] = pow(2,i);
  }
  for(int i = 0; i < ChannelNumber; ++i){
    masked[i] = (number & mask[i])/mask[i];
    if (masked[i] == 0) maskedZero[i] = 1;
    else maskedZero[i] = 0;
    if (masked[i] == 1) maskedOne[i] = 1;
    else maskedOne[i] = 0;
  }
  std::cout << "[VME] <CFD::setActiveChannels> : Enabled channels :  ";
  for(int i = 0; i < ChannelNumber; ++i){
    if (maskedOne[i] == 1) std::cout << i+1 << " ";
  }
  std::cout << "\n[VME] <CFD::setActiveChannels> : Disabled channels : ";
  for(int i = 0; i < ChannelNumber; ++i){
    if (maskedZero[i] == 1) std::cout << i+1 << " ";
  }
  std::cout << "\n";
#endif
  int32_t data = number;
  if(CAENVME_WriteCycle(bhandle,addr,&data,am,cvD16) != cvSuccess) {
    std::cout << "[VME] <CFD::setActiveChannels> ERROR: Write at VME address 0x" << std::hex << addr << std::endl;       
    return -1;
  }


return 0;
}


int CFD::setOutWidth(channel_range channelRange, int width) { // Set out width on each channel
  int addr;
  //char cRname;

  switch (channelRange){
    case CH0TO7:
      addr = baseaddr + 0x40; // Output width register Ch. 0 to 7
      //cRname = "0-7";
      break;
    case CH8TO15:
      addr = baseaddr + 0x42; // Output width register Ch. 8 to 15
      //cRname = "8-15";
      break;
    default:
      std::cout << "[VME] <CFD::setOutWidth> ERROR: Unknown channel range (recognized parameters are 1 or 2)" << std::endl;
      return -1;
    }
#ifdef DEBUG
  std::cout << "[VME] <CFD::setOutWidth> Debug: setOutWidth (channel range " << channelRange << ", value " << width << ")" << std::endl;
#endif
  if(CAENVME_WriteCycle(bhandle,addr,&width,am,cvD16) != cvSuccess) {
    std::cout << "[VME] <CFD::setActiveChannels> ERROR: Write at VME address 0x" << std::hex << addr << ")" << std::endl;       
    return -1;
  }
  return 0;
}

int CFD::setDeadTime(channel_range channelRange, int deadTime){ // Set dead time on each channel
  int addr;
  //char cRname;

  switch (channelRange){
    case CH0TO7:
      addr = baseaddr + 0x44; // Dead Time register Ch. 0 to 7
      //cRname = "0-7";
      break;
    case CH8TO15:
      addr = baseaddr + 0x46; // Dead Time register Ch. 8 to 15
      //cRname = "8-15";
      break;
    default:
      std::cout << "[VME] <CFD::setDeadTime> ERROR: Unknown channel range (recognized parameters are 1 or 2)" << std::endl;
      return -1;
    }
#ifdef DEBUG
  std::cout << "[VME] <CFD::setDeadTime> Debug: setDeadTime (channel range " << channelRange << ", value " << deadTime << ")" << std::endl;
  // std::cout.put(char xxx) ?
#endif
  if(CAENVME_WriteCycle(bhandle,addr,&deadTime,am,cvD16) != cvSuccess) {
    std::cout << "[VME] <CFD::setDeadTime> ERROR: Write at VME address 0x" << std::hex << addr << std::endl;       
    return -1;
  }
  return 0;
}

int CFD::getInfos(int opts) {
  int addr1 = baseaddr+0xFE;
  int addr2 = baseaddr+0xFC;
  int data = 0;
  #ifdef DEBUG
  printf("[VME] <CFD::getInfos> Debug: getInfos at " << std::hex << addr1 << " and " << std::hex << addr2 << std::endl;
  #endif
  if (opts == 0) {
    if (CAENVME_ReadCycle(bhandle,addr1,&data,am,cvD16) != cvSuccess) {
      std::cout << "[VME] <CFD::getInfos> ERROR: Read at " << std::hex << addr1 << " (Serial number and version)" << std::endl;
      return -1;
    }
  }
  else if (opts == 1) {
    if (CAENVME_ReadCycle(bhandle,addr2,&data,am,cvD16) != cvSuccess) {
      std::cout << "[VME] <CFD::getInfos> ERROR: Read at " << std::hex << addr2 << " (Type and manufacturer number)" << std::endl;
      return -1;
    }
  }
  else {
    std::cout << "[VME] <CFD::getInfos> ERROR: wrong parameter given by GUI: " << opts << std::endl;
  }
  return data;
}
