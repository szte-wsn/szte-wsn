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

#ifndef INTERNAL_H
#define INTERNAL_H

#define MAX_EDGE_COUNT  1
#define MAX_NODE_COUNT  10
#define MAX_TIMER_COUNT 4

#if MAX_EDGE_COUNT <= 8
  typedef uint8_t pending_t;
#elif MAX_EDGE_COUNT <= 16
  typedef uint16_t pending_t;
#elif MAX_EDGE_COUNT <= 32
  typedef uint32_t pending_t;
#elif MAX_EDGE_COUNT <= 64
  typedef uint64_t pending_t;
#else
  #error "MAX_EDGE_COUNT is set too high! The current limit is 64!"
#endif
typedef pending_t edgeaddr_t;

#if MAX_TIMER_COUNT <= 4
  typedef uint8_t timer_base_t;
  #define TIMER_BIT_COUNT 4
#elif MAX_TIMER_COUNT <= 8
  typedef uint16_t timer_base_t;
  #define TIMER_BIT_COUNT 8  
#elif MAX_TIMER_COUNT <= 16
  typedef uint32_t timer_base_t;
  #define TIMER_BIT_COUNT 16  
#elif MAX_TIMER_COUNT <= 32
  typedef uint64_t timer_base_t;
  #define TIMER_BIT_COUNT 32  
#else
  #error "MAX_TIMER_COUNT is set too high! The current limit is 32!"
#endif

enum {
  // Policy flags
  GLOBAL_USE_ACK           = 1<<0,
  GLOBAL_USE_BCAST         = 1<<1,
  GLOBAL_USE_LPL           = 1<<2,
};

typedef struct flag_t {
  uint8_t start_trigger : 3;      // When to start sending messages
  uint8_t stop_trigger : 2;       // When to stop an infinite sending loop
  uint8_t need_ack : 1;           // ACK is needed?
  uint8_t inf_loop_on : 1;        // Whether an infinite sending loop is active
  uint8_t reserved : 1;           // Reserved for future expansion
} flag_t;

typedef struct timermask_t {
  timer_base_t start_timers: TIMER_BIT_COUNT;
  timer_base_t stop_timers: TIMER_BIT_COUNT;
} timermask_t;


typedef struct num_t {
  uint8_t send_num : 4;           // How many messages to transmit in general
  uint8_t left_num : 4;           // How many messages are left to transmit
} num_t;

typedef struct edge_t {
  uint16_t      sender;           // Sender end of the edge
  uint16_t      receiver;         // Receiver end of the edge
  timermask_t   timers;           // Timers associated to this edge
  flag_t        policy;           // Sending policies, settings, triggers
  num_t         nums;             // Message counters
  edgeaddr_t    reply_on;         // The edge bitmask used when sending on reception
  uint16_t      nextmsgid;        // The message id to send (on send side)/expected to receive (on receive side)
} edge_t;

// Base type
typedef nx_uint16_t stat_base_t;

// Stats type
typedef nx_struct stat_t {
  stat_base_t   triggerCount;
  stat_base_t   backlogCount;
  stat_base_t   resendCount;

  stat_base_t   sendCount;
  stat_base_t   sendSuccessCount;
  stat_base_t   sendFailCount;

  stat_base_t   sendDoneCount;
  stat_base_t   sendDoneSuccessCount;
  stat_base_t   sendDoneFailCount;

  stat_base_t   wasAckedCount;
  stat_base_t   notAckedCount;

  stat_base_t   receiveCount;
  stat_base_t   expectedCount;
  stat_base_t   duplicateCount;
  stat_base_t   forwardCount;
  stat_base_t   missedCount;
  stat_base_t   wrongCount;

  nx_uint8_t    remainedCount;
} stat_t;

typedef nx_struct profile_t {
  nx_uint32_t   max_atomic;
  nx_uint32_t   max_interrupt;
  nx_uint32_t   max_latency;
} profile_t;


// Basic setup type
typedef nx_struct setup_t {
  nx_uint8_t    problem_idx;      // The problem we should test
  
  nx_uint32_t   pre_run_msec;
  nx_uint32_t   runtime_msec;     // How long should we run the test?
  nx_uint32_t   post_run_msec;
  
  nx_uint8_t    flags;            // Global flags ( such as ACK, LPL, ... )
  nx_uint32_t   timer_start_seed;
  nx_uint32_t   timer_freq[MAX_TIMER_COUNT];
} setup_t;

#endif
