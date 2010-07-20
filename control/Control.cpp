#include "bridgeV1718.h"
#include "scaler1151N.h"
#include "tdcV1x90.h"

#include <iostream>
#include <fstream>
#include <string>
#include <signal.h>

struct file_header_t {
  uint32_t magic;
  uint32_t run_id;
  uint32_t spill_id;
  //uint32_t data_size; //DOUBLE CHECK ON FREAD
};

bridgeV1718 *bridge;
scaler1151N *scaler;
tdcV1x90* tdc;
std::fstream out_file;

int gEnd=0;
void CtrlC(int aSig) {
  gEnd++;
  if (gEnd==1) {
    std::cout << "\nCtrl-C detected... trying clean exit!" << std::endl;
    out_file.close();
    tdc->abort();
  }
  else if (gEnd > 5) {
    std::cout << "\nCtrl-C detected more than five times... forcing exit!" << std::endl;
    exit(0);
  }
}

int main(int argc, char *argv[]) {
  
    if(argc != 2) {
      std::cout << "Usage: " << argv[0] << " FILENAME" << std::endl;
      exit(-1);
    }
    //FIXME: Checks on the filename !!!
    
    int32_t bhandle;
    bridge = new bridgeV1718("/dev/usb/v1718_0");
    bhandle = bridge->getBHandle();
   
    signal(SIGINT, CtrlC);

    tdc = new tdcV1x90(bhandle,0x000d0000,TRIG_MATCH,TRAILEAD);
    tdc->getFirmwareRev();

    //TDC Config
    tdc->setWindowWidth(2040);
    tdc->setWindowOffset(-2045);
   
    tdc->waitMicro(WRITE_OK);
    //tdc->softwareClear(); //FIXME don't forget to erase
    //std::cout << "Are header and trailer bytes sent in BLT? " << tdc->getTDCEncapsulation() << std::endl;
    //FIXME: Need to check the user input
   	out_file.open(argv[1],std::fstream::out | std::ios::binary );	
    if(!out_file.is_open()) {
      std::cerr << argv[0] << ": error opening file " << argv[1] << std::endl;
      return -1;
	  }
	  file_header_t fh;
	  fh.magic = 0x47544B30; //ASCII: GTK0 
		fh.run_id = 0;
		fh.spill_id = 0;
		
		std::cout << "*** Ready for acquisition! ***" << std::endl;
		
		out_file.write((char*)&fh,sizeof(file_header_t));
    int i;
    while(true) {
      tdc->getEvents(&out_file);
    }
    out_file.close();
    delete bridge;
    delete tdc;
    return 0;
}
