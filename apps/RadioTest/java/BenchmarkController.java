/** Copyright (c) 2010, University of Szeged
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

import java.io.*;
import java.util.concurrent.locks.*;
import java.util.concurrent.TimeUnit;
import java.util.Vector;
import net.tinyos.message.*;
import net.tinyos.util.*;
import org.apache.commons.cli.*;
import java.util.Calendar;


public class BenchmarkController implements MessageListener {
	
	private MoteIF mif;
  private short stage;
  PrintStream ostream;

  // Memory for downloaded data
  private Vector< StatT >   stats;
  private long[]            debuglines;
  private SetupT            config;

  // Needed for proper downloading
  final Lock lock = new ReentrantLock();
  final Condition answered = lock.newCondition(); 
  private boolean handshake;
  
  private static int currentMote = 1, currentData = 0;
  private static int maxMoteId = 1, edgecount = 0;
  private long running_time;
    
  private static final short  MAXPROBES   = 3;
  private static final int    MAXTIMEOUT  = 500;

  public BenchmarkController(final int motecount)
	{
	  maxMoteId = motecount;
	  this.init(System.out);
	}
	
	public BenchmarkController(final int motecount, PrintStream stream)
	{
	  maxMoteId = motecount;
	  this.init(stream);
	}
	
	private void init(PrintStream stream) {
	  mif = new MoteIF();
    mif.registerListener(new SyncMsgT(),this);
    mif.registerListener(new DataMsgT(),this);
    
    stage = 0;
    ostream = stream;
	}

	public boolean reset()  
  {
    CtrlMsgT cmsg = new CtrlMsgT();
    cmsg.set_type(BenchmarkStatic.CTRL_RESET);
    ostream.print("> Reset all motes   ... ");
		try {
			mif.send(MoteIF.TOS_BCAST_ADDR,cmsg);
			Thread.sleep((int)(500));
      stage = 0;
      ostream.println("OK");
      return true;
		} catch(Exception e) {
		  ostream.println("FAIL");
      return false;
    }
	}

  public boolean setup(final SetupT conf) {
  
    this.config = conf;

    // Create an appropriate setup message
    SetupMsgT smsg = BenchmarkCommons.createSetupMessage(config);
    smsg.set_type(BenchmarkStatic.SETUP_BASE);
    
    running_time =  config.get_pre_run_msec() + 
                    config.get_runtime_msec() + 
                    config.get_post_run_msec();
    
    ostream.println(BenchmarkCommons.setupAsString(config));
    ostream.print("> Setting up motes  ... ");
   
    // SYNC-request control message
    CtrlMsgT cmsg = new CtrlMsgT();
    cmsg.set_type(BenchmarkStatic.CTRL_SETUP_SYN);
   
    lock.lock();
   	try {
			mif.send(MoteIF.TOS_BCAST_ADDR,smsg);
      Thread.sleep((int)(500));
      
      for ( currentMote = 1; currentMote <= maxMoteId ; ++currentMote ) {
        if ( !requestSync() )
          return false;
      }
      ostream.println("OK");
      return true;
		} catch(Exception e) {
		  return false;
    } finally {
      lock.unlock();
    }
	}

  public boolean run() { 

    CtrlMsgT cmsg = new CtrlMsgT();
    cmsg.set_type(BenchmarkStatic.CTRL_START);
    // We are now on running stage
    stage = 1;    

    ostream.print("> Running benchmark ... ");
		try {
			mif.send(MoteIF.TOS_BCAST_ADDR,cmsg);
      // wait for test completion + 100 msecs
      Thread.sleep((int)(running_time + 100));
      ostream.println("OK");
      return true;
		} catch(Exception e) {
		  ostream.println("FAIL");
      return false;
    }
  }

  public boolean download()
	{
    // We are now on downloading stage
    stage = 2;  
    stats = new Vector<StatT>(edgecount);
    for( int i = 0; i< edgecount; ++i)
      stats.add(new StatT());

    debuglines = new long[maxMoteId];
    ostream.print("> Downloading data  ... "); 

    for ( currentMote = 1; currentMote <= maxMoteId ; ++currentMote ) {
      for ( currentData = 0; currentData < edgecount; ++currentData ) {
        if ( !requestData(BenchmarkStatic.CTRL_STAT_REQ,"STAT") )
          return false;      
      }
      if ( !requestData(BenchmarkStatic.CTRL_DBG_REQ, "DEBUG") )
        return false;
    }
    ostream.println("OK");
    return true;
	}

  public boolean sync() {
    return this.requestSync();
  }

  private boolean requestSync() {

    lock.lock();
    CtrlMsgT cmsg = new CtrlMsgT();
    cmsg.set_type(BenchmarkStatic.CTRL_SETUP_SYN);
    handshake = false;
    for( short probe = 0; !handshake && probe < MAXPROBES; ++probe ) {
      try {
 	  	  mif.send(currentMote,cmsg);
 	  	  answered.await(MAXTIMEOUT,TimeUnit.MILLISECONDS);
 	    } catch(IOException e) {
        break;
      } catch ( InterruptedException e ) {
        break;
      }
    } 
    if ( !handshake )
      ostream.println("SYNC TIMEOUT, MOTE : " + currentMote);
    lock.unlock();
    return handshake;
  }

  private boolean requestData(final short type, final String str) {

    CtrlMsgT cmsg = new CtrlMsgT();
    cmsg.set_type(type);
    cmsg.set_data_req_idx((short)currentData);
    handshake = false;
    lock.lock();
    for( short probe = 0; !handshake && probe < MAXPROBES; ++probe ) {
      try {
 	  	  mif.send(currentMote,cmsg);
 	  	  answered.await(MAXTIMEOUT,TimeUnit.MILLISECONDS);
 	    } catch(IOException e) {
        break;
      } catch ( InterruptedException e ) {
        break;
      }
    } 
    if ( !handshake )
      ostream.println("TIMEOUT, MOTE : " + currentMote + " " + str + " : " + currentData);
    lock.unlock();
    return handshake;
    
  }
  
  public void messageReceived(int dest_addr,Message msg)
	{
	  lock.lock();
    if ( msg instanceof SyncMsgT ) {
      SyncMsgT smsg = (SyncMsgT)msg;
      if ( smsg.get_type() == BenchmarkStatic.SYNC_SETUP_ACK && stage == 0 ) {
        handshake = true;
        edgecount = smsg.get_edgecnt();
        if ( smsg.get_maxmoteid() > maxMoteId )
          maxMoteId = smsg.get_maxmoteid();
        answered.signal();
      }
     
    } else if ( msg instanceof DataMsgT ) {
    
      DataMsgT rmsg = (DataMsgT)msg;
      if ( rmsg.get_type() == BenchmarkStatic.DATA_STAT_OK && stage == 2  && 
           currentData == rmsg.get_data_idx() ) {
       
        stats.set(currentData,
                  BenchmarkCommons.mergeStats(
                    stats.get(currentData),
                    BenchmarkCommons.getStatFromMessage(rmsg)
                  )
                 );
        
        handshake = true;
        answered.signal();
  
      } else if ( rmsg.get_type() == BenchmarkStatic.DATA_DBG_OK && stage == 2 ) {

        debuglines[currentMote-1] = rmsg.get_payload_debug();
        handshake = true;
        answered.signal();
      }
    }
    lock.unlock();
	}

  public void printResults(PrintStream outstream, boolean asXml) {
    if ( asXml ) {
      Calendar calendar = Calendar.getInstance();

      outstream.println("<testresult date=\"" + calendar.getTime().toString() + "\">");
      outstream.println(BenchmarkCommons.setupAsXml(config));
      outstream.println(BenchmarkCommons.statsAsXml(stats));
      outstream.println(BenchmarkCommons.debugAsXml(debuglines));
      outstream.println("</testresult>");
      
    } else {
    
      outstream.println(BenchmarkCommons.statsAsString(stats));
      outstream.println(BenchmarkCommons.debugAsString(debuglines));
    }
  }

}
