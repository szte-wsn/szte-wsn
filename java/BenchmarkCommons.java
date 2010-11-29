
import java.util.Vector;

public class BenchmarkCommons {
  
  public static String xmlHeader() {
    return "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" +
           "<?xml-stylesheet type=\"text/xsl\" href=\"benchmark.xsl\"?>" +
           "<resultset>";
  }
  
  public static String xmlFooter() {
    return "</resultset>";
  }
  
  public static StatT getStatFromMessage(final DataMsgT rmsg) {
    StatT s = new StatT();
    s.set_triggerCount(rmsg.get_payload_stat_triggerCount());
    s.set_backlogCount(rmsg.get_payload_stat_backlogCount());
    s.set_resendCount(rmsg.get_payload_stat_resendCount());
    s.set_sendCount(rmsg.get_payload_stat_sendCount());
    s.set_sendSuccessCount(rmsg.get_payload_stat_sendSuccessCount());
    s.set_sendFailCount(rmsg.get_payload_stat_sendFailCount());

    s.set_sendDoneCount(rmsg.get_payload_stat_sendDoneCount());
    s.set_sendDoneSuccessCount(rmsg.get_payload_stat_sendDoneSuccessCount());
    s.set_sendDoneFailCount(rmsg.get_payload_stat_sendDoneFailCount());

    s.set_wasAckedCount(rmsg.get_payload_stat_wasAckedCount());
    s.set_notAckedCount(rmsg.get_payload_stat_notAckedCount());

    s.set_receiveCount(rmsg.get_payload_stat_receiveCount());
    s.set_expectedCount(rmsg.get_payload_stat_expectedCount());
    s.set_wrongCount(rmsg.get_payload_stat_wrongCount());
    s.set_duplicateCount(rmsg.get_payload_stat_duplicateCount());
    s.set_missedCount(rmsg.get_payload_stat_missedCount());
    s.set_forwardCount(rmsg.get_payload_stat_forwardCount());

    s.set_remainedCount(rmsg.get_payload_stat_remainedCount());
    return s;
  }
  
  
  public static StatT mergeStats(final StatT s1, final StatT s2) {
    StatT s = new StatT();
    
    s.set_triggerCount(         s1.get_triggerCount()          +   s2.get_triggerCount());
    s.set_backlogCount(         s1.get_backlogCount()          +   s2.get_backlogCount());
    s.set_resendCount(          s1.get_resendCount()           +   s2.get_resendCount());
    s.set_sendCount(            s1.get_sendCount()             +   s2.get_sendCount());
    s.set_sendSuccessCount(     s1.get_sendSuccessCount()      +   s2.get_sendSuccessCount());
    s.set_sendFailCount(        s1.get_sendFailCount()         +   s2.get_sendFailCount());

    s.set_sendDoneCount(        s1.get_sendDoneCount()         +   s2.get_sendDoneCount());
    s.set_sendDoneSuccessCount( s1.get_sendDoneSuccessCount()  +   s2.get_sendDoneSuccessCount());
    s.set_sendDoneFailCount(    s1.get_sendDoneFailCount()     +   s2.get_sendDoneFailCount());

    s.set_wasAckedCount(        s1.get_wasAckedCount()         +   s2.get_wasAckedCount());
    s.set_notAckedCount(        s1.get_notAckedCount()         +   s2.get_notAckedCount());

    s.set_receiveCount(         s1.get_receiveCount()          +   s2.get_receiveCount());
    s.set_expectedCount(        s1.get_expectedCount()         +   s2.get_expectedCount());
    s.set_wrongCount(           s1.get_wrongCount()            +   s2.get_wrongCount());
    s.set_duplicateCount(       s1.get_duplicateCount()        +   s2.get_duplicateCount());
    s.set_missedCount(          s1.get_missedCount()           +   s2.get_missedCount());
    s.set_forwardCount(         s1.get_forwardCount()          +   s2.get_forwardCount());

    s.set_remainedCount((short)(s1.get_remainedCount()         +   s2.get_remainedCount()));
    return s;
  }
  
  public static SetupMsgT createSetupMessage(final SetupT config) {
    SetupMsgT smsg = new SetupMsgT();
    
    smsg.set_config_problem_idx(config.get_problem_idx());
    smsg.set_config_pre_run_msec(config.get_pre_run_msec());
    smsg.set_config_runtime_msec(config.get_runtime_msec());
    smsg.set_config_post_run_msec(config.get_post_run_msec());
    smsg.set_config_flags(config.get_flags());
    
    smsg.set_config_timers_isoneshot(config.get_timers_isoneshot());
    smsg.set_config_timers_delay(config.get_timers_delay());
    smsg.set_config_timers_period_msec(config.get_timers_period_msec());
    
    smsg.set_config_macparams(config.get_macparams());
    
    return smsg;    
  }
  
  public static String setupAsString(final SetupT config) {
  
    String newline = System.getProperty("line.separator");
    String out = "> Problem: \t" + config.get_problem_idx() + newline;
    out += "  Runtime: \t[max (" + config.get_pre_run_msec() + "ms) + " 
           + config.get_runtime_msec() + "ms + " 
           + config.get_post_run_msec() + " ms]" + newline;
    out += "  Ack/Bcast: \t";
    out += ( config.get_flags() & 0x1 ) > 0 ? "On/" : "Off/";
    out += ( config.get_flags() & 0x2 ) > 0 ? "On" : "Off";
    out += newline;
    
    out += "  Timers: \t[";
    short ios[] = config.get_timers_isoneshot();
    long delay[] = config.get_timers_delay();
    long period[] = config.get_timers_period_msec(); 
               
    for (int i=0; i< BenchmarkStatic.MAX_TIMER_COUNT; ++i) {
      out += (ios[i] == 1 ) ? "1shot " : "period ";
      out += "max(" + delay[i] + "ms) ";
      out += period[i] + "ms";
      if (i != BenchmarkStatic.MAX_TIMER_COUNT-1)
        out += " | ";
    }
    out += "]";
    return out;
  }
  
  public static String setupAsXml(final SetupT config) {
    String out = "<configuration>";
    out +="    <benchidx>" + config.get_problem_idx() + "</benchidx>";
    out +="    <pre_runtime>" + config.get_pre_run_msec() + "</pre_runtime>";
    out +="    <runtime>" + config.get_runtime_msec() + "</runtime>";
    out +="    <post_runtime>" + config.get_post_run_msec() + "</post_runtime>";
    out +="    <ack>" + (((config.get_flags() & 0x1) > 0 )? "On" : "Off") + "</ack>";
    out +="    <bcast>" + (((config.get_flags() & 0x2) > 0 ) ? "On" : "Off") + "</bcast>";
  
    short ios[] = config.get_timers_isoneshot();
    long delay[] = config.get_timers_delay();
    long period[] = config.get_timers_period_msec();        
    for (int i=0; i< BenchmarkStatic.MAX_TIMER_COUNT; ++i) {
      out += "<timer idx=\"" + i + "\" ";
      out += (ios[0] == 0 )? "oneshot=\"yes\" " : "oneshot=\"no\" ";
      out += "delay=\"" + delay[i] + "\" ";
      out += "period=\"" + period[i] + "\"/>";
    }
    out += "</configuration>";
    return out;
  }

  public static String statsAsString(final Vector<StatT> stats) {
    String newline = System.getProperty("line.separator");
    String hdr = "  Stats:\t[ Tri Blg Res | send Succ Fail | sDone Succ Fail | Ack NAck | Recv  Exp Wrng Dupl Frwd Miss | Rem ]";
   
   	String ret = "";
    for ( int i = 0; i< stats.size(); ++i ) {
      StatT s = stats.get(i);
     
     	String str = String.format("  Stats(%1d):\t[ %2$3d %3$3d %4$3d | %5$4d %6$4d %7$4d | %8$5d %9$4d %10$4d | %11$3d %12$4d | %13$4d %14$4d %15$4d %16$4d %17$4d %18$4d | %19$3d ]",
     		i,
     		s.get_triggerCount(),
     		s.get_backlogCount(),
	     	s.get_resendCount(),
	     	s.get_sendCount(),
	     	s.get_sendSuccessCount(),
	     	s.get_sendFailCount(),
     		s.get_sendDoneCount(),
     		s.get_sendDoneSuccessCount(),
     		s.get_sendDoneFailCount(),
     		s.get_wasAckedCount(),
     		s.get_notAckedCount(),
     		s.get_receiveCount(),
      	s.get_expectedCount(),
      	s.get_wrongCount(),
      	s.get_duplicateCount(),
      	s.get_forwardCount(),
      	s.get_missedCount(),
      	s.get_remainedCount()
     	);
      ret += str + newline;
    }
    return hdr + newline + ret;
  }

  public static String statsAsXml(final Vector<StatT> stats) {
    String ret = "<statlist>";
    for ( int i = 0; i< stats.size(); ++i ) {
      StatT s = stats.get(i);
      ret += "<stat idx=\"" + i + "\">";

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
    }  
    ret += "</statlist>";
    return ret;
  }
  
  public static String debugAsString(final long[] debuglines) {
    String ret = "  Mote debug :\t";
    for( int i = 0; i < debuglines.length; ++i ){
      ret += debuglines[i] + " ";
    }
    return ret;
  }
  
  public static String debugAsXml(final long[] debuglines) {
    String ret = "<debuglist>";
    for( int i = 0; i < debuglines.length; ++i ){
      ret +=  "<debug idx=\"" + (i+1) + "\">" + debuglines[i] + "</debug>";
    }
    ret += "</debuglist>";
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
  
  
  
  

}
