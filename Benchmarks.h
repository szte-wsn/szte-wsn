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
* Author: Krisztian Veress
*         veresskrisztian@gmail.com
*/

#ifndef BENCHMARKS_H
#define BENCHMARKS_H

#include "Internal.h"
#include <AM.h>

#define RT_BENCHMARK_END ,{INVALID_SENDER,0,{0,0},{0,0,0,0,0},{0,0},0,0},
#define PROBLEMSET_END   {0,0,{0,0},{0,0,0,0,0},{0,0},0,0}

#define INVALID_SENDER  AM_BROADCAST_ADDR

#define ALL             AM_BROADCAST_ADDR

#define REPLY_EDGE(POS) (1<<(POS))
#define NUM(QTY) {(QTY), (QTY)}

#define NO_REPLY      0
#define START_MSG_ID  1
#define NO_TIMER      {0,0}

#define TIMER(X)      ((X)-1)

edge_t problemSet[] = {

  { 1, 2, NO_TIMER , { SEND_ON_INIT,  0, 0, 0, 0 }, NUM(1), NO_REPLY, START_MSG_ID }
  RT_BENCHMARK_END

  { 1, 2, NO_TIMER , { SEND_ON_INIT,  0, 0, 0, 0 }, NUM(4), NO_REPLY, START_MSG_ID }
  RT_BENCHMARK_END

  { 1, 2, NO_TIMER , { SEND_ON_INIT,  STOP_ON_ACK, 0, 0, 0 }, NUM(4), NO_REPLY, START_MSG_ID }
  RT_BENCHMARK_END

  { 1, 2, NO_TIMER , { SEND_ON_INIT,  0, 0, 0, 0 }, NUM(INFINITE), NO_REPLY, START_MSG_ID }
  RT_BENCHMARK_END

  { 1, 2, NO_TIMER , { SEND_ON_INIT,  STOP_ON_ACK, 0, 0, 0 }, NUM(INFINITE), NO_REPLY, START_MSG_ID }
  RT_BENCHMARK_END

  { 1, 2, {TIMER(1),0} , { SEND_ON_TIMER,  0, 0, 0, 0 }, NUM(1), NO_REPLY, START_MSG_ID }
  RT_BENCHMARK_END

  { 1, 2, {TIMER(1),0} , { SEND_ON_TIMER,  STOP_ON_ACK, 0, 0, 0 }, NUM(10), NO_REPLY, START_MSG_ID }
  RT_BENCHMARK_END
  
  { 1, 2, {TIMER(1),TIMER(2)} , { SEND_ON_TIMER,  STOP_ON_TIMER, 0, 0, 0 }, NUM(INFINITE), NO_REPLY, START_MSG_ID }
  RT_BENCHMARK_END

  { 1, 2, {TIMER(1), 0} , { SEND_ON_TIMER,  0, 0, 0, 0 }, NUM(3), NO_REPLY, START_MSG_ID },
  { 2, 1, {0,TIMER(2)} , { SEND_ON_INIT,  STOP_ON_TIMER | STOP_ON_ACK , 0, 0, 0 }, NUM(INFINITE), NO_REPLY, START_MSG_ID }
  RT_BENCHMARK_END

  PROBLEMSET_END
}; // problemSet END

#endif
