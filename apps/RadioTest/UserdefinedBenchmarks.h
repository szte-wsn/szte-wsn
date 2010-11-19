/** 
 * User Defined Benchmark Database file
 * ------------------------------------------------------------------------
 * 
 * This is a user-modifiable file, keep it clean, and stay to the rules below.
 *
 * Instructions for how to define a benchmark:
 *  1, All benchmarks MUST begin with a _BMARK_START_ macro.
 *  2, All benchmarks MUST end   with a _BMARK_END_   macro.
 *  3, Between these macros, the edges (allowed communication links between two)
 *     separate motes) of the modeled network are enlisted.
 *     
 *     Each edge is a 7-element structure :
 *     { SENDER, RECEIVER, TIMER_DESC, POLICY_DESC, MSG_COUNT, REPLY, 'START_MSG_ID' }
 *      
 *  4, SENDER:    - any positive number, denoting the mote id
 *     RECEIVER:  - any positive number other than the sender, denoting the mote id,
 *                - 'ALL', denoting all motes. This automatically implies
 *                   that on this edge, broadcasting is used
 *
 *     TIMER_DESC:
 *                - 'NO_TIMER', if timers are not used on this edge
 *                - {START_TIMER_DESC, STOP_TIMER_DESC} otherwise
 *
 *     START_TIMER_DESC:
 *     STOP_TIMER_DESC:
 *                - '0', if sending is not initiated by a timer
 *                - 'TIMER(X)', representing the Xth timer.
 *     
 *     POLICY_DESC:
 *                - { SEND_TRIG, STOP_TRIG, ACK, 0, 0 }
 *     SEND_TRIG: - 'SEND_NONE', to send never, nothing,
 *                - 'SEND_ON_INIT', to send message on benchmark start,
 *                - 'SEND_ON_TIMER', to send message on timer event (
 *                  see START_TIMER_DESC)
 *     STOP_TRIG: - '0', if no message sending stopper is required
 *                - 'STOP_ON_ACK', if message sending is required to stop on an ACK
 *                - 'STOP_ON_TIMER', if message sending is req. to stop on a timer event (
 *                  see STOP_TIMER_DESC)
 *     ACK:       - '0', if acknowledgements are not requested
 *                - 'NEED_ACK', if acknowledgements are requested
 *
 *     MSG_COUNT: - NUM(X), denoting X message to send.
 *
 *     REPLY:     - 'NO_REPLY', if message is not required to send on reception
 *                - 'REPLY_EDGE(X)', if message is to send on reception on edge X.
 *                - 'REPLY_EDGE(X) | REPLY_EDGE(Y) | ...', if message is to send on reception on edge X and Y
 *                   (the edge ids count from zero in the current benchmark)
 *
 *
 *  For examples, see below, and do not hesitate to erase them to reduce memory overhead.
 */


_BMARK_START_
  { 1, 2,   {TIMER(1),0}, { SEND_ON_TIMER,  STOP_ON_ACK, 0, 0, 0 }, NUM(2), REPLY_ON(1), START_MSG_ID },
  { 2, 3,   NO_TIMER , { SEND_ON_REQ,  0, 0, 0, 0 }, NUM(1), REPLY_ON(2), START_MSG_ID },
  { 3, 1,   NO_TIMER , { SEND_ON_REQ,  0, 0, 0, 0 }, NUM(1), NO_REPLY, START_MSG_ID }
_BMARK_END_

_BMARK_START_
  { 1, 2,   {TIMER(1),0}, { SEND_ON_TIMER, 0, NEED_ACK, 0, 0 }, NUM(2), REPLY_ON(2), START_MSG_ID },
  { 3, 2,   {TIMER(2),TIMER(3)}, { SEND_ON_TIMER, STOP_ON_TIMER | STOP_ON_ACK, 0, 0, 0 }, NUM(1), REPLY_ON(2), START_MSG_ID },
  { 2, ALL, NO_TIMER    , { SEND_ON_REQ,  0, 0, 0, 0 }, NUM(1), REPLY_ON(3), START_MSG_ID },
  { 3, 1,   NO_TIMER    , { SEND_ON_INIT,  0, 0, 0, 0 }, NUM(4) , NO_REPLY, START_MSG_ID }
_BMARK_END_


_BMARK_START_
  { 1, 2,   {TIMER(1),0}, { SEND_ON_TIMER,  STOP_ON_ACK, NEED_ACK, 0, 0 }, NUM(2), REPLY_ON(1), START_MSG_ID },
  { 2, 1,   NO_TIMER , { SEND_ON_REQ,  0, 0, 0, 0 }, NUM(2), NO_REPLY, START_MSG_ID }
_BMARK_END_


