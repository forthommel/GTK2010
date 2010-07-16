#include "bridgeV1718.h"
#include "scaler1151N.h"
#include "tdcV1x90.h"

#include <iostream>
#include <signal.h>

bridgeV1718 *bridge;
scaler1151N *scaler;
tdcV1x90* tdc;

int gEnd=0;
void CtrlC(int aSig) {
  gEnd=1;
  if (gEnd==5) {
    std::cout << "Ctrl-C detected five times... trying clean exit!" << std::endl;
    tdc->abort();
  }
	else if (gEnd > 5) {
    std::cout << "Ctrl-C detected > five times... forcing exit!" << std::endl;
		exit(0);
	}
  else {
    std::cout << "Ctrl-C detected, setting end flag..." << std::endl;
    gEnd++;
  }
}

int main() {
    int32_t bhandle;
    bridge = new bridgeV1718("/dev/usb/v1718_0");
    bhandle = bridge->getBHandle();
   
    signal(SIGINT, CtrlC);
    scaler = new scaler1151N(bhandle,0x000b0000);    
    //scaler->resetAll();
    //for (int i=0;i<10;i++){
    //  printf("1151N: ch[1]=%d\n",scaler->readChannel(1));
    //}
   
    //tdc = new tdcV1x90(bhandle,0x00010000,CONT_STORAGE); //v1190
    //tdc = new tdcV1x90(bhandle,0x000d0000,CONT_STORAGE);   //v1290
    //tdc->checkConfiguration();

    tdc = new tdcV1x90(bhandle,0x000d0000,TRIG_MATCH,TRAILEAD);
    tdc->getFirmwareRev();

    //TDC Config
    tdc->setWindowWidth(1000);
    tdc->setWindowOffset(-1024);
    /*std::cout << "window width: " << (tdc->readTrigConf(MATCH_WIN_WIDTH)) << std::endl;
      std::cout << "window offset: " << (tdc->readTrigConf(WIN_OFFSET)) << std::endl;*/
    
    //tdc->softwareClear(); //FIXME don't forget to erase
    
    //tdc->setTDCEncapsulation(false);
    std::cout << "Are header and trailer bytes sent in BLT? " << tdc->getTDCEncapsulation() << std::endl;
    
    //usleep(300000); //FIXME !!!!!! need to wait before data acquisition !!!!!!
    tdc->waitMicro(WRITE_OK);
    
    int i;
    for(i = 0; i < 10; i++) {
    //while(true) {
      tdc->getEvents();
      //std::cout << "IS FULL? " << tdc->getStatusRegister(FULL) << std::endl;
      //sleep(1);
    }

  //Input line test 
  //bridge->inputConf(cvInput0);
  //bridge->inputConf(cvInput1);
  //int i;
  //for(i = 0; i < 10000; i++) {
  //  bridge->inputRead(cvInput0);
  //  usleep(10);
  //}
    delete bridge;
    //delete scaler;
    delete tdc;
    return 0;
}
