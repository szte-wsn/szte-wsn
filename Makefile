COMPONENT=BenchmarkAppC

# use low power listening
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

include Makefile.Mig
include $(MAKERULES)