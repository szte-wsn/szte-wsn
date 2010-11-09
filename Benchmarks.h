/*
* Copyright (c) 2009, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Veress Krisztian
*         veresskrisztian@gmail.com
*/

#ifndef BENCHMARKS_H
#define BENCHMARKS_H

#include "Internal.h"
#include <AM.h>

#define INVALID_SENDER 0xFFFF

#define RT_BENCHMARK {
#define RT_BENCHMARK_END ,{INVALID_SENDER,0,{0,0},{0,0,0,0,0},{0,0},0,0}},

#define TIMER(POS) (1<<(POS))
#define REPLY_EDGE(POS) (1<<(POS))
#define NUM(QTY) {(QTY), (QTY)}

#define NO_REPLY      0
#define START_MSG_ID  1
#define TIMER_NONE {0,0}
#define NULL_TIMER    0

#define ALL AM_BROADCAST_ADDR

// Sending flags
enum {
  SEND_NONE       = 0,
  SEND_ON_INIT    = 1,
  SEND_ON_RECV    = 2,
  SEND_ON_TIMER   = 3,

  STOP_ON_ACK     = 1<<0,
  STOP_ON_TIMER   = 1<<1,
  
  NEED_ACK = 1,
  
  INFINITE = 0,
  INF_LOOP = 1
};

edge_t problemSet[][(MAX_EDGE_COUNT+1)] = {

/** TRIGGER PROBLEMS -------------------------------------- */

RT_BENCHMARK
  { 1, 2, TIMER_NONE , { SEND_ON_INIT,  0, 0, 0, 0 }, NUM(1), NO_REPLY, START_MSG_ID }
RT_BENCHMARK_END

RT_BENCHMARK
  { 1, 2, TIMER_NONE , { SEND_ON_INIT,  0, 0, 0, 0 }, NUM(4), NO_REPLY, START_MSG_ID }
RT_BENCHMARK_END

RT_BENCHMARK
  { 1, 2, TIMER_NONE , { SEND_ON_INIT,  0, 0, INF_LOOP, 0 }, NUM(INFINITE), NO_REPLY, START_MSG_ID }
RT_BENCHMARK_END

/* 0. */
RT_BENCHMARK
  { 1, 2, { TIMER(1) , NULL_TIMER}, { SEND_ON_TIMER,  0, 0, 0, 0}, NUM(1), NO_REPLY, START_MSG_ID }
RT_BENCHMARK_END

/* 1. */
RT_BENCHMARK
  { 1, 2, { TIMER(1), TIMER(2) }, { SEND_ON_TIMER,  STOP_ON_TIMER, 0, 0, 0}, NUM(10), NO_REPLY, START_MSG_ID },
  { 2, 1, { TIMER(2), TIMER(1) }, { SEND_ON_TIMER,  STOP_ON_TIMER, 0, 0, 0}, NUM(5),  NO_REPLY, START_MSG_ID }
RT_BENCHMARK_END
  
/* 2. */
RT_BENCHMARK
  { 1, 2, { TIMER(1) , NULL_TIMER}, { SEND_ON_TIMER,  0, 0, 0, 0}, NUM(2), NO_REPLY, START_MSG_ID },
  { 2, 1, { TIMER(2) , NULL_TIMER}, { SEND_ON_TIMER,  0, 0, 0, 0}, NUM(3), NO_REPLY, START_MSG_ID }
RT_BENCHMARK_END

/* 3. */
RT_BENCHMARK
  { 1, 2,   { TIMER(1) , NULL_TIMER}, { SEND_ON_TIMER,  0, 0, 0, 0}, NUM(2), NO_REPLY, START_MSG_ID },
  { 1, 3,   { TIMER(2) , NULL_TIMER}, { SEND_ON_TIMER,  0, 0, 0, 0}, NUM(1), NO_REPLY, START_MSG_ID },
  { 2, ALL, { TIMER(3) , NULL_TIMER}, { SEND_ON_TIMER,  0, 0, 0, 0}, NUM(3), NO_REPLY, START_MSG_ID }
RT_BENCHMARK_END

/** MAXIMAL THROUGHPUT PROBLEMS -------------------------------------- */

/* 4. */
RT_BENCHMARK
  { 1, 2, TIMER_NONE, { SEND_ON_INIT,  0, NEED_ACK, 0, 0}, NUM(INFINITE), NO_REPLY, START_MSG_ID }
RT_BENCHMARK_END
  
/* 5. */
RT_BENCHMARK
  { 1, ALL, TIMER_NONE, { SEND_ON_INIT, 0, 0, 0, 0}, NUM(INFINITE), NO_REPLY, START_MSG_ID }
RT_BENCHMARK_END

/* 6. */
RT_BENCHMARK
  { 1, 2, TIMER_NONE, { SEND_ON_INIT, STOP_ON_ACK, NEED_ACK, 0, 0}, NUM(INFINITE), NO_REPLY, START_MSG_ID },
  { 2, 1, {NULL_TIMER, TIMER(1)}, { SEND_ON_INIT, STOP_ON_TIMER, 0, 0, 0}, NUM(INFINITE), NO_REPLY, START_MSG_ID }
RT_BENCHMARK_END

/** PING-PONG PROBLEMS -------------------------------------- */

/* 7. */
RT_BENCHMARK
  { 1, 2, TIMER_NONE, { SEND_ON_INIT, 0, 0, 0, 0}, NUM(1), REPLY_EDGE(2), START_MSG_ID },
  { 1, 2, TIMER_NONE, { SEND_ON_RECV, 0, 0, 0, 0}, NUM(1), REPLY_EDGE(2), START_MSG_ID },
  { 2, 1, TIMER_NONE, { SEND_ON_RECV, 0, 0, 0, 0}, NUM(1), REPLY_EDGE(1), START_MSG_ID }
RT_BENCHMARK_END

/* 8. */
RT_BENCHMARK
  { 1, 2, TIMER_NONE, { SEND_ON_INIT, 0, 0, 0, 0}, NUM(1), REPLY_EDGE(2), START_MSG_ID },
  { 1, 2, TIMER_NONE, { SEND_ON_RECV, 0, 0, 0, 0}, NUM(1), REPLY_EDGE(2), START_MSG_ID },
  { 2, 1, TIMER_NONE, { SEND_ON_RECV, 0, 0, 0, 0}, NUM(1), REPLY_EDGE(1), START_MSG_ID },
  
  { 2, 3, TIMER_NONE, { SEND_ON_INIT, 0, 0, 0, 0}, NUM(1), REPLY_EDGE(5), START_MSG_ID },
  { 2, 3, TIMER_NONE, { SEND_ON_RECV, 0, 0, 0, 0}, NUM(1), REPLY_EDGE(5), START_MSG_ID },
  { 3, 2, TIMER_NONE, { SEND_ON_RECV, 0, 0, 0, 0}, NUM(1), REPLY_EDGE(4), START_MSG_ID },
  
  { 1, 3, TIMER_NONE, { SEND_ON_INIT, 0, 0, 0, 0}, NUM(1), REPLY_EDGE(8), START_MSG_ID },
  { 1, 3, TIMER_NONE, { SEND_ON_RECV, 0, 0, 0, 0}, NUM(1), REPLY_EDGE(8), START_MSG_ID },
  { 3, 1, TIMER_NONE, { SEND_ON_RECV, 0, 0, 0, 0}, NUM(1), REPLY_EDGE(7), START_MSG_ID }
RT_BENCHMARK_END

/* 9. */
RT_BENCHMARK
  { 1, 2, { TIMER(1), TIMER(2) }, { SEND_ON_TIMER, STOP_ON_TIMER, 0, 0, 0}, NUM(INFINITE), REPLY_EDGE(1), START_MSG_ID },
  { 2, 3, TIMER_NONE, { SEND_ON_RECV, 0, 0, 0, 0}, NUM(1), REPLY_EDGE(2), START_MSG_ID },
  { 3, 4, TIMER_NONE, { SEND_NONE, 0, 0, 0, 0}, NUM(1), NO_REPLY, START_MSG_ID }
RT_BENCHMARK_END

}; // problemSet END

#define PROBLEMSET_COUNT 13

#endif
