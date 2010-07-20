#include "tdcV1x90.h"

//#include <stdio.h>
//#include <vector>

tdcV1x90::tdcV1x90(int32_t abhandle,uint32_t abaseaddr,acq_mode acqmode=TRIG_MATCH,det_mode detmode=TRAILEAD) {

  this->bhandle = abhandle;
  this->baseaddr = abaseaddr;
  this->am = cvA32_U_DATA;
  this->am_blt = cvA32_U_BLT;

  //event_nb = 0;
  //event_max = 1024;

  buffer=(uint32_t *)malloc(16*1024*1024); // 16Mb of buffer!
  if (buffer == NULL) {
    std::cout << "[VME] <TDC::constructor> ERROR: buffer has not been allocated" << std::endl;
    exit(0);
  }
  
  if (checkConfiguration()) {
  
    softwareReset();
    setAcquisitionMode(acqmode);
    
    detm = detmode;
    setDetection(TRAILEAD);
    setLSBTraileadEdge(r25ps);
    setRCAdjust(0,0);
    setRCAdjust(1,0);
    setGlobalOffset(0x0,0x0); // coarse and fine set
    readGlobalOffset();
    setBLTEventNumberRegister(1); // FIXME find good value!
    setTDCEncapsulation(true);
    setTDCErrorMarks(true);
    setETTT(true);
    setWindowWidth(2045);
    setWindowOffset(-2050);
    //setPairModeResolution(0,0x4);
    //readResolution(detect);
    
    gEnd = false;
 }
     


  //FIXME horribly crappy!
  /*std::vector<std::string> pair_lead_res(8);
  /*vector<string> pair_width_res(16);
  vector<string> trailead_edge_res(4);
  
  pair_lead_res[] = {"100ps","200ps","400ps","800ps","1.6ns","3.12ns","6.25ns","12.5ns"};
  for (int i=0;i<8;i++) std::cout << "pair_lead_res[" << i << "]: " << pair_lead_res[i] << std::endl;*/
    
  char* c_pair_lead_res[] = {
    "100ps",
    "200ps",
    "400ps",
    "800ps",
    "1.6ns",
    "3.12ns",
    "6.25ns",
    "12.5ns"};
  char* c_pair_width_res[] = {
    "100ps",
    "200ps",
    "400ps",
    "800ps",
    "1.6ns",
    "3.2ns",
    "6.25ns",
    "12.5ns",
    "25ns",
    "50ns",
    "100ns",
    "200ns",
    "400ns",
    "800ns",
    "invalid","invalid"};
  char* c_trailead_edge_res[] = {
    "800ps",
    "200ps",
    "100ps",
    "25ps"};
  for(int i=0;i<8;i++) pair_lead_res[i]=c_pair_lead_res[i];
  for(int i=0;i<16;i++) pair_width_res[i]=c_pair_width_res[i];
  for(int i=0;i<4;i++) trailead_edge_res[i]=c_trailead_edge_res[i];

}

tdcV1x90::~tdcV1x90() { //FIXME implement destructor...
   free(buffer);
   buffer = NULL;
   /*free(final);
   final = NULL;*/
}

uint32_t tdcV1x90::getModel() {
  uint32_t model;
  uint16_t data[3];
  mod_reg addr[3] = {ROMBoard0, ROMBoard1, ROMBoard2};
  for(int i=0; i<3; i++) readRegister(addr[i],&(data[i]));
  model = (((data[2]&0xff) << 16)+((data[1]&0xff) << 8)+(data[0]&0xff));
  #ifdef DEBUG
  std::cout << "[VME] <TDC::getModel> Debug: Model is " 
      << std::dec << model << std::endl;
  #endif
  return model;
}

uint32_t tdcV1x90::getOUI() {
  uint32_t oui;
  uint16_t data[3];
  mod_reg addr[3] = {ROMOui0, ROMOui1, ROMOui2};
  for(int i=0; i<3; i++) readRegister(addr[i],&(data[i]));
  oui = (((data[2]&0xff) << 16)+((data[1]&0xff) << 8)+(data[0]&0xff));
  #ifdef DEBUG
  std::cout << "[VME] <TDC::getOUI> Debug: OUI manufacturer number is " 
      << std::dec << oui << std::endl;
  #endif
  return oui;
}

uint32_t tdcV1x90::getSerNum() {
  uint32_t sn;
  uint16_t data[2];
  mod_reg addr[2] = {ROMSerNum0, ROMSerNum1};
  for(int i=0; i<2; i++) readRegister(addr[i],&(data[i]));
  sn = (((data[1]&0xff) << 8)+(data[0]&0xff));
  #ifdef DEBUG
  std::cout << "[VME] <TDC::getSerNum> Debug: Serial number is " 
      << std::dec << sn << std::endl;
  #endif
  return sn;
}

void tdcV1x90::getFirmwareRev() { //FIXME need to clean up
  uint32_t fr[2];
  uint16_t data;
  readRegister(FirmwareRev,&data);
  fr[0] = data&0xF;
  fr[1] = (data&0xF0)>>4;
  #ifdef DEBUG
  std::cout << "[VME] <TDC::getFirmwareRev> Debug: Firmware revision is " 
      << std::dec << fr[1] << "." << fr[0] << std::endl;
  #endif
  //return sn;
}

bool tdcV1x90::checkConfiguration() {
  uint32_t oui;
  uint32_t model;

  oui = getOUI();
  model = getModel();

  if (oui != 0x0040e6) { // C.A.E.N.
    std::cerr << "[VME] <TDC::getOUI> ERROR: Wrong manufacturer identifier: " 
              << std::hex << oui << std::endl;
    return false;
  }
  //if (model != 1190) {
  if ((model != 1190) && (model != 1290)) {
    std::cerr << "[VME] <TDC::getModel> ERROR: Wrong model number: model is " 
              << std::dec << model << std::endl;
    return false;
  }
  
  /*#ifdef DEBUG
  std::cout << "[VME] <TDC::checkConfiguration> Debug:" << std::endl;
  std::cout << "       OUI manufacturer number is 0x"
            << std::hex << std::setfill('0') << std::setw(6) << oui << std::endl;
  std::cout << "                  Model number is "
            << std::dec << model << std::endl;
  std::cout << "                 Serial number is "
            << std::dec << getSerNum() << std::endl;
  #endif*/
  return true;
}

void tdcV1x90::setPoI(uint16_t word) {
  
}

void tdcV1x90::setLSBTraileadEdge(trailead_edge_lsb conf) {
  uint16_t word = conf;
  uint16_t value = tdcV1x90Opcodes::SET_TR_LEAD_LSB;
  waitMicro(WRITE_OK);
  writeRegister(Micro,&value);
  waitMicro(WRITE_OK);
  writeRegister(Micro,&word);
  #ifdef DEBUG
  std::cout << "[VME] <TDC::setLSBTraileadEdge> Debug: ";
  switch(conf){
    case r800ps: std::cout << "800ps" << std::endl; break;
    case r200ps: std::cout << "200ps" << std::endl; break;
    case r100ps: std::cout << "100ps" << std::endl; break;
    case r25ps: std::cout << "25ps" << std::endl; break;
  }
  #endif
}

void tdcV1x90::setGlobalOffset(uint16_t word1,uint16_t word2) {
  uint16_t opcode = tdcV1x90Opcodes::SET_GLOB_OFFS;
  waitMicro(WRITE_OK);
  writeRegister(Micro,&opcode);
  waitMicro(WRITE_OK);
  writeRegister(Micro,&word1);
  waitMicro(WRITE_OK);
  writeRegister(Micro,&word2);
  #ifdef DEBUG
  std::cout << "[VME] <TDC::setGlobalOffset> Debug: " << std::endl;
  std::cout << "             coarse counter offset: " << word1 << std::endl;
  std::cout << "               fine counter offset: " << word2 << std::endl;
  #endif
}

glob_offs tdcV1x90::readGlobalOffset() {
  uint16_t opcode = tdcV1x90Opcodes::READ_GLOB_OFFS;
  uint16_t data[2];
  waitMicro(WRITE_OK);
  writeRegister(Micro,&opcode);
  int i;
  for(i=0;i<2;i++){
    waitMicro(READ_OK);
    readRegister(Micro,&(data[i]));
  }
  #ifdef DEBUG
  std::cout << "[VME] <TDC::readGlobalOffset> Debug: " << std::endl;
  std::cout << "              coarse counter offset: " << data[0] << std::endl;
  std::cout << "                fine counter offset: " << data[1] << std::endl;
  #endif
  glob_offs ret;
  ret.fine = data[1];
  ret.coarse = data[0];
  return ret;
}

void tdcV1x90::setRCAdjust(int tdc, uint16_t value) { //FIXME find a better way to insert value for 12 RCs
  uint16_t word = value;
  uint16_t opcode = tdcV1x90Opcodes::SET_RC_ADJ+(tdc&0x3);
  waitMicro(WRITE_OK);
  writeRegister(Micro,&opcode);
  waitMicro(WRITE_OK);
  writeRegister(Micro,&word);
  
  /*opcode = tdcV1x90Opcodes::SAVE_RC_ADJ;
  waitMicro(WRITE_OK);
  writeRegister(Micro,&opcode);
  waitMicro(WRITE_OK);
  writeRegister(Micro,&word); */
  #ifdef DEBUG
  std::cout << "[VME] <TDC::setRCAdjust> Debug: TDC " << tdc 
            << ", value " << value << std::endl;  
  #endif
}

uint16_t tdcV1x90::readRCAdjust(int tdc) {
  uint16_t opcode = tdcV1x90Opcodes::READ_RC_ADJ+(tdc&0x3);
  uint16_t data;
  waitMicro(WRITE_OK);
  writeRegister(Micro,&opcode);
  waitMicro(READ_OK);
  readRegister(Micro,&data);
  
#ifdef DEBUG
  std::cout << "[VME] <TDC:readRCAdjust> Debug: value for TDC " << tdc << std::endl;
  double i;
  for(i=0;i<12;i++) {
    std::cout << "   bit " << std::setw(2) << i << ": ";
    char bit = (data&(uint16_t)(std::pow(2,i)));
    switch(bit) {
      case 0: std::cout << "contact open"; break;
      case 1: std::cout << "contact closed"; break;
    }
    std::cout << std::endl;
  }
#endif
  return data;
}

void tdcV1x90::setDetection(det_mode mode) {
  uint16_t word = mode;
  uint16_t value = tdcV1x90Opcodes::SET_DETECTION;
  waitMicro(WRITE_OK);
  writeRegister(Micro,&value);
  waitMicro(WRITE_OK);
  writeRegister(Micro,&word);
  #ifdef DEBUG
  std::cout << "[VME] <TDC::setDetection> Debug: ";
  switch(mode){
    case PAIR: std::cout << "pair mode" << std::endl; break;
    case OTRAILING: std::cout << "only trailing" << std::endl; break;
    case OLEADING: std::cout << "only leading" << std::endl; break;
    case TRAILEAD: std::cout << "trailing and leading" << std::endl; break;
  }
  #endif
}

det_mode tdcV1x90::readDetection() {
  uint16_t value = tdcV1x90Opcodes::READ_DETECTION;
  uint16_t data;
  waitMicro(WRITE_OK);
  writeRegister(Micro,&value);
  waitMicro(READ_OK);
  readRegister(Micro,&data);
  
#ifdef DEBUG
  std::cout << "[VME] <TDC:readDetection> Debug: ";
  switch(data){
    case PAIR: std::cout << "pair mode" << std::endl; break;
    case OTRAILING: std::cout << "only trailing" << std::endl; break;
    case OLEADING: std::cout << "only leading" << std::endl; break;
    case TRAILEAD: std::cout << "trailing and leading" << std::endl; break;
  }
#endif
  return (det_mode)data;
}

void tdcV1x90::setWindowWidth(uint16_t width) {
  uint16_t word = width;
  uint16_t value = tdcV1x90Opcodes::SET_WIN_WIDTH;
  waitMicro(WRITE_OK);
  writeRegister(Micro,&value);
  waitMicro(WRITE_OK);
  writeRegister(Micro,&word);
}

void tdcV1x90::setWindowOffset(int16_t offs) {
  //FIXME warning at sign bit
  uint16_t word = offs;
  uint16_t value = tdcV1x90Opcodes::SET_WIN_OFFS;
  waitMicro(WRITE_OK);
  writeRegister(Micro,&value);
  waitMicro(WRITE_OK);
  writeRegister(Micro,&word);
}
  
uint16_t tdcV1x90::readTrigConf(trig_conf type) {
  uint16_t value = tdcV1x90Opcodes::READ_TRG_CONF;
  uint16_t buff[5];
  waitMicro(WRITE_OK);
  writeRegister(Micro,&value);
  int i;
  for (i=0;i<5;i++) {
    waitMicro(READ_OK);
    readRegister(Micro,&(buff[i]));
  }
  return buff[type];
}

void tdcV1x90::readResolution(det_mode det) {
  uint16_t value = tdcV1x90Opcodes::READ_RES;
  uint16_t data;
  waitMicro(WRITE_OK);
  writeRegister(Micro,&value);
  waitMicro(READ_OK);
  readRegister(Micro,&data);
  
  std::cout << "[VME] <TDC:readResolution> Debug: ";
  switch(det) {
    case PAIR: 
      std::cout << "(pair mode) leading edge res.: " << pair_lead_res[data&0x7]
                << ", pulse width res.: " << pair_width_res[(data&0xF00)>>8]
                << std::endl;
      break;
    case OLEADING:
    case OTRAILING:
    case TRAILEAD:
      std::cout << "(l/t mode) leading/trailing edge res.: "
                << trailead_edge_res[data&0x3] << std::endl;
      break;
  }
}

void tdcV1x90::setPairModeResolution(int lead_time_res, int pulse_width_res) {
  uint16_t value = tdcV1x90Opcodes::SET_PAIR_RES;
  uint16_t data = 0;
  data = lead_time_res+0x100*pulse_width_res;
  /*(data&0x7)=lead_time_res;
  (data&0xf00)=pulse_width_res;*/
  waitMicro(WRITE_OK);
  writeRegister(Micro,&value);
  waitMicro(WRITE_OK);
  writeRegister(Micro,&data);
}


void tdcV1x90::setAcquisitionMode(acq_mode mode) {
  acqm = mode;
  switch(mode){
    case CONT_STORAGE:
      if (!(setContinuousStorage()))
        std::cerr << "[VME] <TDC::setContinuousStorage> ERROR: while entering the continuous storage mode" << std::endl;
      break;
    case TRIG_MATCH:
      if (!(setTriggerMatching()))
        std::cerr << "[VME] <TDC::setTriggerMatching> ERROR: while entering the trigger matching mode" << std::endl;
      break;
    default:
      std::cerr << "[VME] <TDC> ERROR: Wrong acquisition mode" << std::endl;
      break;
  }
}

bool tdcV1x90::setTriggerMatching() {
  uint16_t value = tdcV1x90Opcodes::TRG_MATCH;
  waitMicro(WRITE_OK);
  writeRegister(Micro,&value);
  waitMicro(WRITE_OK);
  #ifdef DEBUG
  std::cout << "[VME] <TDC::setTriggerMatching> Debug: trigger matching mode"
            << std::endl;
  #endif
  return true;
}

bool tdcV1x90::isTriggerMatching() {
  uint16_t data;

  /*uint32_t addr = baseaddr+0x1002; // Status register
  std::cout << "ReadCycle response: " << std::dec << CAENVME_ReadCycle(bhandle,addr,&data,am,cvD16) << std::endl;
  std::cout << "isTriggerMatching: value: " << ((data>>3)&0x1) << std::endl;*/

  uint16_t value = tdcV1x90Opcodes::READ_ACQ_MOD;
  waitMicro(WRITE_OK);
  writeRegister(Micro,&value);
  waitMicro(READ_OK);
  readRegister(Micro,&data);
  std::cout << "[VME] <TDC::isTriggerMatching> Debug: value: "
      << data << " (";
  switch(data) {
    case 0: std::cout << "continuous storage"; break;
    case 1: std::cout << "trigger matching"; break;
    default: std::cout << "wrong answer!"; break;
  }
  std::cout << ")" << std::endl;
  return (bool)data;
}

bool tdcV1x90::setContinuousStorage() {
  uint16_t value = tdcV1x90Opcodes::CONT_STOR;
  waitMicro(WRITE_OK);
  writeRegister(Micro,&value);
  waitMicro(WRITE_OK);
  #ifdef DEBUG
  std::cout << "[VME] <TDC::setContinuousStorage> Debug: continuous storage mode" << std::endl;
  #endif
  return true;
}



bool tdcV1x90::softwareReset() {
  uint16_t value = 0x0;
  writeRegister(ModuleReset,&value);
  return true;
}

bool tdcV1x90::softwareClear() {
  uint16_t value = 0x0;
  writeRegister(SoftwareClear,&value);
  return true;
}

bool tdcV1x90::getStatusRegister(stat_reg bit) {
  uint16_t data;
  readRegister(Status,&data);
  return ((data&(1<<bit))>>bit);
}

void tdcV1x90::setStatusRegister(stat_reg reg, bool value) {
  bool act = getStatusRegister(reg);
  if (act != value) {
    uint16_t buff;
    readRegister(Status,&buff);
    switch(value) {
      case true: buff+=std::pow(2,(double)reg); break;
      case false: buff-=std::pow(2,(double)reg); break;
    }
    writeRegister(Status,&buff);
  }
}

bool tdcV1x90::getCtlRegister(ctl_reg bit) {
  uint16_t data;
  readRegister(Control,&data);
  return ((data&(1<<bit))>>bit);
}

void tdcV1x90::setCtlRegister(ctl_reg reg, bool value) {
  bool act = getCtlRegister(reg);
  if (act != value) {
    uint16_t buff;
    readRegister(Control,&buff);
    switch(value) {
      case true: buff+=std::pow(2,(double)reg); break;
      case false: buff-=std::pow(2,(double)reg); break;
    }
    writeRegister(Control,&buff);
  }
}

bool tdcV1x90::isEventFIFOReady() {
  uint16_t data;
  uint16_t data2;
  std::cout << "[VME] <TDC::ifEventFIFOReady> Debug: is FIFO enabled: "
            << getCtlRegister(EVENT_FIFO_ENABLE) << std::endl;
  setFIFOSize(7); //FIXME
  readFIFOSize();
  /*readRegister(EventFIFOStatusRegister,&data);
  std::cout << "[VME] <TDC::ifEventFIFOReady> Debug: data: " << data << std::endl;
  std::cout << "                DATA_READY: " << (data&1) << std::endl;
  std::cout << "                      FULL: " << ((data&2)>>1) << std::endl;
  readRegister(EventFIFOStoredRegister,&data2);
  std::cout << "[VME] <TDC::ifEventFIFOReady> Debug: data2: " << ((data2&0x7ff)>>11) << std::endl;*/
  
}
void tdcV1x90::setFIFOSize(uint16_t size) {
  //std::cout << "size: " << (int)(std::log(size/2)/std::log(2)) << std::endl;
  //FIXME! do some crappy math
  uint16_t word;
  switch(size) {
    case 2: word=0;break;
    case 4: word=1;break;
    case 8: word=2;break;
    case 16: word=3;break;
    case 32: word=4;break;
    case 64: word=5;break;
    case 128: word=6;break;
    case 256: word=7;break;
    default: exit(0);
  }
  std::cout << "[VME] <TDC::writeFIFOSize> Debug: WRITE_FIFO_SIZE: "
            << word << std::endl;
  uint16_t opcode = tdcV1x90Opcodes::SET_FIFO_SIZE;
  waitMicro(WRITE_OK);
  writeRegister(Micro,&opcode);
  waitMicro(WRITE_OK);
  writeRegister(Micro,&word);
  #ifdef DEBUG
  std::cout << "[VME] <TDC::writeFIFOSize> Debug: WRITE_FIFO_SIZE: "
            << word << std::endl;
  #endif
}

void tdcV1x90::readFIFOSize() {
  uint16_t word = tdcV1x90Opcodes::READ_FIFO_SIZE;
  uint16_t data;
  waitMicro(WRITE_OK);
  writeRegister(Micro,&word);
  waitMicro(READ_OK);
  readRegister(Micro,&data);
  std::cout << "[VME] <TDC::readFIFOSize> Debug: READ_FIFO_SIZE: "
            << std::dec << std::pow(2,data+1) << std::endl;
}

void tdcV1x90::setTDCEncapsulation(bool mode) {
  uint16_t opcode;
  switch(mode){
    case false:
      opcode = tdcV1x90Opcodes::DIS_HEAD_TRAILER;
      outBufTDCHeadTrail=false;
      break;
    case true:
      opcode = tdcV1x90Opcodes::EN_HEAD_TRAILER;
      outBufTDCHeadTrail=true;
      break;
  }
  waitMicro(WRITE_OK);
  writeRegister(Micro,&opcode);
  #ifdef DEBUG
  std::cout << "[VME] <TDC::setTDCEncapsulation> Debug: Enabled? "
            << mode << std::endl;
  #endif
  
}

bool tdcV1x90::getTDCEncapsulation() {
  uint16_t opcode = tdcV1x90Opcodes::READ_HEAD_TRAILER;
  uint16_t enc;
  waitMicro(WRITE_OK);
  writeRegister(Micro,&opcode);
  waitMicro(READ_OK);
  readRegister(Micro,&enc);
  #ifdef DEBUG
  std::cout << "[VME] <TDC::getTDCEncapsulation> Debug: READ_HEAD_TRAILER: "
            << enc << std::endl;
  #endif
  return enc;
}

uint32_t tdcV1x90::getEventCounter() {
  uint32_t value;
  readRegister(EventCounter,&value);
  return value;
}

uint16_t tdcV1x90::getEventStored() {
  uint16_t value;
  readRegister(EventStored,&value);
  return value;
}

void tdcV1x90::setTDCErrorMarks(bool mode){
  uint16_t opcode;
  switch(mode){
    case false:
      opcode = tdcV1x90Opcodes::DIS_ERROR_MARK;
      outBufTDCErr=false;
      break;
    case true:
      opcode = tdcV1x90Opcodes::EN_ERROR_MARK;
      outBufTDCErr=true;
      break;
  }
  waitMicro(WRITE_OK);
  writeRegister(Micro,&opcode);
  #ifdef DEBUG
  std::cout << "[VME] <TDC::setTDCErrorMarks> Debug: Enabled? "
            << mode << std::endl;
  #endif
}

/*bool tdcV1x90::getTDCErrorMarks() {
  uint16_t opcode = tdcV1x90Opcodes::READ_HEAD_TRAILER;
}*/

void tdcV1x90::setBLTEventNumberRegister(uint16_t value) {
  writeRegister(BLTEventNumber,&value);
  #ifdef DEBUG
  std::cout << "[VME] <TDC::setBLTEventNumberRegister> Debug: value: "
            << value << std::endl;
  #endif
}

uint16_t tdcV1x90::getBLTEventNumberRegister() {
  uint16_t value;
  readRegister(BLTEventNumber,&value);
  #ifdef DEBUG
  std::cout << "[VME] <TDC::getBLTEventNumberRegister> Debug: value: "
            << value << std::endl;
  #endif
  return value;
}
  
  
void tdcV1x90::setETTT(bool mode) {
  setCtlRegister(EXTENDED_TRIGGER_TIME_TAG_ENABLE,mode);
  outBufTDCTTT = mode;
  #ifdef DEBUG
  std::cout << "[VME] <TDC::setETTT> Debug: Enabled? "
            << mode << std::endl;
  #endif
}

bool tdcV1x90::getETTT() {
  return getCtlRegister(EXTENDED_TRIGGER_TIME_TAG_ENABLE);
}

bool tdcV1x90::getEvents(std::fstream * out_file) {
  // Start readout (check if BERR is set to 0)
  // Nw words are transmitted until the global TRAILER
  // Move file!
  
  memset(buffer,0,sizeof(buffer));
  
  int count=0;
  int blts = 1024;
  bool finished;
  int i;

  CVErrorCodes ret;
  ret = CAENVME_BLTReadCycle(bhandle,baseaddr+0x0000,(char *)buffer,blts,am_blt,cvD32,&count);
  finished = ((ret==cvSuccess)||(ret==cvBusError)||(ret==cvCommError)); //FIXME investigate...
  if (finished && gEnd) {
    #ifdef DEBUG
    std::cout << "[VME] <TDC::getEvents> Debug: Exit requested!" << std::endl;
    #endif
    exit(0);
  }
  if (acqm == TRIG_MATCH) {
    for(i=0;i < count/4;i++) {
      if ((buffer[i]>>27)!=0x18) out_file->write((char*)&(buffer[i]),sizeof(uint32_t));
      //(*out_file) << (buffer[i]);
      /*std::cout << "buffer while writing: " << (buffer[i]>>27) << std::endl;*/
    }
  }
  else std::cerr << "[VME] <TDC::getEvents> ERROR: Wrong acquisition mode: "
                 << acqm << std::endl;
  return true;
}

void tdcV1x90::abort() {
  #ifdef DEBUG
  std::cout << "[VME] <TDC::abort> Debug: received abort signal" << std::endl;
  #endif
  // Raise flag
  gEnd = true;
}

int tdcV1x90::writeRegister(mod_reg addr, uint16_t* data) {
  uint32_t address = baseaddr+addr;
  if (CAENVME_WriteCycle(bhandle,address,data,am,cvD16) != cvSuccess) {
      std::cerr << "[VME] <TDC::writeRegister (cvD16)> ERROR: Read at "
                << std::hex << addr << std::endl;
      return -1;
  }
  return 0;
}

int tdcV1x90::writeRegister(mod_reg addr, uint32_t* data) {
  uint32_t address = baseaddr+addr;
  if (CAENVME_WriteCycle(bhandle,address,data,am,cvD32) != cvSuccess) {
      std::cerr << "[VME] <TDC::writeRegister (cvD32)> ERROR: Read at "
                << std::hex << addr << std::endl;
      return -1;
  }
  return 0;
}

int tdcV1x90::readRegister(mod_reg addr, uint16_t* data) {
  uint32_t address = baseaddr+addr;
  if (CAENVME_ReadCycle(bhandle,address,data,am,cvD16) != cvSuccess) {
      std::cerr << "[VME] <TDC::readRegister (cvD16)> ERROR: Read at "
                << std::hex << addr << std::endl;
      return -1;
  }
  return 0;
}

int tdcV1x90::readRegister(mod_reg addr, uint32_t* data) {
  uint32_t address = baseaddr+addr;
  if (CAENVME_ReadCycle(bhandle,address,data,am,cvD32) != cvSuccess) {
      std::cerr << "[VME] <TDC::readRegister (cvD32)> ERROR: Read at "
                << std::hex << addr << std::endl;
      return -1;
  }
  return 0;
}

bool tdcV1x90::waitMicro(micro_handshake mode) {
  uint16_t data;
  bool status = false;
  while(status == false) {
    readRegister(MicroHandshake,&data);
    switch(mode){
      case WRITE_OK:
        status = (bool)(data&1);
        break;
      case READ_OK:
        status = (bool)((data&2)/2);
        break;
      default:
        return false;
    }
  }
  return status;
}
