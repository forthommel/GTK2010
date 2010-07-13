#include "scaler1151N.h"

scaler1151N::scaler1151N(int32_t bhandle, int baseaddr) {
  int i;    
  this->bhandle = bhandle;
  this->baseaddr = baseaddr;
  this->am = cvA24_U_DATA; // Address modifier
  // Setting the map channel -> address for reading
  for(i = 0; i < 16; ++i) {
    read[i+1] = baseaddr + 0x80 + i*0x4;  
  }
  // Setting the map channel -> address for reading (w/ reset)
  for(i = 0; i < 16; ++i) {
    readreset[i+1] = baseaddr + 0x40 + i*0x4;
  }
  // Setting the map channel -> address for reset
  for(i = 0; i < 16; ++i) {
    reset[i+1] = baseaddr + 0x40 + i*0x4;    
  }
}

int scaler1151N::readChannel(int channel) {
  int32_t data = 0;
  int addr = read[channel];
#ifdef DEBUG
  //printf("[VME] <scaler1151N::readChannel> : Debug : readChannel at 0x%08x (channel %d)\n",addr,channel);
#endif
  if(addr == 0) {
    printf("[VME] <scaler1151N::readChannel> : Error : Unknown channel %i\n",channel);
    return -1;
  }
  if(CAENVME_ReadCycle(bhandle,addr,&data,am,cvD32) != cvSuccess) {
    printf("[VME] <scaler1151N::readChannel> : Error : Read at VME address 0x%08x\n",addr);       
    return -1;
  }    
#ifdef DEBUG
 // printf("[VME] <scaler1151N> : Debug : Count %u\n",data);
#endif
  return data;
}





int scaler1151N::resetAll() {
  int16_t data = 0x0000; // 16 bits
  int addr = baseaddr + 0x00; 
#ifdef DEBUG
  printf("[VME] <scaler1151N::resetAll> : Debug : resetAll\n");
#endif
  if(CAENVME_WriteCycle(bhandle,addr,&data,am,cvD16) != cvSuccess) {
    printf("[VME] <scaler1151N::resetAll> : Error : Write at VME address 0x%08x\n",addr);
    return -1;
  }
  return 0;
};
