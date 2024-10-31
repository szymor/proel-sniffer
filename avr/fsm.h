#ifndef _H_FSM
#define _H_FSM

#include <stdint.h>

typedef void (*fsm_cb)(uint8_t flatno);

void fsm_reset(void);
void fsm_set_cb(fsm_cb callback);
void fsm_push_event(uint8_t raising, uint32_t period);

#endif
