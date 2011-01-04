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
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.yaml.snakeyaml.Yaml;

public class BenchmarkBatch {

  private final String S_CONFIG           = "config";
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

  public BenchmarkBatch(String configfile, String outputfile) {
    try {
      for (Object bmark : new Yaml().loadAll(new FileInputStream(new File(configfile)))) {
        Map<String, Object> bdata = (Map<String, Object>) bmark;

        // Get the main configuration
        Map<String, Integer> bconfig =
                (Map<String, Integer>) bdata.get(S_CONFIG);



        // Get the timer configurations
        Map<String, List<Integer>> btimers =
                (Map<String, List<Integer>>) bdata.get(S_TIMERS);
        // Get the communication mode options
        List<String> bopts =
                (List<String>) bdata.get(S_CMODE);
        // Get the MAC parameter
        Integer bmacparam = (Integer) bdata.get(S_MAC);

        System.out.print(bconfig.get(S_CONFIG_MOTEC) + " ");
        System.out.print(btimers.get(S_TIMER_PREFIX + "1"));
        System.out.print(bopts.contains("ack"));
        System.out.println(bmacparam);
      }
    } catch (FileNotFoundException ex) {
      System.out.println("File named " + configfile + " not found!");
    } catch (Exception e) {
      System.out.println("Check your configuration file's format!");
    }
  }

  public void run() {
    
  }

}
