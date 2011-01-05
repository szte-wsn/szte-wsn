/*
* Copyright (c) 2010, University of Szeged
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

package benchmark.common;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.yaml.snakeyaml.Yaml;

public class BenchmarkBatch {

  private List<SetupT>  setups;
  private List<Integer> motecounts;
  private String        outputfile;


  private static short def_runtimes[] = { 0, 1000, 20 };
  private static short def_timers[] =   { 1, 0, 100 };

  private final String S_CONFIG           = "config";
  private final String S_CONFIG_BMARK     = "bmark";
  private final String S_CONFIG_MOTEC     = "motes";
  private final String S_CONFIG_RANDSTART = "randstart";
  private final String S_CONFIG_TIME      = "time";
  private final String S_CONFIG_LC        = "lchance";
  private final String S_TIMERS           = "timers";
  private final String S_TIMER_PREFIX     = "t";
  private final String S_FORCES           = "forces";
  private final String S_FACK             = "ack";
  private final String S_FBCAST           = "bcast";
  private final String S_WAKEUP           = "wakeup";

  public BenchmarkBatch(String configfile, String outputfile) {
  
    try {
      this.outputfile = outputfile;
      this.setups = new ArrayList<SetupT>();
      this.motecounts = new ArrayList<Integer>();
      
      for (Object doc : new Yaml().loadAll(new FileInputStream(new File(configfile)))) {
        Map<String, Object> bmark = (Map<String, Object>) doc;
        if ( bmark == null )
          throw new WrongConfigSection();
        
        // config section
        Map<String, Integer> bconfig = null;
        if ( bmark.containsKey(S_CONFIG) ) {
          bconfig = (Map<String, Integer>) bmark.get(S_CONFIG);
          if ( ! (bconfig.containsKey(S_CONFIG_BMARK) && bconfig.containsKey(S_CONFIG_TIME) ) )
            throw new WrongConfigSection();
        }
        else
          throw new MissingConfigSection();

        // timers section
        List< Map<String,List<Integer>> > btimers = null;
        if ( bmark.containsKey(S_TIMERS) )
          btimers = (List< Map<String,List<Integer>> >) bmark.get(S_TIMERS);

        // forces section
        List<String> forceopts = null;
        if ( bmark.containsKey(S_FORCES) )
          forceopts = (List<String>) bmark.get(S_FORCES);

        // wakeup section
        Integer bwakeup =
                bmark.containsKey(S_WAKEUP)
                ? (Integer) bmark.get(S_WAKEUP)
                : (Integer)0;


        // Create a SetupT for the current benchmark
        SetupT setup = new SetupT();
        setup.set_problem_idx(bconfig.get(S_CONFIG_BMARK).shortValue());

        setup.set_pre_run_msec(
                bconfig.containsKey(S_CONFIG_RANDSTART) ?
                bconfig.get(S_CONFIG_RANDSTART) :
                def_runtimes[0]);

        setup.set_runtime_msec(bconfig.get(S_CONFIG_TIME).shortValue());
        setup.set_post_run_msec(
                bconfig.containsKey(S_CONFIG_LC) ?
                bconfig.get(S_CONFIG_LC) :
                def_runtimes[2]);

        setup.set_lplwakeup(bwakeup);

        short flags = 0;
        if ( forceopts != null ) {
          if ( forceopts.contains(S_FACK) )
            flags |= BenchmarkStatic.GLOBAL_USE_ACK;
          if ( forceopts.contains(S_FBCAST) )
            flags |= BenchmarkStatic.GLOBAL_USE_BCAST;
          if ( bmark.containsKey(S_WAKEUP) )
            flags |= BenchmarkStatic.GLOBAL_USE_EXTERNAL_MAC;
        }
        setup.set_flags(flags);

        // Trigger timer initialization values
        short ios[] =   new short[BenchmarkStatic.MAX_TIMER_COUNT];
        long delay[] =  new long[BenchmarkStatic.MAX_TIMER_COUNT];
        long period[] = new long[BenchmarkStatic.MAX_TIMER_COUNT];

        for( int i = 0; i < BenchmarkStatic.MAX_TIMER_COUNT; ++i ) {
          ios[i] = def_timers[0];
          delay[i] = def_timers[1];
          period[i] = def_timers[2];
        }

        if (btimers != null) {
          for (Map<String, List<Integer>> timerspec : btimers) {
            for (int i = 1; i <= BenchmarkStatic.MAX_TIMER_COUNT; ++i) {
              if (timerspec.containsKey(S_TIMER_PREFIX + i)) {
                List<Integer> timervalues = (List<Integer>) timerspec.get(S_TIMER_PREFIX + i);
                if (timervalues.size() != 3) {
                  throw new WrongTimerFormat();
                }

                ios[i-1] = timervalues.get(0).shortValue();
                delay[i-1] = timervalues.get(1).longValue();
                period[i-1] = timervalues.get(2).longValue();
              }
            }
          }
        }

        setup.set_timers_isoneshot(ios);
        setup.set_timers_delay(delay);
        setup.set_timers_period_msec(period);
        this.setups.add(setup);
        this.motecounts.add(
                bconfig.containsKey(S_CONFIG_MOTEC) ?
                bconfig.get(S_CONFIG_MOTEC) : 1);
      }
      
    } catch (FileNotFoundException ex) {
      System.out.println("File named " + configfile + " not found!");

    } catch (MissingConfigSection ex) {
      System.out.println("There is no " + S_CONFIG + " section in at least one benchmark description!");

    } catch (WrongConfigSection ex) {
      System.out.println("Each " + S_CONFIG + " section must contain the " +
              S_CONFIG_BMARK + " and " + S_CONFIG_TIME +" values!");
    } catch (WrongTimerFormat ex) {
      System.out.println("Each entry in " + S_TIMERS + " sections must contain exactly 3 integer values! (ex: - t1: [0,20,30] )");

    } catch (Exception e) {
      System.out.println("Check your configuration file's format!");
      System.out.println(e.getMessage());
    }
  }

  public void run() {
    PrintStream ps = null;
    try {
      ps = new PrintStream(this.outputfile);
      ps.println(BenchmarkCommons.xmlHeader());
      String newline = System.getProperty("line.separator");

      BenchmarkController rbr = new BenchmarkController(0);
      int i = 0;
      for (SetupT s : setups) {
        rbr.setMotecount(this.motecounts.get(i));
        String error = "";
        error += rbr.reset() ? "" : ("Reset error." + newline);
        error += rbr.setup(s) ? "" : ("Error during setup phase." + newline);
        error += rbr.run() ? "" : ("Error during running phase." + newline);
        error += rbr.download() ? "" : ("Error during downloading phase." + newline);
        error += rbr.download_debug() ? "" : ("Error during debug downloading phase." + newline);

        if ( error.isEmpty() )
          rbr.printResults(ps, true);
        else
          ps.print(BenchmarkCommons.xmlTestcaseError(error));
      }
      ps.println(BenchmarkCommons.xmlFooter());
      
    } catch (FileNotFoundException ex) {
      System.out.println("File named " + outputfile + " cannot be created!");
    } finally {
      ps.close();
    }
  }

}

class MissingConfigSection extends Exception {}
class WrongConfigSection extends Exception {}
class WrongTimerFormat extends Exception {}
