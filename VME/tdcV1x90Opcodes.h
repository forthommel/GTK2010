#ifndef __TDCV1x90OPCODES_H
#define __TDCV1x90OPCODES_H

namespace tdcV1x90Opcodes {
  
  #define Opcode static const unsigned short
  
  // Acquisition mode
  
  Opcode TRG_MATCH          (0x0000);
  Opcode CONT_STOR          (0x0100);
  Opcode READ_ACQ_MOD       (0x0200);
  Opcode SET_KEEP_TOKEN     (0x0300);
  Opcode CLEAR_KEEP_TOKEN   (0x0400);
  Opcode LOAD_DEF_CONFIG    (0x0500);
  Opcode SAVE_USER_CONFIG   (0x0600);
  Opcode LOAD_USER_CONFIG   (0x0700);
  Opcode AUTOLOAD_USER_CONF (0x0800);
  Opcode AUTOLOAD_DEF_CONFI (0x0900);
  
  // Trigger
  
  Opcode SET_WIN_WIDTH      (0x1000);
  Opcode SET_WIN_OFFS       (0x1100);
  Opcode SET_SW_MARGIN      (0x1200);
  Opcode SET_REJ_MARGIN     (0x1300);
  Opcode EN_SUB_TRG         (0x1400);
  Opcode DIS_SUB_TRG        (0x1500);
  Opcode READ_TRG_CONF      (0x1600);
  
  // TDC edge detection & resolution
  
  Opcode SET_DETECTION      (0x2200);
  Opcode READ_DETECTION     (0x2300);
  Opcode SET_TR_LEAD_LSB    (0x2400);
  Opcode SET_PAIR_RES       (0x2500);
  Opcode READ_RES           (0x2600);
  Opcode SET_DEAD_TIME      (0x2800);
  Opcode READ_DEAD_TIME     (0x2900);
  
  // TDC readout
  
  Opcode EN_HEAD_TRAILER    (0x3000);
  Opcode DIS_HEAD_TRAILER   (0x3100);
  Opcode READ_HEAD_TRAILER  (0x3200);
  Opcode SET_EVENT_SIZE     (0x3300);
  Opcode READ_EVENT_SIZE    (0x3400);
  Opcode EN_ERROR_MARK      (0x3500);
  Opcode DIS_ERROR_MARK     (0x3600);
  Opcode EN_ERROR_BYPASS    (0x3700);
  Opcode DIS_ERROR_BYPASS   (0x3800);
  Opcode SET_ERROR_TYPES    (0x3900);
  Opcode READ_ERROR_TYPES   (0x3a00);
  Opcode SET_FIFO_SIZE      (0x3b00);
  Opcode READ_FIFO_SIZE     (0x3c00);
  
  // Channel enable/disable
  
  Opcode EN_CHANNEL         (0x4000); // FIXME channel number must be implemented
  Opcode DIS_CHANNEL        (0x4100); // FIXME channel number must be implemented
  Opcode EN_ALL_CHANNEL     (0x4200);
  Opcode DIS_ALL_CHANNEL    (0x4300);
  Opcode WRITE_EN_PATTERN   (0x4400);
  Opcode READ_EN_PATTERN    (0x4500);
  Opcode WRITE_EN_PATTERN32 (0x4600);
  Opcode READ_EN_PATTERN32  (0x4700);
  
  // Adjust
  
  Opcode SET_GLOB_OFFS      (0x5000);
  Opcode READ_GLOB_OFFS     (0x5100);
  // TODO
  Opcode SET_RC_ADJ         (0x5400);
  Opcode READ_RC_ADJ        (0x5500);
  Opcode SAVE_RC_ADJ        (0x5600);

  // Miscellaneous
  // TODO
  
  // Advanced
  // TODO
  
  // Debug and test
  // TODO
};

#endif /* __TDCV1x90OPCODES_H */


