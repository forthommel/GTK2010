#include "VME.h"

VME::VME() {
  //FIXME check getenv
  toVME = new Queue("../control/cosmic.pid",0,"[VME] ->VME");  
  toGUI = new Queue("../control/cosmic.pid",1,"[VME] ->GUI"); 
  bridge = new Bridge("/dev/usb/v1718_0");
  bhandle = bridge->getBHandle();
  scaler = new Scaler(bhandle,0x000b0000);
  cfd1 = new CFD(bhandle,0x00030000);
  cfd2 = new CFD(bhandle,0x00050000);
  cfd3 = new CFD(bhandle,0x00070000);
  cfd4 = new CFD(bhandle,0x00090000);
  //tdc = new TDC(bhandle,0x0001);  // MSW = Most Significant Word
  //tdc = new TDC(bhandle,0x000D);  // MSW = Most Significant Word
  data = new ShareMem("../control/cosmic.pid",0,2*16,"[VME] data"); // FIXME Dirty
  control = new ShareMem("../control/cosmic.pid",1,sizeof(struct control),"[VME] control");  
  //Clear the queue toVME 
  toVME->Clear(); //FIXME check return value !
  
  //Set status to running
  struct control d_control;
  d_control.vme = 1; //ON
  d_control.vme_quitting = 0;
  control->Lock();
  memcpy((void *)control->getShmAddr(),&d_control,sizeof(struct control));
  std::cout << "[VME] Status updated !" << std::endl;
  control->Unlock();
}

VME::~VME() {
  //Set status to quitting
  /*struct control d_control;
  d_control.vme = 0; //ON
  d_control.vme_quitting = 1;
  control->Lock();
  memcpy((void *)control->getShmAddr(),&d_control,sizeof(struct control));
  std::cout << "[VME] Status updated !" << std::endl;
  control->Unlock();*/

  delete bridge;
  delete scaler;
  delete cfd1;
  delete cfd2;
  delete cfd3;
  delete cfd4;
  //delete tdc;
  delete toVME;
  delete toGUI;
  delete data;
  delete control;
}

 
 int VME::Run() {
  //unsigned int buf;    
  //int channel;
  //int act = 0;
  //int running=1;
  int mopts;
  int mcode;
  //int ret;
  //int serialnum;
  //int modtype;
  int word;
  
  
  /*//FIXME DEBUG TDC
  std::cout << "----------DEBUG TDC----------" << std::endl;
  std::cout << "TDC SN: " << tdc->fetch_data(TDC_SERIAL_NUMBER) << std::endl;
  std::cout << "TDC Firmware rev.: " << tdc->fetch_data(TDC_FIRMWARE_REF) << std::endl;
  std::cout << "TDC Micro-firmware rev.: " << tdc->fetch_data(TDC_MICRO_FIRMWARE_REF) << std::endl;
  std::cout << "-----------------------------" << std::endl;
  //*/
  
  while (1) { // Forever
    if(toVME->Status() == -1) {
      std::cout << "[VME] ERROR: Message queue was removed" << std::endl;
      return -1;
    }  
    toVME->Receive(&mcode,&mopts,1);
    
    switch (mcode) { 
    /*case mQuitVME:
      running=0;
      break;*/
    case mReadChannel:
      ReadChannel(mopts);
      break;
    case mRead:
      ReadChannel(mopts);
      break;
    case mResetAll:
      ResetAll();
      break;
    case mReset:
      ResetAll();
      break;
    case mCFDInfos:
      word = cfd1->getInfos(mopts);
      data->Lock();
      memcpy((void *)data->getShmAddr(),&word,sizeof(int));
      data->Unlock();
      toGUI->Send(mDataReady,0,1);
      break;
    /*case mTDCReadSN: // TDC not yet implemented
      serialnum = tdc->getSN();
      data->Lock();
      memcpy((void *)data->getShmAddr(),&serialnum,sizeof(int));
      data->Unlock();
      toGUI->Send(mDataReady,0,1)*/
    case mCFDReadConfiguration:
      //#ifdef DEBUG
      std::cout << "[VME] <readCFDConfiguration> Debug: Value: " << mopts << std::endl;
      //#endif
      ReadAllCFDConfig();
      break;
    case mSetPoI_1: //FIXME
      panel = (mopts&983040)/65536; // four first bits are treated in order to determine CFD
      #ifdef DEBUG
        // Displays enabled and disabled channels on debug mode
        int mask[ChannelNumber];
        int masked[ChannelNumber];
        int maskedZero[ChannelNumber];
        int maskedOne[ChannelNumber];
        for(int i = 0; i < 16; ++i){
          mask[i] = pow(2,i);
        }
        for(int i = 0; i < 16; ++i){
          masked[i] = ((mopts&65535) & mask[i])/mask[i];
          if (masked[i] == 0) maskedZero[i] = 1;
          else maskedZero[i] = 0;
          if (masked[i] == 1) maskedOne[i] = 1;
          else maskedOne[i] = 0;
        }
        std::cout << "[VME] <setPoI> Debug: Enabled channels on panel " << panel+1 << ":  ";
        for(int i = 0; i < 16; ++i){
          if (maskedOne[i] == 1) std::cout << i+1 << " ";
        }
        std::cout << "\n[VME] <setPoI> Debug: Disabled channels on panel " << panel+1 << ": ";
        for(int i = 0; i < 16; ++i){
          if (maskedZero[i] == 1) std::cout << i+1 << " ";
        }
        std::cout << "\n";
      #endif
      if (panel==0) cfd1->setActiveChannels(mopts&65535);
      else if (panel==1) cfd2->setActiveChannels(mopts&65535);
      else if (panel==2) cfd3->setActiveChannels(mopts&65535);
      else if (panel==3) cfd4->setActiveChannels(mopts&65535);
      else std::cout << "[VME] <setPoI> ERROR: Please define a larger number of CFDs to work with in VME.cpp and VME.h" << std::endl;
      break;
    case mSetWidth_1: //FIXME
      panel = (mopts&7680)/512;
      #ifdef DEBUG
      std::cout << "[VME] <setWidth_1> Debug: value: " << mopts << ", panel: " << panel << ", channel range: " << (mopts&256)/256 << ", width: " << (mopts&255) << std::endl;
      #endif
      if (panel==0) cfd1->setOutWidth((channel_range)((mopts&256)/256),mopts&255);
      else if (panel==1) cfd2->setOutWidth((channel_range)((mopts&256)/256),mopts&255);
      else if (panel==2) cfd3->setOutWidth((channel_range)((mopts&256)/256),mopts&255);
      else if (panel==3) cfd4->setOutWidth((channel_range)((mopts&256)/256),mopts&255);
      break;
    case mSetDeadTime_1: //FIXME
      panel = (mopts&7680)/512;
      #ifdef DEBUG
      std::cout << "[VME] <setDeadTime1> Debug: Value: " << mopts << std::endl;
      #endif
      if (panel==0) cfd1->setDeadTime((channel_range)((mopts&256)/256),mopts&255);
      else if (panel==1) cfd2->setDeadTime((channel_range)((mopts&256)/256),mopts&255);
      else if (panel==2) cfd3->setDeadTime((channel_range)((mopts&256)/256),mopts&255);
      else if (panel==3) cfd4->setDeadTime((channel_range)((mopts&256)/256),mopts&255);
      break;
    case mSetThreshold_1:
      panel = (mopts&61440)/4096;
      #ifdef DEBUG
      std::cout << "[VME] <setThreshold> Debug: panel: " << panel << ", channel: " << (mopts&3840)/256 << ", value: " << (mopts&255) << std::endl;
      #endif
      if (panel==0) cfd1->setThreshold((mopts&3840)/256,mopts&255);
      else if (panel==1) cfd2->setThreshold((mopts&3840)/256,mopts&255);
      else if (panel==2) cfd3->setThreshold((mopts&3840)/256,mopts&255);
      else if (panel==3) cfd4->setThreshold((mopts&3840)/256,mopts&255);
      break;
    case mError:
      std::cout << "[VME] ERROR notified by Message Queue !" << std::endl;
      break;
    default: 
      std::cout << "[VME] ERROR: Received unknown message !" << std::endl;
    }
  }
  return 0;
}

void VME::ReadChannel(int opts) { // FIXME : put in Scaler.cpp script
  int count;
  const void *shmaddr = data->getShmAddr(); 
  if(opts == 0) { // Read all channels
    int i;
    int count[16];
    for(i=0; i<16; i++) {
      count[i] = scaler->readChannel(i+1);
    } 
    memcpy((void *)shmaddr,&count,16*sizeof(int)); // FIXME
    toGUI->Send(mDataReady,16*sizeof(int),1);
  }
  else { // Read channel i
    count = scaler->readChannel(opts);      
    // Copy data to shared memory
    memcpy((void *)shmaddr,&count,sizeof(int));
    // Send ACK to GUI
    toGUI->Send(mDataReady,sizeof(int),1);
  }
}

void VME::ResetAll() { // FIXME : put in Scaler.cpp script
  scaler->resetAll();
  toGUI->Send(mMemoryCleared,0,1);
}

void VME::ReadAllCFDConfig() {
  //int enabl[4][16]; // is the channel enabled
  int thres[4][16]; // threshold value for the channel
  int last[4][CFD_ADDED_LINES];   // output width, dead time, majority threshold
  int poi[4]={0, 0, 0, 0};
  std::string filename[4] = {"../commons/cfd_1.conf","../commons/cfd_2.conf","../commons/cfd_3.conf","../commons/cfd_4.conf"};
  for(int j=0; j<4; ++j){
    std::ifstream ifile; // read-only use of config file (fetch saved configuration for CFD module)
    ifile.open((const char*)filename[j].c_str());
    for(int i=0; i<16+CFD_ADDED_LINES; ++i){
      int buffer;
      ifile >> buffer;
      if (i<16) {
        thres[j][i]=buffer&255;
        poi[j]=poi[j]+((buffer&256)/256*std::pow(2,(double) i));
      }
      else last[j][i-16] = buffer; // last lines in config file (output width, dead time, etc)
    }
    //std::cout << "poi for channel " << j << ": " << poi[j] << std::endl;
    ifile.close();
    if (j==0) {
      cfd1->setActiveChannels(poi[0]);
      cfd1->setOutWidth(CH0TO7,last[j][0]&255);
      cfd1->setOutWidth(CH8TO15,(last[j][0]&65280)/256);
    }
    else if (j==1) {
      cfd2->setActiveChannels(poi[1]);
      cfd2->setOutWidth(CH0TO7,last[j][0]&255);
      cfd2->setOutWidth(CH8TO15,(last[j][0]&65280)/256);
    }
    else if (j==2) {
      cfd3->setActiveChannels(poi[2]);
      cfd3->setOutWidth(CH0TO7,last[j][0]&255);
      cfd3->setOutWidth(CH8TO15,(last[j][0]&65280)/256);
    }
    else if (j==3) {
      cfd4->setActiveChannels(poi[3]);
      cfd4->setOutWidth(CH0TO7,last[j][0]&255);
      cfd4->setOutWidth(CH8TO15,(last[j][0]&65280)/256);
    }
  }
  for(int i=0; i<16; ++i) {
    cfd1->setThreshold(i,thres[0][i]);
    cfd2->setThreshold(i,thres[1][i]);
    cfd3->setThreshold(i,thres[2][i]);
    cfd4->setThreshold(i,thres[3][i]);
  }
}
