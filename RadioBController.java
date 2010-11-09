/** Copyright (c) 2009, University of Szeged
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

import java.io.*;
import java.util.concurrent.locks.*;
import java.util.concurrent.TimeUnit;
import java.util.Vector;
import net.tinyos.message.*;
import net.tinyos.util.*;
import org.apache.commons.cli.*;

public class RadioBController implements MessageListener {
	
	private MoteIF mif;
  private short stage, pidx;

  // Memory for downloaded data
  private Vector< StatT > stats;
//  private long[][]  finalMsgIds;
  private long[]    debuglines;
//  private long[][]  profiles;

  // Needed for proper downloading
  final Lock lock = new ReentrantLock();
  final Condition answered = lock.newCondition(); 
  private boolean handshake;
  
  private static short currentMote = 0, currentData = 0;
  private static final short  MAXPROBES   = 6;
  private static final int    MAXTIMEOUT  = 500;

  // Conformance to the NesC code
  // refer to RadioTestCases.h
  public static final short PROBLEMSET_COUNT = 13;
  private static short[] trproblem = new short[] { 0,0,0,1,0,0,0,0,0,0,0,1,1,1 };
  private static short[] motecount = new short[] { 2,2,2,3,2,2,3,3,2,3,3,4,5,2 };
  private static short[] edgecount = new short[] { 1,1,1,6,1,2,3,6,2,3,6,3,6,1 };

	public RadioBController(final short p)
	{
		mif = new MoteIF();
    mif.registerListener(new SyncMsgT(),this);
    mif.registerListener(new DataMsgT(),this);
    
    stage = 0;
    pidx = p;
 	}

	public void resetMotes()  
  {
    System.out.print("> Reset all motes  ... ");
    CtrlMsgT cmsg = new CtrlMsgT();
    // refer to RadioTest.h CTRL_RESET
    cmsg.set_type((short)20);
		try {
			mif.send(MoteIF.TOS_BCAST_ADDR,cmsg);
			Thread.sleep((int)(500));
      // We should go to pre-setup stage
      stage = 0;
      System.out.println("OK");
		} catch(IOException e) {
		  System.out.println("FAIL");
    } catch ( InterruptedException e ) {
      System.err.println("FAIL");
    }
	}

  public boolean setupMotes(final SetupT config) throws MissingOptionException {

/*    if ( trproblem[pidx] != 0 && config.get_timer_msec() == 0 )
      throw new MissingOptionException("Network problem " + pidx + " needs trigger timer specified!");
  */
    
    String out = "> Problem: " + config.get_problem_idx();
    out += " | Runtime: " + config.get_runtime_msec() + " ms";
    out += " | Trigger: " + ((trproblem[pidx] != 0) ? config.get_timer_freq()[0] : 0)  + " ms";
    out += " | LCTime: " + config.get_post_run_msec() + " ms";
    out += " | ACK/F-BC/LPL: ";
    out += ( config.get_flags() & 0x1 ) > 0 ? "On/" : "Off/";
    out += ( config.get_flags() & 0x2 ) > 0 ? "On/" : "Off/";
    out += ( config.get_flags() & 0x4 ) > 0 ? "On/" : "Off/";
    System.out.println(out); 

    System.out.print("> Setting up motes for problem " + config.get_problem_idx() + " ... ");
    
    SetupMsgT smsg = new SetupMsgT();
    
    smsg.set_type((short)0);
    smsg.set_config_problem_idx(config.get_problem_idx());
    smsg.set_config_runtime_msec(config.get_runtime_msec());
/*    cmsg.set_config_timer_msec( (trproblem[pidx] != 0) ? config.get_timer_msec() : 0);
    cmsg.set_config_lastchance_msec(config.get_lastchance_msec());
    cmsg.set_config_lplwakeupintval(config.get_lplwakeupintval());*/
    smsg.set_config_flags(config.get_flags());
    
    // SYNC-request control message
    CtrlMsgT cmsg = new CtrlMsgT();
    cmsg.set_type((short)5);
    
    lock.lock();

   	try {
			mif.send(MoteIF.TOS_BCAST_ADDR,smsg);
      Thread.sleep((int)(500));
      
      for ( currentMote = 0; currentMote < motecount[pidx] ; ++currentMote ) {
        handshake=false;
        // send CTRL_SETUP_SYN and wait for RESP_SETUP_ACK at most MAXTIMEOUT secs at most MAXPROBES times
        for( short probe = 0; !handshake && probe < MAXPROBES ; ++probe ) {
          mif.send(currentMote+1,cmsg);
          answered.await(MAXTIMEOUT,TimeUnit.MILLISECONDS);
        }
        if ( !handshake ) {
          System.out.println("\nTIMEOUT, MOTE : " + (currentMote+1) + ", quitting.");
          return false;
        }
      }
      System.out.println("OK");
      return true;
		} catch(IOException e) {
		  System.out.println("FAIL");
    } catch ( InterruptedException e ) {
      System.err.println("FAIL");
    } finally {
      lock.unlock();
    }
    return true;
	}

	public boolean run(final int fullruntime)
	{
    // We are now on running stage
    stage = 1;
    System.out.print("> Running testcase ... ");
    CtrlMsgT cmsg = new CtrlMsgT();
    // refer to RadioTest.h CTRL_START
    cmsg.set_type((short)10);
		try {
			mif.send(MoteIF.TOS_BCAST_ADDR,cmsg);
      // wait for test completion + 100 msecs
      Thread.sleep((int)(fullruntime + 100));
      System.out.println("OK");
      return true;
		} catch(IOException e) {
		  System.out.println("FAIL");
      return false;
    } catch ( InterruptedException e ) {
      System.err.println("FAIL");
      return false;
    }
	}

  public boolean collect()
	{
    // We are now on downloading stage
    stage = 2;  
    stats = new Vector<StatT>();
 
    debuglines = new long[motecount[pidx]];
    /*finalMsgIds = new long[edgecount[pidx]][2];
    profiles = new long[motecount[pidx]][3];
*/
    System.out.print("> Downloading data ... "); 

    lock.lock();
    for ( currentMote = 0; currentMote < motecount[pidx] ; ++currentMote ) {
      for ( currentData = 0; currentData < edgecount[pidx]; ++currentData ) {

        stats.add(new StatT());
 
        // Download stat - refer to RadioTest.h CTRL_DATA_REQ
        if ( !requestData((short)(30),"STAT") )
          return false;
          
        
      }
      
      // Download debug info - refer to RadioTest.h CTRL_DBG_REQ
      if ( !requestData((short)(40), "DEBUG") )
        return false;
      
      // Download profile info - refer to RadioTest.h CTRL_PROFILE_REQ
      //if ( !requestData((short)(50),"PROFILE") )
      //  return false;
        
    }
    System.out.println("OK");
    lock.unlock();
    return true;
	}

  private boolean requestData(final short type, final String str) {

    CtrlMsgT cmsg = new CtrlMsgT();
    cmsg.set_type(type);
    cmsg.set_data_req_idx(currentData);
    handshake = false;
    for( short probe = 0; !handshake && probe < MAXPROBES; ++probe ) {
      try {
 	  	  mif.send(currentMote+1,cmsg);
 	  	  answered.await(MAXTIMEOUT,TimeUnit.MILLISECONDS);
 	    } catch(IOException e) {
        return false;
      } catch ( InterruptedException e ) {
        return false;
      }
    } 
    if ( !handshake )
      System.out.println("TIMEOUT, MOTE : " + (currentMote+1) + " " + str + " : " + currentData);
    return handshake;
    
  }
  
  public void messageReceived(int dest_addr,Message msg)
	{
	  lock.lock();
    if ( msg instanceof SyncMsgT ) {
      SyncMsgT smsg = (SyncMsgT)msg;
      // SYNC_SETUP_ACK
      if ( smsg.get_type() == 6 && stage == 0 ) {
        handshake = true;
        answered.signal();
      }
      
    } else if ( msg instanceof DataMsgT ) {
    
      DataMsgT rmsg = (DataMsgT)msg;

      // refer to RadioTest.h RESP_DATA_OK
      if ( rmsg.get_type() == 31 && stage == 2  && currentData == rmsg.get_data_idx() ) {

        // Save stat
        StatT s = stats.get(currentData);
        s.set_triggerCount(         s.get_triggerCount()          +   rmsg.get_payload_stat_triggerCount());
        s.set_backlogCount(         s.get_backlogCount()          +   rmsg.get_payload_stat_backlogCount());
        s.set_resendCount(          s.get_resendCount()           +   rmsg.get_payload_stat_resendCount());
        s.set_sendCount(            s.get_sendCount()             +   rmsg.get_payload_stat_sendCount());
        s.set_sendSuccessCount(     s.get_sendSuccessCount()      +   rmsg.get_payload_stat_sendSuccessCount());
        s.set_sendFailCount(        s.get_sendFailCount()         +   rmsg.get_payload_stat_sendFailCount());

        s.set_sendDoneCount(        s.get_sendDoneCount()         +   rmsg.get_payload_stat_sendDoneCount());
        s.set_sendDoneSuccessCount( s.get_sendDoneSuccessCount()  +   rmsg.get_payload_stat_sendDoneSuccessCount());
        s.set_sendDoneFailCount(    s.get_sendDoneFailCount()     +   rmsg.get_payload_stat_sendDoneFailCount());

        s.set_wasAckedCount(        s.get_wasAckedCount()         +   rmsg.get_payload_stat_wasAckedCount());
        s.set_notAckedCount(        s.get_notAckedCount()         +   rmsg.get_payload_stat_notAckedCount());

        s.set_receiveCount(         s.get_receiveCount()          +   rmsg.get_payload_stat_receiveCount());
        s.set_expectedCount(        s.get_expectedCount()         +   rmsg.get_payload_stat_expectedCount());
        s.set_wrongCount(           s.get_wrongCount()            +   rmsg.get_payload_stat_wrongCount());
        s.set_duplicateCount(       s.get_duplicateCount()        +   rmsg.get_payload_stat_duplicateCount());
        s.set_missedCount(          s.get_missedCount()           +   rmsg.get_payload_stat_missedCount());
        s.set_forwardCount(         s.get_forwardCount()          +   rmsg.get_payload_stat_forwardCount());

        s.set_remainedCount((short)( s.get_remainedCount()        +   rmsg.get_payload_stat_remainedCount()));
        stats.set(currentData,s);

        handshake = true;
        answered.signal();
  
      // refer to RadioTest.h RESP_DBG_OK
      } else if ( rmsg.get_type() == 41 && stage == 2 ) {

        debuglines[currentMote] = rmsg.get_payload_debug();

        handshake = true;
        answered.signal();
        
      // refer to RadioTest.h RESP_PROFILE_OK
      }
    }
    lock.unlock();
	}

  private String statAsString(final short idx) {
    StatT s = stats.get(idx);
    String ret = " Stat(" + idx + "): ";

    ret += " " + s.get_triggerCount();
    ret += " " + s.get_backlogCount();
    ret += " " + s.get_resendCount();

    ret += " | " + s.get_sendCount();
    ret += " " + s.get_sendSuccessCount();
    ret += " " + s.get_sendFailCount();

    ret += " | " + s.get_sendDoneCount();
    ret += " " + s.get_sendDoneSuccessCount();
    ret += " " + s.get_sendDoneFailCount();
      
    ret += " | " + s.get_wasAckedCount();
    ret += " " + s.get_notAckedCount();

    ret += " | " + s.get_receiveCount();
    ret += " " + s.get_expectedCount();
    ret += " " + s.get_wrongCount();
    ret += " " + s.get_duplicateCount();
    ret += " " + s.get_forwardCount();
    ret += " " + s.get_missedCount();

    ret += " | " + s.get_remainedCount();
    ret += " | ";

    return ret;
  }

  private String statAsXml(final short idx) {
    StatT s = stats.get(idx);
    String ret = "<stat idx=\"" + idx + "\">";

    ret += "<TC>" + s.get_triggerCount() + "</TC>";
    ret += "<BC>" + s.get_backlogCount() + "</BC>";
    ret += "<RC>" + s.get_resendCount() + "</RC>";

    ret += "<SC>" + s.get_sendCount() + "</SC>";
    ret += "<SSC>" + s.get_sendSuccessCount() + "</SSC>";
    ret += "<SFC>" + s.get_sendFailCount() + "</SFC>";

    ret += "<SDC>" + s.get_sendDoneCount() + "</SDC>";
    ret += "<SDSC>" + s.get_sendDoneSuccessCount() + "</SDSC>";
    ret += "<SDFC>" + s.get_sendDoneFailCount() + "</SDFC>";

    ret += "<WAC>" + s.get_wasAckedCount() + "</WAC>";
    ret += "<NAC>" + s.get_notAckedCount() + "</NAC>";

    ret += "<RCC>" + s.get_receiveCount() + "</RCC>";
    ret += "<EXC>" + s.get_expectedCount() + "</EXC>";
    ret += "<WC>" + s.get_wrongCount() + "</WC>";
    ret += "<FC>" + s.get_forwardCount() + "</FC>";
    ret += "<DRC>" + s.get_duplicateCount() + "</DRC>";
    ret += "<MC>" + s.get_missedCount() + "</MC>";

    ret += "<REMC>" + s.get_remainedCount() + "</REMC>";
    ret += "</stat>";
    return ret;
  }

 /* private String finalMsgIdsAsString(final int idx) {
    String ret = "Final MsgIds : ";
    ret += " " + finalMsgIds[idx][0];
    ret += " " + finalMsgIds[idx][1];
    return ret;
  }

  private String finalMsgIdsAsXml(final int idx) {
    String ret = "<finaledgestate idx=\"" + idx + "\">";
    ret += "<SNM>" + finalMsgIds[idx][0] + "</SNM>";
    ret += "<RNM>" + finalMsgIds[idx][1] + "</RNM>";
    ret += "</finaledgestate>";
    return ret;
  }
  
  private String profileAsString(final int idx) {
    String ret = " Profile " + idx + ": ";
    ret += " MaxAtomic " + profiles[idx][0];
    ret += " MaxInterrupt " + profiles[idx][1];
    ret += " Maxlatency " + profiles[idx][2];    
    return ret;
  }

  private String profileAsXml(final int idx) {
    String ret = "<profiles idx=\"" + idx + "\">";
    ret += "<MAL>" + profiles[idx][0] + "</MAL>";
    ret += "<MIL>" + profiles[idx][1] + "</MIL>";
    ret += "<MTL>" + profiles[idx][2] + "</MTL>";    
    ret += "</profile>";
    return ret;
  }*/

  public void printResults(final SetupT config, final String output) {
    if ( output == "" ) {

      System.out.println();

      // Dump out the stats
      for( currentData = 0; currentData < edgecount[pidx]; ++currentData ) {
        System.out.print( statAsString(currentData) );
        //System.out.println( finalMsgIdsAsString(currentData) );
      }
      System.out.println();


      // Dump out debug info
      System.out.print(" Mote DebugLines (1-" + motecount[pidx] + "): " );
      for( currentMote = 0; currentMote < motecount[pidx]; ++currentMote ) {
        System.out.print( debuglines[currentMote] + " " );
      }      
      System.out.println();
      /*
      // Dump out profile info
      for( currentMote = 0; currentMote < motecount[pidx]; ++currentMote ) {
        System.out.println( profileAsString(currentMote) );
      }      
      System.out.println();
*/
      System.out.println("> Test completed.");

  /*  } else {
      try {
        PrintStream pstream = new PrintStream(new FileOutputStream(output,true));
        // Dump out the test's configuration
        pstream.println("<testresult>");
        pstream.println("  <testcase>");
        pstream.println("    <idx>" + pidx + "</idx>");
        pstream.println("    <runtime>" + config.get_runtime_msec() + "</runtime>");
        pstream.println("    <trigger>" + ((trproblem[pidx] != 0) ? config.get_timer_msec() : 0) + "</trigger>");
        pstream.println("    <lastchance>" + config.get_lastchance_msec() + "</lastchance>");
        pstream.println("    <ack>" + (((config.get_flags() & 0x1) > 0 )? "On" : "Off") + "</ack>");
        pstream.println("    <daddr>" + (((config.get_flags() & 0x2) > 0 ) ? "On" : "Off") + "</daddr>");
        pstream.println("    <lpl>" + ((config.get_lplwakeupintval() > 0 ) ? "On" : "Off") + "</lpl>");
        pstream.println("    <lplintval>" + config.get_lplwakeupintval() + "</lplintval>");
        pstream.println("  </testcase>");

        // Dump out statistics
        pstream.println("  <statlist>");
        for( currentData = 0; currentData < edgecount[pidx]; ++currentData )
          pstream.println( "    " + statAsXml(currentData) );
        pstream.println("  </statlist>");      

        // Dump out last msg ids
        pstream.println("  <finallist>");
        for( currentData = 0; currentData < edgecount[pidx]; ++currentData )
          pstream.println( "    " + finalMsgIdsAsXml(currentData) );
        pstream.println("  </finallist>");      
        pstream.println("  <debuglist>");
        for( currentMote = 0; currentMote < motecount[pidx]; ++currentMote ) 
          pstream.println( "    <debug idx=\"" + (currentMote+1) + "\">" + debuglines[currentMote] + "</debug>" );
        pstream.println("  </debuglist>");
        
        pstream.println("  <profilelist>");
        for( currentMote = 0; currentMote < motecount[pidx]; ++currentMote ) 
          pstream.println( "    " + profileAsXml(currentMote) );
        pstream.println("  </profilelist>");
        
        pstream.println("</testresult>");
        pstream.close();
      } catch( FileNotFoundException e ) {
        System.err.println("Invalid XML output file specified!");
      }*/
    }
  }

}
