/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package benchmark.batch;

import benchmark.common.BenchmarkController;

/*import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.yaml.snakeyaml.Yaml;
*/
public class BenchmarkBatch {

/*  private final String S_CONFIG           = "config";
  private final String S_CONFIG_BMARK     = "bmark";
  private final String S_CONFIG_MOTEC     = "motes";
  private final String S_CONFIG_RANDSTART = "randstart";
  private final String S_CONFIG_TIME      = "time";
  private final String S_CONFIG_LC        = "lchance";
  private final String S_TIMERS           = "timers";
  private final String S_TIMER_PREFIX     = "t";
  private final String S_CMODE            = "force";
  private final String S_FACK             = "ack";
  private final String S_FBCAST           = "bcast";
  private final String S_MAC              = "wakeup";
*/
  public BenchmarkBatch(String configfile) {
/*    try {
     for (Object bmark : new Yaml().loadAll(new FileInputStream(new File(configfile))) ){
        Map<String, Object> bdata = (Map<String, Object>) bmark;

        // Get the main configuration
        Map<String,Integer>  bconfig = 
                (Map<String, Integer>) bdata.get(S_CONFIG);

        

        // Get the timer configurations
        Map<String,List<Integer> > btimers = 
                (Map<String, List<Integer> >) bdata.get(S_TIMERS);
        // Get the communication mode options
        List<String> bopts =
                (List<String>) bdata.get(S_CMODE);
        // Get the MAC parameter
        Integer bmacparam = (Integer) bdata.get(S_MAC);

        System.out.print(bconfig.get(S_CONFIG_MOTEC) + " ");
        System.out.print(btimers.get(S_TIMER_PREFIX+"1"));
        System.out.print(bopts.contains("ack"));
        System.out.println(bmacparam);
      }
    } catch (FileNotFoundException ex) {
      Logger.getLogger(BenchmarkBatch.class.getName()).log(Level.SEVERE, null, ex);
    } catch (Exception e) {
      System.out.println("Check your configuration file's format!");
    }*/
    System.out.println(configfile);
  }
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
      BenchmarkBatch m = new BenchmarkBatch("batch.yml");
      System.out.println("make a controller!");
      BenchmarkController rbr = new BenchmarkController((short)0);
    }

}
