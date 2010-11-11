#define  _DEBUG_MODE_

#include "Internal.h"
#include "Benchmarks.h"

#define SET_STATE(s) atomic { call Leds.set(s); _state = s; }

#if defined(_DEBUG_MODE_)
  #define _ASSERT_(cond) if(!(cond || dbgLINE)){ dbgLINE = __LINE__; }
#else
  #define str(s) s
  #define _ASSERT_(cond) str()
#endif

module BenchmarkCoreP @safe() {

  provides {
    interface Init;
    interface StdControl as Test;
    interface BenchmarkCore;
  }

  uses {
    interface Receive as RxTest;
    interface AMSend  as TxTest;
  
    interface Timer<TMilli> as TestTimer;
//    interface Timer<TMilli> as TriggerTimer;
    
    interface Packet;
    interface PacketAcknowledgements as Ack;
    
    interface LowPowerListening;
    interface Leds;
  }

}

implementation {

  enum {
    // Mote states
    STATE_INVALID     = 0x0,
    STATE_IDLE        = 0x1,
    STATE_CONFIGURED  = 0x2,
    STATE_PRE_RUN     = 0x3,
    STATE_RUNNING     = 0x4,
    STATE_POST_RUN    = 0x5,
    STATE_FINISHED    = 0x6,
    STATE_UPLOADING   = 0x7,
    
    // Sendlock states
    UNLOCKED          = 0,
    LOCKED            = 1

  };
  
  uint8_t _state, sendlock;
  setup_t _config;  
  stat_t  stats[MAX_EDGE_COUNT];
  
  message_t   pkt;
  edge_t*     problem;

  // Bitmask specifying edges with pending send requests
  pending_t pending;
  
  // Last edge index on which we have sent message
  uint8_t  eidx = 0xFF;
  
  #ifdef _DEBUG_MODE_
  uint16_t dbgLINE;
  #endif
  
  task void sendPending();
  
  /** CLEAN THE STATE MACHINE VARIABLES **/
  void cleanstate() {
   
    // Disassociate the problem    
    problem = (edge_t*)NULL;
    
    // Clear stats and configuration values
    memset(stats,0,sizeof(stat_t)*MAX_EDGE_COUNT);
    memset(&_config,0,sizeof(setup_t));
    
    pending = 0x0;
    eidx = 0xFF;
      
    #ifdef _DEBUG_MODE_
    dbgLINE = 0;
    #endif
      
    sendlock = UNLOCKED;
    SET_STATE( STATE_IDLE )
  }
  
  void postNewTrigger(pending_t sbitmask) {
    uint8_t i = 0;
    pending_t blogd;
    
    _ASSERT_( sbitmask > 0 )
    _ASSERT_(_state == STATE_RUNNING || _state == STATE_IDLE )
    
    atomic {
      // Check which edges need to be backlogged
      blogd = pending & sbitmask;
      pending |= sbitmask;
    }

    // Count backlog values
    for ( i = 0; blogd; ++i, blogd >>= 1) {
      if ( blogd & 0x1 )
        ++(stats[i].backlogCount);
    }
    
    // Count trigger values
    for ( i = 0; sbitmask; ++i, sbitmask >>= 1) {
      if ( sbitmask & 0x1 ) {
        ++(stats[i].triggerCount);
        if ( problem[i].nums.send_num == INFINITE )
          problem[i].policy.inf_loop_on = TRUE;
      }
    }
    post sendPending();
  }
  
  command error_t Init.init() {
    cleanstate();
    return SUCCESS;
  }
  
  /** REQUEST BECHMARK RESULTS **/
  command stat_t* BenchmarkCore.getStat(uint16_t idx) { 
    _ASSERT_( idx < MAX_EDGE_COUNT )
    _ASSERT_( _state == STATE_FINISHED )
    return stats + idx;
  }
  
  command uint16_t BenchmarkCore.getDebug() {
    #ifdef _DEBUG_MODE_
    return dbgLINE;
    #else
    return 0;
    #endif
  }
  
  /** SETUP THE BENCHMARK **/
  command void BenchmarkCore.setup(setup_t config) {
    uint8_t   idx;
    
    _ASSERT_( _state == STATE_INVALID || _state == STATE_IDLE || _state == STATE_CONFIGURED )
    _ASSERT_( config.problem_idx < PROBLEMSET_COUNT );
    _ASSERT_( config.runtime_msec > 0 );
    
    // Do nothing if already configured or running or data is available
    if ( _state >= STATE_CONFIGURED )
      return;
    
    // Get clean variables and save the configuration
    cleanstate();
    _ASSERT_ ( call Ack.noAck(&pkt) == SUCCESS )
    
    _config = config;
    
    // Setup the problem and initialize the edges
    problem = problemSet[_config.problem_idx];
    for( idx = 0; problem[idx].sender != INVALID_SENDER; ++idx )
    {
      // Clean values that are changed during operation
      problem[idx].policy.inf_loop_on = FALSE;
      problem[idx].nums.left_num = problem[idx].nums.send_num;
      problem[idx].nextmsgid = START_MSG_ID;
            
      // If the sender is not this node, continue
      if( problem[idx].sender != TOS_NODE_ID )
        continue;

      // Set the pending bits if this node needs to send at start        
      if ( problem[idx].policy.start_trigger == SEND_ON_INIT )
        postNewTrigger( 1<<idx );
    }
    SET_STATE( STATE_CONFIGURED )
    signal BenchmarkCore.setupDone();
  }
  
  /** RESETS THE CORE **/
  command void BenchmarkCore.reset() {
    SET_STATE ( STATE_INVALID );
    call TestTimer.stop();
    
    // TODO:
    //call TriggerTimer.stop();

    cleanstate();
    signal BenchmarkCore.resetDone();
  }
  
  /** START THE REAL BENCHMARK */
  void startBenchmark() {
    // If this node sends initial message(s)
    if ( pending )
      post sendPending();

    // Start the test timer      
    call TestTimer.startOneShot(_config.runtime_msec); 
  }
  
  /** START THE CURRENTLY CONFIGURED BENCHMARK */
  command error_t Test.start() { 
    _ASSERT_( _state == STATE_CONFIGURED )
    
    // If a pre-benchmark delay is requested, make a delay
    if ( _config.pre_run_msec > 0 ) {
      SET_STATE ( STATE_PRE_RUN )
      call TestTimer.startOneShot( _config.pre_run_msec );
    } else {
      SET_STATE( STATE_RUNNING )
      startBenchmark();         
    }
    
    //TODO: TimerFarm start();
    
    return SUCCESS; 
  }
    
  /** STOP A TEST */
  command error_t Test.stop() {
    _ASSERT_( _state == STATE_PRE_RUN || _state == STATE_RUNNING || _state == STATE_POST_RUN )
    
    call TestTimer.stop();
    SET_STATE( STATE_FINISHED );
    
    // TODO: TimerFarm stop
    signal BenchmarkCore.finished();
  }
    
  event void TestTimer.fired() {
    uint8_t i = 0;
    switch(_state) {
    
      case STATE_PRE_RUN:
        SET_STATE( STATE_RUNNING )
        startBenchmark();
        break;
        
      case STATE_RUNNING:
        
        // TODO: TimerFarm stop
        
        // check if we need a post-run state
        if ( _config.post_run_msec > 0 ) {
          SET_STATE( STATE_POST_RUN )
          call TestTimer.startOneShot(_config.post_run_msec);
          break;
        } 
        // break; missing: fallback to STATE_POST_RUN !
 
      case STATE_POST_RUN:
        SET_STATE( STATE_FINISHED )
        // compute the remained statistic
        for ( i = 0; pending; ++i, pending >>= 1) {
          if ( pending & 0x1 )
            ++(stats[i].remainedCount);
        }
        signal BenchmarkCore.finished();
        break;
        
      default:
        _ASSERT_( FALSE )
    }
  }
  
  // event void TriggerTimer.fired() {}
  
  event message_t* RxTest.receive(message_t* bufPtr, void* payload, uint8_t len) {
    testmsg_t* msg = (testmsg_t*)payload;
    // helper variables
    stat_t* stat = stats + msg->edgeid;
    edge_t* edge = problem + msg->edgeid;     
     
    // In case the message is sent to this mote (also)
    if (  ( _state == STATE_RUNNING || _state == STATE_POST_RUN ) && 
          ( edge->receiver == TOS_NODE_ID || edge->receiver == AM_BROADCAST_ADDR ) ){

      ++(stat->receiveCount);

      // If the message id is ok
      if ( msg->msgid == edge->nextmsgid ) {
        ++(stat->expectedCount);

      } else {
        ++(stat->wrongCount);
        
        // If we got a message with a lower id than expected -> duplicate
        if ( msg->msgid < edge->nextmsgid )
          ++(stat->duplicateCount);
        // If we got a message with a higher id than expected -> we have missed messages
        else {
          ++(stat->forwardCount);
          stat->missedCount += msg->msgid - edge->nextmsgid;
        }
      }

      // Set the next expected message id
      edge->nextmsgid = msg->msgid + 1;

      // Check whether we have to reply
      if ( edge->reply_on )
        postNewTrigger(edge->reply_on);
    }
    return bufPtr;

  }
  
  event void TxTest.sendDone(message_t* bufPtr, error_t error) {
  
    testmsg_t* msg = (testmsg_t*)(call Packet.getPayload(bufPtr,sizeof(testmsg_t)));
    bool validSend = TRUE, wasACK = FALSE, sendMore = TRUE;
    
    // helper variables
    stat_t* stat = stats + msg->edgeid;
    edge_t* edge = problem + msg->edgeid;

    _ASSERT_( sendlock == LOCKED )
    _ASSERT_( _state == STATE_RUNNING || _state == STATE_POST_RUN || _state == STATE_FINISHED )
    
    if ( _state == STATE_RUNNING || _state == STATE_POST_RUN ) {

      _ASSERT_( edge->sender == TOS_NODE_ID )
      _ASSERT_( pending & (1 << msg->edgeid) )
      ++(stat->sendDoneCount);

      if ( error == SUCCESS ) {
        ++(stat->sendDoneSuccessCount);

        // If ACK is not requested
        if ( edge->policy.need_ack == 0 && (_config.flags & GLOBAL_USE_ACK) == 0 ) {
          ++(edge->nextmsgid);

        // If ACK is requested and received
        } else if ( call Ack.wasAcked(bufPtr) ) {
          ++(edge->nextmsgid);
          ++(stat->wasAckedCount);
          wasACK = TRUE;
            
        // Else the message is not considered to be sent
        } else {
          ++(stat->notAckedCount);
          validSend = FALSE;
        }
        
      } else {
        ++(stat->sendDoneFailCount);
        validSend = FALSE;
      }

      // If message is considered to be sent
      if ( validSend ) {
        
        // Decrement the number of messages that are left to send
        // and restore the original value if necessary
        // this works for INFINITE and also for non-INF edges
        if ( edge->nums.send_num != INFINITE && --(edge->nums.left_num) == 0 ) {
          // Restore the value
          edge->nums.left_num = edge->nums.send_num;
          sendMore = FALSE;
        }
       
        // Check if we need to stop sending on ACK
        if ( wasACK && edge->policy.stop_trigger & STOP_ON_ACK ) {
            // This works for INFINITE and also for non-INF edges
            edge->policy.inf_loop_on = FALSE;
            edge->nums.left_num = edge->nums.send_num;
            sendMore = FALSE;
        }
            
      } else {
        ++(stat->resendCount);
        ++(stat->triggerCount);
        // sendMore == TRUE;
      } 
          
      // Remove the pending bit if applicable     
      if ( !sendMore ) {
        atomic { pending &= ~ (1 << msg->edgeid ); }
      } 
            
      sendlock = UNLOCKED;
      if ( pending )
        post sendPending();
    }
  }


  task void sendPending() {
    
    pending_t   pidx;
    am_addr_t   address;
    uint8_t     oldlock;
    testmsg_t*  t_msg;
    
    // In case we have any chance to send    
    if ( _state == STATE_RUNNING && pending ) {

      // safe locking    
      atomic{
        oldlock = sendlock;
        sendlock = LOCKED;
      }
      if ( oldlock == LOCKED )
        return;
     
      // find the next edge on which there exist any request
      do {
        pidx = 1 << (++eidx);
        if ( pidx == 0 ) {
          eidx  = 0x0;
          pidx  = 0x1;
        }
      } while ( !(pending & pidx) );
      
      _ASSERT_( problem[eidx].sender == TOS_NODE_ID )
        
      // Compose the new message
      call Packet.clear(&pkt);
      t_msg = (testmsg_t*)(call Packet.getPayload(&pkt,sizeof(testmsg_t)));
      t_msg->edgeid = eidx;
      t_msg->msgid = problem[eidx].nextmsgid;
      
      // Find out the required addressing mode
      address = ( _config.flags & GLOBAL_USE_BCAST ) ? AM_BROADCAST_ADDR : problem[eidx].receiver;
      
      // Find out whether we need to use ACK
      if ( (_config.flags & GLOBAL_USE_ACK) || problem[eidx].policy.need_ack == 1 ) {
        call Ack.requestAck(&pkt);
      } else {
        call Ack.noAck(&pkt);
      }
      // Send out
      switch ( call TxTest.send( address, &pkt, sizeof(testmsg_t)) ) {
        case SUCCESS :
          ++(stats[eidx].sendSuccessCount);
          break;
        case FAIL :
          ++(stats[eidx].sendFailCount);
          ++(stats[eidx].resendCount);
          sendlock = UNLOCKED;
          post sendPending();
          break;
        default :
          _ASSERT_( 0 );
          break;
      }
      ++(stats[eidx].sendCount);
    }
  }

}
