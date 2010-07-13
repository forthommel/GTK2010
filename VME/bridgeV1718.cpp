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

//
//
//

int bridgeV1718::setOutput(CVOutputSelect output,int state) {
  //state = 1 on, state = 0 off.  
  CVIOPolarity mode;  
  if(state == 1) {mode = cvInverted;} else {mode = cvDirect;}
	if(CAENVME_SetOutputConf(bhandle,cvOutput0,mode,cvActiveHigh,cvManualSW)) {
	  if(!CAENVME_SetOutputRegister(bhandle,cvOut0Bit)) {
		  std::cout << "Error: SetOutputRegister" << std::endl;
	  }
  }
	return 0;
} 

bridgeV1718::~bridgeV1718() {
  CAENVME_End(bhandle);
}
