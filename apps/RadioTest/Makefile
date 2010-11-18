COMPONENT=BenchmarkAppC

USE_CC2420X_EXPERIMENTAL=no

CFLAGS += -DCC2420_DEF_CHANNEL=$(DEF_CHANNEL)
CFLAGS += -DRF230_DEF_CHANNEL=$(DEF_CHANNEL)

CFLAGS += -DMAX_EDGE_COUNT=8

# in case of 16-bit length statistics
CFLAGS += -DTOSH_DATA_LENGTH=51

# in case of 32-bit length statistics
#CFLAGS += -DTOSH_DATA_LENGTH=71


#CFLAGS += -I$(SZTETOSDIR)/lib/CodeProfile
#CFLAGS += -DLOW_POWER_LISTENING

include Makemigrules
include $(MAKERULES)
include $(SZTETOSROOT)/support/make/Makeplatformopts

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
