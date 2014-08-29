#include "VME/bridgeV1718.h"

bridgeV1718::bridgeV1718(const char *device) {
  int dev;
  dev = atoi(device); 
  if(CAENVME_Init(cvV1718,0,dev, &bhandle) != cvSuccess) {
    std::cout << "[VME] <Bridge::Bridge> ERROR: Opening the device" << std::endl;
    //exit(1);
  }
  #ifdef DEBUG
  std::cout << "[VME] <Bridge::Bridge> Debug: BHandle " << (int)bhandle << std::endl;
  #endif
  
  //Map output lines [0,4] to corresponding register.
  map_port[cvOutput0] = cvOut0Bit;
  map_port[cvOutput1] = cvOut1Bit;
  map_port[cvOutput2] = cvOut2Bit;
  map_port[cvOutput3] = cvOut3Bit;
  map_port[cvOutput4] = cvOut4Bit;
}

int32_t bridgeV1718::getBHandle() {
  #ifdef DEBUG
  std::cout << "[VME] <Bridge::getBHandle> Debug: BHandle " << (int)bhandle << std::endl;
  #endif
  return bhandle;
}

bridgeV1718::~bridgeV1718() {
  CAENVME_End(bhandle);
}

// output := cvOutput[0,4] 
int bridgeV1718::outputConf(CVOutputSelect output) {
  if(CAENVME_SetOutputConf(bhandle,output,cvDirect,cvActiveHigh,cvManualSW) != cvSuccess) {
    std::cout << "[VME] <Bridge::outputConf> ERROR: config of output " << (int)output << " failed" << std::endl; 	
    return -1;
  }
  return 0;
}

// output := cvOutput[0,4]
int bridgeV1718::outputOn(CVOutputSelect output) {
  if(CAENVME_SetOutputRegister(bhandle,map_port[output]) != cvSuccess) {
    std::cout << "[VME] <Bridge::outputOn> ERROR: set register failed" << std::endl;
    return -1;
  }
  return 0;
}
int bridgeV1718::outputOff(CVOutputSelect output) {	
  if(CAENVME_ClearOutputRegister(bhandle,map_port[output]) != cvSuccess) {
    std::cout << "[VME] <Bridge::outputOn> ERROR: set register failed" << std::endl;
    return -1;
  }
  return 0;
}

// input := cvInput[0,1]
int bridgeV1718::inputConf(CVInputSelect input) {
  if(CAENVME_SetInputConf(bhandle,input,cvDirect,cvActiveHigh) != cvSuccess) {
    std::cout << "[VME] <Bridge::inputConf> ERROR: config of input " << (int)input << " failed" << std::endl;
    return -1;
  }
  return 0;
}

int bridgeV1718::inputRead(CVInputSelect input) {
	unsigned int data;
	if(CAENVME_ReadRegister(bhandle,cvInputReg,&data) != cvSuccess) {
		std::cout << "[VME] <Bridge::inputRead> ERROR: read input register failed" << std::endl;
		return -1;
	}
	// decoding with CVInputRegisterBits
	std::cout << "Input line 0 status: " << ((data&cvIn0Bit) >> 0) << std::endl;
	std::cout << "Input line 1 status: " << ((data&cvIn1Bit) >> 1) << std::endl;
	return 0;
}
