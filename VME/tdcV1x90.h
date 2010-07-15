#ifndef TDCV1x90_H 
#define TDCV1x90_H

#define DEBUG
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>

#include <string.h>
#include <stdio.h>

#include "CAENVMElib.h"
#include "CAENVMEoslib.h"
#include "CAENVMEtypes.h"

#include "tdcV1x90Opcodes.h"
//#include "../lib/dataStruct.h"

typedef enum {
  MATCH_WIN_WIDTH        = 0,
  WIN_OFFSET             = 1,
  EXTRA_SEARCH_WIN_WIDTH = 2,
  REJECT_MARGIN          = 3,
  TRIG_TIME_SUB          = 4,
} trig_conf;

typedef enum {
  WRITE_OK      = 0, /*!< \brief Is the TDC ready for writing? */
  READ_OK       = 1, /*!< \brief Is the TDC ready for reading? */
} micro_handshake;

typedef enum {
  CONT_STORAGE,
  TRIG_MATCH,
} acq_mode;

typedef enum {
  PAIR      = 0,
  OTRAILING = 1,
  OLEADING  = 2,
  TRAILEAD  = 3,
} det_mode;

typedef enum {
  DATA_READY    = 0,
  ALM_FULL      = 1,
  FULL          = 2,
  TRG_MATCH     = 3,
  HEADER_EN     = 4,
  TERM_ON       = 5,
  ERROR0        = 6,
  ERROR1        = 7,
  ERROR2        = 8,
  ERROR3        = 9,
  BERR_FLAG     = 10,
  PURG          = 11,
  RES_1         = 12,
  RES_2         = 13,
  PAIRED        = 14,
  TRIGGER_LOST  = 15,
} stat_reg;

typedef enum {
  BERREN                           = 0,
  TERM                             = 1,
  TERM_SW                          = 2,
  EMPTY_EVENT                      = 3,
  ALIGN64                          = 4,
  COMPENSATION_ENABLE              = 5,
  TEST_FIFO_ENABLE                 = 6,
  READ_COMPENSATION_SRAM_ENABLE    = 7,
  EVENT_FIFO_ENABLE                = 8,
  EXTENDED_TRIGGER_TIME_TAG_ENABLE = 9,
} ctl_reg;

typedef enum {

  //Register OutputBuffer   (0x0000),
  Control                 = 0x1000, // D16 R/W
  Status                  = 0x1002, // D16 R
  InterruptLevel          = 0x100a, // D16 R/W
  InterruptVector         = 0x100c, // D16 R/W
  GeoAddress              = 0x100e, // D16 R/W
  MCSTBase                = 0x1010, // D16 R/W
  MCSTControl             = 0x1012, // D16 R/W
  ModuleReset             = 0x1014, // D16 W
  SoftwareClear           = 0x1016, // D16 W
  BLTEventNumber          = 0x1024, // D16 R/W
  FirmwareRev             = 0x1026, // D16 R
  Micro                   = 0x102e, // D16 R/W
  MicroHandshake          = 0x1030, // D16 R
  
  EventFIFO               = 0x1038, // D32 R
  EventFIFOStoredRegister = 0x103c, // D16 R
  EventFIFOStatusRegister = 0x103e, // D16 R  
  
  ROMOui2                 = 0x4024,
  ROMOui1                 = 0x4028,
  ROMOui0                 = 0x402c,
  
  ROMBoard2               = 0x4034,
  ROMBoard1               = 0x4038,
  ROMBoard0               = 0x403c,
  ROMRevis3               = 0x4040,
  ROMRevis2               = 0x4044,
  ROMRevis1               = 0x4048,
  ROMRevis0               = 0x404c,
  ROMSerNum1              = 0x4080,
  ROMSerNum0              = 0x4084,
  
} mod_reg;

typedef enum {
  global_header = 0x8,
  tdc_header = 0x1,
  tdc_measur = 0x0,
  tdc_trailer = 0x3,
  tdc_error = 0x4,
  ettt = 0x11,
  global_trailer = 0x10,
  filler = 0x18,
} word_type;

/* Hit structure (N in each event) */
struct hit_t {
  int tdc; /* TDC identification */
  int trailead; /* Trailing or leading measurement */
  int channel; /* Channel */
  int hit_id; /* ? */
  int bunch_id; /* ? */
  int tdc_measur; /* */
  int word_count; /* ? */
  int error_flags; /* ? */
};

/* Event structure (one for each trigger) */

struct event_t {
  int geo; /* GEO address */
  int ettt; /* Extended trigger time tag */
  int status; /* TDC error, output buffer overflow, trigger lost */
  int word_count; /* ? */
  int cur_pos; /* Current position in the hits memory zone */
  hit_t * hits;
  int nb_hits;
};

class tdcV1x90 {

  public:
    tdcV1x90(int32_t, uint32_t, acq_mode, det_mode);
    ~tdcV1x90();
    
    bool Initialize(acq_mode mode);
    uint32_t getModel();
    uint32_t getOUI();
    uint32_t getSerNum();
    bool checkConfiguration();
    
    void setPoI(uint16_t);
    void setAcquisitionMode(acq_mode);
    bool setTriggerMatching();
    bool isTriggerMatching();
    bool setContinuousStorage();
    void getFirmwareRev();
    
    void setDetection(det_mode);
    det_mode readDetection();
    
    void setTDCEncapsulation(bool);
    bool getTDCEncapsulation();
    
    void readResolution(det_mode);
    void setPairModeResolution(int,int);

    void setBLTEventNumberRegister(uint16_t);
    uint16_t getBLTEventNumberRegister();
    
    void setWindowWidth(uint16_t);
    void setWindowOffset(int16_t);
    uint16_t readTrigConf(trig_conf);

    bool waitMicro(micro_handshake);
    bool softwareClear();
    bool softwareReset();
    bool hardwareReset();
    
    bool getStatusRegister(stat_reg);
    void setStatusRegister(stat_reg,bool);
    bool getCtlRegister(ctl_reg);
    void setCtlRegister(ctl_reg,bool);
    
    void setETTT(bool);
    bool getETTT();
    
    bool getEvents();
    void eventFill(uint32_t,event_t*);

    bool isEventFIFOReady();
    void setFIFOSize(uint16_t);
    void readFIFOSize();
    
	  // Close/Clean everything before exit
    void abort();

   /*!\brief Write on register
    *
    * Write a word in the register
    * \param[in] addr register
    * \param[in] data word
    * \return 0 on success
    * \return -1 on error
    */
    int writeRegister(mod_reg,uint16_t*);
    int writeRegister(mod_reg,uint32_t*);
   /*!\brief Read on register
    *
    * Read a word in the register
    * \param[in] addr register
    * \param[out] data word
    * \return 0 on success
    * \return -1 on error
    */  
    int readRegister(mod_reg,uint16_t*);
    int readRegister(mod_reg,uint32_t*);


  private:
    uint32_t baseaddr;
    int32_t bhandle;
    CVAddressModifier am; // Address modifier
    CVAddressModifier am_blt; // Address modifier (Block Transfert)
    
    uint32_t* buffer;

    det_mode detm;
    acq_mode acqm;
    
    bool outBufTDCHeadTrail;
    bool outBufTDCErr;
    bool outBufTDCTTT;

    uint32_t nchannels;
    bool gEnd;

    std::string pair_lead_res[8]; 
    std::string pair_width_res[16];
    std::string trailead_edge_res[4];

};

#endif /* TDCV1x90_H */

