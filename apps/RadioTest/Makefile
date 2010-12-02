COMPONENT=BenchmarkAppC

# use CC2420X experimental radio driver ?
USE_CC2420X_EXPERIMENTAL=no

# use low power listening
PFLAGS += -I./mac
PFLAGS += -I./mac/null
#CFLAGS += -DLOW_POWER_LISTENING

# use 32-bit length statistics ? (default is 16 bits)
#CFLAGS += -DUSE_32_BITS
# the maximum edge count that is present in all benchmarks
CFLAGS += -DMAX_EDGE_COUNT=8
# exclude the standard benchmarks from the problemset
#CFLAGS += -DEXCLUDE_STANDARD
# exclude the user defined benchmarks from the problemset
#CFLAGS += -DEXCLUDE_USERDEFINED

CFLAGS += -DCC2420_DEF_CHANNEL=$(DEF_CHANNEL)
CFLAGS += -DRF230_DEF_CHANNEL=$(DEF_CHANNEL)

ifneq (,$(findstring USE_32_BITS,$(CFLAGS)))
  CFLAGS += -DTOSH_DATA_LENGTH=87
else
  CFLAGS += -DTOSH_DATA_LENGTH=67
endif

#CFLAGS += -I$(SZTETOSDIR)/lib/CodeProfile

include Makefile.Mig
include $(MAKERULES)
include Makefile.Platforms

ifdef RADIO_CC2420
	ifeq ($(USE_CC2420X_EXPERIMENTAL),yes)
		
		CFLAGS += -I$(SZTETOSDIR)/chips/cc2420x
		CFLAGS += -I$(SZTETOSDIR)/chips/cc2420x/util
		CFLAGS += -I$(SZTETOSDIR)/chips/cc2420x/layers
		CFLAGS += -I$(SZTETOSDIR)/platforms/telosa/chips/cc2420x
		CFLAGS += -I$(SZTETOSDIR)/platforms/telosa

		CFLAGS += -DRADIO_CC2420X
		CFLAGS += -DCC2420X_DEF_CHANNEL=$(DEF_CHANNEL)
		
	endif
endif
