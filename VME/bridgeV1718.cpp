#include "VME/bridgeV1718.h"

bridgeV1718::bridgeV1718(const char *device) {
  int dev;
	dev = atoi(device); 
  if(CAENVME_Init(cvV1718,0,dev, &bhandle) != cvSuccess) {
    std::cout << "[VME] <Bridge::Bridge> ERROR: Opening the device" << std::endl;
    exit(1); 
  }
	#ifdef DEBUG
	std::cout << "[VME] <Bridge::Bridge> Debug: BHandle " << (int)bhandle << std::endl;
	#endif
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
    std::cout << "[VME] <Bridge::outputConf> ERROR: Config of output " << (int)output << " failed" << std::endl; 	
    return -1;
  }
  return 0; 
}

// output := cvOutput[0,4]
int bridgeV1718::outputOn(CVOutputSelect output) {
	//FIXME: Silly and Ugly ! Use a map ?
	int t;
	switch (output) {
		case cvOutput0:
			t = cvOut0Bit;
			break;
		case cvOutput1:
			t = cvOut1Bit;
			break;
		case cvOutput2:
			t = cvOut2Bit;
			break;
		case cvOutput3:
			t = cvOut3Bit;
			break;
		case cvOutput4:
			t = cvOut4Bit;
			break;
		default:
			std::cout << "[VME] <Bridge::outputOn> ERROR: Unknown output" << std::endl;
			return -1;			
	}
	if(CAENVME_SetOutputRegister(bhandle,t) != cvSuccess) {
		std::cout << "[VME] <Bridge::outputOn> ERROR: set register failed" << std::endl;
		return -1;	
	}
	return 0;
}
int bridgeV1718::outputOff(CVOutputSelect output) {
	//FIXME: Silly and Ugly ! Use a map ?
	int t;
	switch (output) {
		case cvOutput0:
			t = cvOut0Bit;
			break;
		case cvOutput1:
			t = cvOut1Bit;
			break;
		case cvOutput2:
			t = cvOut2Bit;
			break;
		case cvOutput3:
			t = cvOut3Bit;
			break;
		case cvOutput4:
			t = cvOut4Bit;
			break;
		default:
			std::cout << "[VME] <Bridge::outputOn> ERROR: Unknown output" << std::endl;
			return -1;			
	}
	if(CAENVME_ClearOutputRegister(bhandle,t) != cvSuccess) {
		std::cout << "[VME] <Bridge::outputOn> ERROR: set register failed" << std::endl;
		return -1;	
	}
	return 0;
}


