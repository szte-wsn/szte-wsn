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

import org.apache.commons.cli.*;
import java.util.regex.Pattern;
import java.util.regex.Matcher;
import java.io.PrintStream;

public class Benchmark {

  private static int def_runtimes[] = { 0, 1000, 20 };
  private static int def_timers[] =   { 1, 20,100 };

  public static Options opt;

  public static void printHelp() {
    
    HelpFormatter f = new HelpFormatter();
    System.out.println("Usage scenarios:");

    Options opt0 = new Options();
    opt0.addOption(opt.getOption("h"));
    System.out.println();
    System.out.println("1. Print help information.");
    System.out.println("--------------------------------------------------------------------");
    f.printHelp("Benchmark", opt0, true);

    // Batch - usage
    Options opt1 = new Options();
    opt1.addOption(opt.getOption("F"));
    opt1.addOption(opt.getOption("o"));    
    System.out.println();
    System.out.println("2. Running benchmarks with pre-defined configurations in batch mode.");
    System.out.println("--------------------------------------------------------------------");
    f.printHelp("Benchmark", opt1, true);
    
    // Reset - usage
    Options opt2 = new Options();
    opt2.addOption(opt.getOption("r"));
    System.out.println();
    System.out.println("3. Reset all motes.");
    System.out.println("--------------------------------------------------------------------");
    f.printHelp("Benchmark", opt2, true);
    
    // Download - usage
    Options opt4 = new Options();
    opt4.addOption(opt.getOption("dload"));
    System.out.println();
    System.out.println("4. Only download data from the motes.");
    System.out.println("--------------------------------------------------------------------");
    f.printHelp("Benchmark", opt4, true);
    
    // Command-line usage
    Options opt3 = new Options();
    opt3.addOption(opt.getOption("b"));
    opt3.addOption(opt.getOption("t"));
    opt3.addOption(opt.getOption("rs"));
    opt3.addOption(opt.getOption("lc"));
    opt3.addOption(opt.getOption("tr"));
    opt3.addOption(opt.getOption("ack"));
    opt3.addOption(opt.getOption("bcast"));
    opt3.addOption(opt.getOption("xml"));
    opt3.addOption(opt.getOption("mac"));
    System.out.println();
    System.out.println("5. Running a specific benchmark with command-line arguments");
    System.out.println("--------------------------------------------------------------------");
    f.printHelp(150, "Benchmark", "", opt3, "", true);
    
  }

	public static void main (String[] args)
	{
    opt = new Options();
    try {
  
      // Batch related options
      Option batchfile = OptionBuilder.withArgName( "file" )
                                .hasArg()
                                .withDescription( "The batch file with configuration parameters for multiple benchmark runs" )
                                .create( "F" );
                                
      Option batchoutput = OptionBuilder.withArgName( "file" )
                                .hasArg()
                                .withDescription( "The output XML file name. [default: results.xml]" )
                                .create( "o" ); 
                                
                                
                           
      // Problem id option
      Option problem = OptionBuilder.withArgName( "number" )
                                .hasArg()
                                .withDescription( "The benchmark to be used" )
                                .withLongOpt("benchmark")
                                .create( "b" );

      // Time- related options
      Option randomstart = OptionBuilder.withArgName( "number" )
                                .hasArg()
                                .withDescription( "Random start delay in millisecs. [default: " + def_runtimes[0] + " msec]" )
                                .withLongOpt("randomstart")
                                .create( "rs" );
      
      Option runtime = OptionBuilder.withArgName( "normal" )
                                .hasArg()
                                .withDescription( "The benchmark running time in millisecs. [default: " + def_runtimes[1] + " msec]" )
                                .withLongOpt("time")
                                .create( "t" );

      Option lastchance = OptionBuilder.withArgName( "number" )
                                .hasArg()
                                .withDescription( "The grace time period after test completion for last-chance reception. [default : " + def_runtimes[2] + " msec]" )
                                .withLongOpt("lastchance")
                                .create( "lc" );
                                                                
      // Timer-related option
      String deftimer = def_timers[0] + "," + def_timers[1] + "," + def_timers[2];
      Option trtimers = OptionBuilder.withArgName( "timer config list" )
                                .hasArg()
                                .withDescription( "Trigger timer configuration index:isoneshot,maxrandomdelay,period.  [default : 1:"+ deftimer + " ]" )
                                .withLongOpt("triggers")
                                .create( "tr" );

                          
      Option xml = OptionBuilder.withArgName( "file" )
                                .hasArg()
                                .withDescription( "Produce xml output" )
                                .create( "xml" );                          

     Option mac = OptionBuilder.withArgName( "MAC params" )
                                .hasArg()
                                .withDescription( BenchmarkStatic.MAX_MAC_PARAMS + " comma separated, 32bit parameters for MAC protocol")
                                .create( "mac" );
                          
      opt.addOption("h", "help", false, "Print help for this application");
      opt.addOption("r", "reset", false, "Reset all motes");

      opt.addOption(problem);
      opt.addOption(randomstart);
      opt.addOption(runtime);
      opt.addOption(lastchance);
      opt.addOption(xml);
      opt.addOption(mac);
        
      opt.addOption(trtimers);
      opt.addOption("ack", false, "Force acknowledgements. [default : false]");
      opt.addOption("bcast", "broadcast", false, "Force broadcasting. [default : false]");
      opt.addOption("dload", "download", false, "Only download data from motes.");
      
      opt.addOption(batchfile);
      opt.addOption(batchoutput);

      BasicParser parser = new BasicParser();
      CommandLine cl = parser.parse(opt, args);

      if ( cl.hasOption('h') ) {
        printHelp();
      } else if ( cl. hasOption('r') ) {
      
        // Reset the motes
        BenchmarkController rbr = new BenchmarkController();
        rbr.reset();
      } else if ( cl.hasOption("dload") ) {
      
        // Download the data
        BenchmarkController rbr = new BenchmarkController();
        if ( rbr.sync() && rbr.download() )
            rbr.printResults(System.out,false);

      } else if ( cl.hasOption('F') ) {
        String bfile = cl.getOptionValue('F');
        String ofile = cl.hasOption('o') ? cl.getOptionValue('o') : "results.xml";
        
        BenchmarkBatch rbb = new BenchmarkBatch(bfile,ofile);
        rbb.run();
     
      } else if ( cl.hasOption('b') ) {
        
        short problemidx = (short)Integer.parseInt(cl.getOptionValue('b'));
        if ( problemidx < 0 )
          throw new MissingOptionException("Invalid problem specified!");
  
        int runtimemsec = cl.hasOption('t') 
                                ? Integer.parseInt(cl.getOptionValue("t")) 
                                : def_runtimes[1];
        if ( runtimemsec <= 0 )
          throw new MissingOptionException("Invalid runtime specified!");
  
        int startdelay = cl.hasOption("rs") 
                                ? Integer.parseInt(cl.getOptionValue("rs")) 
                                : def_runtimes[0];
        if ( startdelay < 0 )
            throw new MissingOptionException("Invalid random start time specified!");
  
        int lchance = cl.hasOption("lc") 
                                ? Integer.parseInt(cl.getOptionValue("lc")) 
                                : def_runtimes[2];
        if ( lchance < 0 )
          throw new MissingOptionException("Invalid last chance time specified!");        

 
        // Trigger timer initialization values
        short ios[] =   new short[BenchmarkStatic.MAX_TIMER_COUNT];
        long delay[] =  new long[BenchmarkStatic.MAX_TIMER_COUNT];
        long period[] = new long[BenchmarkStatic.MAX_TIMER_COUNT];
        
        for( int i = 0; i < BenchmarkStatic.MAX_TIMER_COUNT; ++i ) {
          ios[i] = (byte)def_timers[0];
          delay[i] = def_timers[1];
          period[i] = def_timers[2];
        }
        
        if ( cl.hasOption("tr") ) {
          String[] triggers = cl.getOptionValues("tr");
          Pattern pattern = Pattern.compile("(\\d+):(\\d+),(\\d+),(\\d+)");

          for( int i = 0; i < triggers.length; ++i ) {
            Matcher matcher = pattern.matcher(triggers[i]);
            if (matcher.find()) {
              int trigidx = Integer.parseInt(matcher.group(1));
              if ( trigidx < 1 || trigidx > BenchmarkStatic.MAX_TIMER_COUNT)
                throw new MissingOptionException("Valid timer indexes are : [1.." + BenchmarkStatic.MAX_TIMER_COUNT + "], see help!");
              --trigidx;
              
              ios[trigidx] = (byte)Integer.parseInt(matcher.group(2));
              
              delay[trigidx] = Integer.parseInt(matcher.group(3));
              period[trigidx] = Integer.parseInt(matcher.group(4));
              
              if (period[trigidx] < 0 || delay[trigidx] < 0 || ios[trigidx] < 0 || ios[trigidx] > 1)
                throw new MissingOptionException("Trigger timer "+ (trigidx+1) +" is invalid, see help!");
              
              // at time 0, only one-shot timers are allowed to fire
              if (period[trigidx] == 0 && ios[trigidx] != 1)
                throw new MissingOptionException("Only one-shot timers are allowed with 0 ms period!");
              
            } else
              throw new MissingOptionException("Invalid trigger timer specification, see help!");
          }
          
        }
        
        short flags = 0;
        if ( cl.hasOption("ack") )
          flags |= BenchmarkStatic.GLOBAL_USE_ACK; 
        if ( cl.hasOption("bcast") )
          flags |= BenchmarkStatic.GLOBAL_USE_BCAST;
        if ( cl.hasOption("mac") )
          flags |= BenchmarkStatic.GLOBAL_USE_EXTERNAL_MAC;
        
        int macparams[] = new int[BenchmarkStatic.MAX_MAC_PARAMS];
        for( int i = 0; i < BenchmarkStatic.MAX_MAC_PARAMS; ++i ) {
          macparams[i] = 0;
        }
        
        if ( cl.hasOption("mac") ) {
          // Create pattern string
          String patternstr="";
          for (int i =0; i< BenchmarkStatic.MAX_MAC_PARAMS-1; ++i)
            patternstr += "(\\d+),";
          patternstr += "(\\d+)";

          // Create pattern
          Pattern pattern = Pattern.compile(patternstr);
          Matcher matcher = pattern.matcher(cl.getOptionValue("mac"));
          if ( matcher.find() ) {
            for( int i = 0; i < BenchmarkStatic.MAX_MAC_PARAMS; ++i ) {
              macparams[i] = Integer.parseInt(matcher.group(i+1));
            }
          } else
              throw new MissingOptionException("Invalid MAC params, see help!");
        }
        
        SetupT st = new SetupT();
        st.set_problem_idx(problemidx);
        st.set_pre_run_msec(startdelay);
        st.set_runtime_msec(runtimemsec);
        st.set_post_run_msec(lchance);
        st.set_flags(flags);
        st.set_timers_isoneshot(ios);
        st.set_timers_delay(delay);
        st.set_timers_period_msec(period);
        st.set_macparams(macparams);
        
        // Reset the motes
        BenchmarkController rbr = new BenchmarkController();
        
        PrintStream ps = cl.hasOption("xml") ? new PrintStream(cl.getOptionValue("xml")) : System.out;
        
        // Run the test
        if (  rbr.reset() && rbr.setup(st) && rbr.run() && rbr.download() ) {
          if (cl.hasOption("xml")) {
            ps.println(BenchmarkCommons.xmlHeader());
            rbr.printResults(ps,true);
            ps.println(BenchmarkCommons.xmlFooter());
          }
          else
            rbr.printResults(ps,false);
        } else {
          System.exit(1);
        } 
      
      } else {
        throw new MissingOptionException("Invalid program arguments, see help!");
      }
    } catch (Exception e) {
      System.err.println();
      System.err.println("Error : " + e.getMessage());
      System.err.println();
    } finally {
      System.exit(0);
    }
  }
}
