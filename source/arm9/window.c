#include <nds/arm9/window.h>


//set window dimensions
void windowSetBounds(WINDOW w, u8 left,  u8 top, u8 right,u8 bottom) {
  sassert(w == WINDOW_0 || w == WINDOW_1, "Only WINDOW_0 or WINDOW_1 have bounds");

  if(w == WINDOW_0) {
    WIN0_X0 = left;
    WIN0_X1 = right;
    WIN0_Y0 = top;
    WIN0_Y1 = bottom;
  }
  else if(w == WINDOW_1) {
    WIN1_X0 = left;
    WIN1_X1 = right;
    WIN1_Y0 = top;
    WIN1_Y1 = bottom;
  }
}
void windowSetBoundsSub(WINDOW w, u8 left, u8 top,  u8 right,u8 bottom) {
  sassert(w == WINDOW_0 || w == WINDOW_1, "Only WINDOW_0 or WINDOW_1 have bounds.");

  if(w == WINDOW_0) {
    SUB_WIN0_X0 = left;
    SUB_WIN0_X1 = right;
    SUB_WIN0_Y0 = top;
    SUB_WIN0_Y1 = bottom;
  }
  else if(w == WINDOW_1) {
    SUB_WIN1_X0 = left;
    SUB_WIN1_X1 = right;
    SUB_WIN1_Y0 = top;
    SUB_WIN1_Y1 = bottom;
  }
}


//background id, the target window
void bgWindowEnable(int id, WINDOW w) {
  vu32 *win = 0;
  u32  mask = 0;

  sassert(id >= 0 && id <= 7, "Must choose a valid bg id.");

  if(id < 4)
    win = (vu32*)&WIN_IN;
  else {
    id -= 4;
    win = (vu32*)&SUB_WIN_IN;
  }

  if(w & WINDOW_0)   mask |= BIT(id);
  if(w & WINDOW_1)   mask |= BIT(id + 8);
  if(w & WINDOW_OUT) mask |= BIT(id + 16);
  if(w & WINDOW_OBJ) mask |= BIT(id + 24);

  *win |= mask;
}
void bgWindowDisable(int id, WINDOW w) {
  vu32 *win = 0;
  u32  mask = 0;

  sassert(id >= 0 && id <= 7, "Must choose a valid bg id.");

  if(id < 4)
    win = (vu32*)(&WIN_IN);
  else {
    id -= 4;
    win = (vu32*)(&SUB_WIN_IN);
  }

  if(w & WINDOW_0)   mask |= BIT(id);
  if(w & WINDOW_1)   mask |= BIT(id + 8);
  if(w & WINDOW_OUT) mask |= BIT(id + 16);
  if(w & WINDOW_OBJ) mask |= BIT(id + 24);

  *win &= ~mask;
}

void oamWindowEnable(OamState* oam, WINDOW w) {
  vu32 *win = 0;
  u32  mask = 0;

  sassert(oam == &oamMain || oam == &oamSub, "Must choose a valid OamState.");

  if(oam == &oamMain)
    win = (vu32*)(&WIN_IN);
  else
    win = (vu32*)(&SUB_WIN_IN);

  if(w & WINDOW_0)   mask |= BIT(4);
  if(w & WINDOW_1)   mask |= BIT(12);
  if(w & WINDOW_OUT) mask |= BIT(20);
  if(w & WINDOW_OBJ) mask |= BIT(28);

  *win |= mask;
}
void oamWindowDisable(OamState* oam, WINDOW w) {
  vu32 *win = 0;
  u32  mask = 0;

  sassert(oam == &oamMain || oam == &oamSub, "Must choose a valid OamState.");

  if(oam == &oamMain)
    win = (vu32*)(&WIN_IN);
  else
    win = (vu32*)(&SUB_WIN_IN);

  if(w & WINDOW_0)   mask |= BIT(4);
  if(w & WINDOW_1)   mask |= BIT(12);
  if(w & WINDOW_OUT) mask |= BIT(20);
  if(w & WINDOW_OBJ) mask |= BIT(28);

  *win &= ~mask; 
}