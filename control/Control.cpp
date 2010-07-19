#include "bridgeV1718.h"
#include "scaler1151N.h"
#include "tdcV1x90.h"

#include <iostream>
#include <fstream>
#include <signal.h>

bridgeV1718 *bridge;
scaler1151N *scaler;
tdcV1x90* tdc;

int gEnd=0;
void CtrlC(int aSig) {
  gEnd++;
  if (gEnd==5) {
    std::cout << "\nCtrl-C detected five times... trying clean exit!" << std::endl;
    tdc->abort();
  }
  else if (gEnd > 5) {
    std::cout << "\nCtrl-C detected more than five times... forcing exit!" << std::endl;
    exit(0);
  }
  else std::cout << "\nCtrl-C detected, setting end flag..." << std::endl;
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
    tdc->setWindowWidth(2040);
    tdc->setWindowOffset(-2045);
    /*std::cout << "window width: " << (tdc->readTrigConf(MATCH_WIN_WIDTH)) << std::endl;
      std::cout << "window offset: " << (tdc->readTrigConf(WIN_OFFSET)) << std::endl;*/
    
    //tdc->softwareClear(); //FIXME don't forget to erase
    
    //tdc->setTDCEncapsulation(false);
    std::cout << "Are header and trailer bytes sent in BLT? " << tdc->getTDCEncapsulation() << std::endl;
   
    tdc->waitMicro(WRITE_OK);
    
    //Output to file;
    std::fstream out_file;
    out_file.open(argv[1],std::fstream::out | std::fstream::app);
    int i;
    //for(i = 0; i < 20000; i++) {
    while(true) {
      tdc->getEvents(&out_file);
    }
    out_file.close();

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
