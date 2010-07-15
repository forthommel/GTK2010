#include "tdcV1x90.h"

//#include <stdio.h>
//#include <vector>

tdcV1x90::tdcV1x90(int32_t abhandle,uint32_t abaseaddr,acq_mode acqmode=TRIG_MATCH,det_mode detmode=TRAILEAD) {

  this->bhandle = abhandle;
  this->baseaddr = abaseaddr;
  this->am = cvA32_U_DATA;
  this->am_blt = cvA32_U_BLT;
  
  if (checkConfiguration()) {
    softwareReset();
    setAcquisitionMode(acqmode);
    
    detm = detmode;
    /*det_mode arf = PAIR;
    setDetection(arf);
    det_mode detect = readDetection();
    std::cout << "detection mode: " << detect << std::endl;*/
    
    setTDCEncapsulation(1);
    setPairModeResolution(0,0x4);
    //readResolution(detect);
    
    gEnd = false;
 }
     


  //FIXME horribly crappy!
  /*std::vector<std::string> pair_lead_res(8);
  /*vector<string> pair_width_res(16);
  vector<string> trailead_edge_res(4);
  
  pair_lead_res[] = {"100ps","200ps","400ps","800ps","1.6ns","3.12ns","6.25ns","12.5ns"};
  for (int i=0;i<8;i++) std::cout << "pair_lead_res[" << i << "]: " << pair_lead_res[i] << std::endl;*/
    
  char* c_pair_lead_res[] = {"100ps","200ps","400ps","800ps","1.6ns","3.12ns","6.25ns","12.5ns"};
  char* c_pair_width_res[] = {"100ps","200ps","400ps","800ps","1.6ns","3.2ns","6.25ns","12.5ns","25ns","50ns","100ns","200ns","400ns","800ns","invalid","invalid"};
  char* c_trailead_edge_res[] = {"800ps","200ps","100ps","25ps"};
  for(int i=0;i<8;i++) pair_lead_res[i]=c_pair_lead_res[i];
  for(int i=0;i<16;i++) pair_width_res[i]=c_pair_width_res[i];
  for(int i=0;i<4;i++) trailead_edge_res[i]=c_trailead_edge_res[i];

}

bool tdcV1x90::Initialize(acq_mode mode){

 
    
  //while(1){ //FIXME include a break
    //std::cout << "\033c";
      //getEvents(mode,detect);
  
    
    
    //std::cout << "Trigger Matching? " << getStatusRegister(TRG_MATCH) << std::endl;
    /*std::cout << "Full? " << getStatusRegister(FULL) << std::endl;
    if(isTriggerMatching()) setAcquisitionMode(CONT_STORAGE);
    std::cout << "Drdy? " << getStatusRegister(DATA_READY) << std::endl;*/
    
    //isEventFIFOReady();
  //}
  //else {
  // std::cerr << "[VME] <TDC> ERROR: Wrong configuration" << std::endl;
    //hardwareReset();
  //}
}

tdcV1x90::~tdcV1x90() { //FIXME implement destructor...
  
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
  
  #ifdef DEBUG
  std::cout << "[VME] <TDC::checkConfiguration> Debug:" << std::endl;
  std::cout << "       OUI manufacturer number is 0x"
            << std::hex << std::setfill('0') << std::setw(6) << oui << std::endl;
  std::cout << "                  Model number is "
            << std::dec << model << std::endl;
  std::cout << "                 Serial number is "
            << std::dec << getSerNum() << std::endl;
  #endif
  return true;
}

void tdcV1x90::setPoI(uint16_t word) {
  
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
        std::cerr << "[VME] <TDC::setContinuousStorage> ERROR while entering the continuous storage mode" << std::endl;
      break;
    case TRIG_MATCH:
      if (!(setTriggerMatching()))
        std::cerr << "[VME] <TDC::setTriggerMatching> ERROR while entering the trigger matching mode" << std::endl;
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
  setFIFOSize(7);
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

void tdcV1x90::setETTT(bool mode) {
  setCtlRegister(EXTENDED_TRIGGER_TIME_TAG_ENABLE,mode);
  outBufTDCTTT = mode;
}

bool tdcV1x90::getETTT() {
  return getCtlRegister(EXTENDED_TRIGGER_TIME_TAG_ENABLE);
}

bool tdcV1x90::getEvents() {
  // Start readout (check if BERR is set to 0)
  // Nw words are transmitted until the global TRAILER
  
  uint32_t* buffer=(uint32_t*)malloc(16*1024*1024); // 16Mb of buffer!

  int count;
  int blts = 512;
  for (int j=0;j<blts;j++){
    buffer[j]=0;
    if (CAENVME_BLTReadCycle(bhandle,baseaddr+0x0000,(char*)buffer,blts,am_blt,cvD32,&count) == cvSuccess) {
      if (gEnd == true) {
        std::cout << "Exit requested!" << std::endl;
        exit(0);
      }
      else std::cout << "number of events: " << count << std::endl;
    }
  }
  uint32_t value;
  uint16_t channel;
  uint16_t width;
  int trailing;
  
  /*ofstream myfile;
  myfile.open ("example.txt");
  myfile.close();*/
  //std::cout << "det: " << det << std::endl;
  if (acqm == CONT_STORAGE) {
    for (int i=0; i<count; i++) {
      value = buffer[i]&0x7FFFF;
      channel = (buffer[i]&0x3F80000)>>19;
      trailing = (buffer[i]&0x4000000)>>26;
      if (value != 0) {
        std::cout << "event " << std::dec << i << " \t channel " << channel << "\t";
        switch(detm) {
          case PAIR:
            width = (buffer[i]&0x7F000)>>12;
            value = buffer[i]&0xFFF;
            std::cout << std::hex << "width " << width << "\t\t value " << std::dec << value;
          break;
        case OTRAILING:
        case OLEADING:
          std::cout << "value " << std::dec << value << "\t trailing? " << trailing;
          break;
        case TRAILEAD:
          std::cout << "value " << std::dec << value << "\t trailing? " << trailing;
          break;
        default:
          std::cerr << "Error: not a registered detection mode: " << detm;
          break;
        }
        std::cout << std::endl;
      }
    }
  }
  else { // TRIGGER MATCHING MODE 
    //dummy[0] -> global_header (event = 42, geo = 1)
    //dummy[1] -> tdc_measurement [single edge] (trailing measurement, channel = 0, trailing time = 10) 
    int i;
    event_t evt; /* FIXME initilalize the structure */
    int blk_size = 3;
    int dummy[3] = {0x40000541,0x400000A,0x400000A};  
    /* First word of the block (!!! assuming Event Aligned BLT !!!) */ 
    if((dummy[0] >> 27) != global_header) {
      /* Abort */
      printf("First word is not the global header, abort\n");
    }
    /* Read the hits number */
    evt.nb_hits = (dummy[0]&0x7FFFFE0) >> 5;
    printf("%d hits in the event (trigger)\n",evt.nb_hits);
    /* Reserve memory for the hits FIXME reserve only one time, one big chunk of memory */
    evt.hits = (hit_t*) calloc(evt.nb_hits,sizeof(hit_t));
    evt.cur_pos = 0; /* move the cursor */	
    /* read the hits and fill the structure */
    for(i = 1;i < blk_size-1;i++) {
      eventFill(dummy[i],&evt);
    }
    /* Last word must be global_trailer */
    if((dummy[i] >> 27) != global_trailer) {
      /* Abort */
      printf("Last word is not the global trailer, abort\n");
    }

    /* --------*/

    /* Display the block */
    for(i = 0; i < evt.nb_hits; i++) {
      printf("Channel: %d, Measurement: %d\n",evt.hits[i].channel,evt.hits[i].tdc_measur);
    }
    /* Not forget to free the memory !*/
    free(evt.hits);
  }
  free(buffer);
  return true;
}

void tdcV1x90::eventFill(int word, event_t* evt) {
  int id_word = word >> 27;
  switch(id_word) {
    case global_header:
      /* Second global_header ?! Abort */
      printf("Second header, abort\n");
      break;
    case tdc_header:
      /* FIXME masking and shifting */
      evt->hits[evt->cur_pos].hit_id = 0;
      evt->hits[evt->cur_pos].bunch_id = 0;
      break;
    case tdc_measur:
      evt->hits[evt->cur_pos].tdc_measur = word&0x7ffff;
      evt->hits[evt->cur_pos].channel = (word&0x3f8000) >> 19;
      evt->hits[evt->cur_pos].trailead = (word&0x4000000) >> 26;
      break;
    case tdc_trailer:
      if(evt->cur_pos >= (evt->nb_hits - 1)) {
        /* Too many hits ! Abort */
        printf("Announced hits nb != actual hits nb, abort\n");
        break;
      }
      evt->hits[evt->cur_pos].word_count = 0;
      evt->cur_pos++; /* Last item of the hit, move to the next one */	
      break;
    case tdc_error:
      break;
    case ettt:
      break;
    case global_trailer:
      /* Trailer not in the last position, abort */
      printf("Trailer not in the last position\n");
      break;
    case filler:
      break;
    default:
      printf("Unknown word, abort\n");
    //Unknown word
  }
}

void tdcV1x90::abort() {
  #ifdef DEBUG
  std::cout << "[VME] <TDC::abort> DEBUG: received abort signal" << std::endl;
  #endif
  //Raise flag
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
