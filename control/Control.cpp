#include "bridgeV1718.h"
#include "scaler1151N.h"
#include "tdcV1x90.h"

//#include "CAENV1x90.h"

#include <iostream>
#include <signal.h>
//#include <string>


int gEnd=0;
void CtrlC(int aSig) {
  //gEnd=1;
  if (gEnd>=5) {
    std::cout << "Ctrl-C detected five times... forcing exit!" << std::endl;
    exit(0);
  }
  else {
    std::cout << "Ctrl-C detected, setting end flag..." << std::endl;
    gEnd++;
  }
}

/*bool Continue(){
  return (gEnd==0);
}*/

int main() {
    int32_t bhandle;
    bridgeV1718 *bridge;
    bridge = new bridgeV1718("/dev/usb/v1718_0");
    bhandle = bridge->getBHandle();
   
    signal(SIGINT, CtrlC);

    scaler1151N *scaler;
    scaler = new scaler1151N(bhandle,0x000b0000);    

    //scaler->resetAll();
    //for (int i=0;i<10;i++){
    //  printf("1151N: ch[1]=%d\n",scaler->readChannel(1));
    //}
   

    tdcV1x90 *tdc;
    //tdc = new tdcV1x90(bhandle,0x00010000,CONT_STORAGE); //v1190
    //tdc = new tdcV1x90(bhandle,0x000d0000,CONT_STORAGE);   //v1290
    //tdc->checkConfiguration();


    //tdc = new CAENV1x90(bhandle,0x00010000,TRIG_MATCH);
    //tdc = new CAENV1x90(bhandle,0x000d0000,CONT_STORAGE);
    tdc = new tdcV1x90(bhandle,0x000d0000,TRIG_MATCH);
    tdc->getFirmwareRev();

    //TDC Config
    /*tdc->setWindowWidth(4095);
    tdc->setWindowOffset(-2048);*/
    /*std::cout << "window width: " << (tdc->readTrigConf(MATCH_WIN_WIDTH)) << std::endl;
    std::cout << "window offset: " << (tdc->readTrigConf(WIN_OFFSET)) << std::endl;*/

    //while(true) tdc->getEvents(TRIG_MATCH,tdc->readDetection());
    //
    //FIXME: TEST TEST TEST
    //OFF  
    int i = 0;
    while(true) {
      bridge->setOutput((CVOutputSelect)0,i%2);
      
      i++;
    //std::cout << "Output 0: off" << std::endl;
    usleep(1);
    bridge->setOutput((CVOutputSelect)0,1);   
    //std::cout << "Output 0: on" << std::endl;
    }    
//
    delete bridge;
    //delete scaler;
    delete tdc;
    return 0;
}
