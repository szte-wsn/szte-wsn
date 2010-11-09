#include "Internal.h"

interface BenchmarkCore {
  
  /**
   * Requests a statistics indexed by 'index'
   * @return the stat
   */
  command stat_t* getStat(uint16_t index);
  
  /**
   * Requests the debug information
   * @return the line where the first assertion failed
   */
  command uint16_t getDebug();
  
  /** Configures the benchmark core with 'config' */
  command void setup(setup_t config);
  
  /** Indicates the successfull configuration of the benchmark */
  event void setupDone();
  
  /** Resets the benchmarking core component */
  command void reset();
  /** Indicates the finish of the reset operation */
  event void resetDone();
  
  /** Indicates the finish of the benchmark */
  event void finished();
  
  
}
