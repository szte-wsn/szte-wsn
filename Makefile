COMPONENT=BenchmarkAppC

# MAC protocol usage
#CFLAGS += -DLOW_POWER_LISTENING
#CFLAGS += -DPACKET_LINK

# use 32-bit length statistics ? (default is 16 bits)
#CFLAGS += -DUSE_32_BITS

# the maximum edge count that is present in all benchmarks
CFLAGS += -DMAX_EDGE_COUNT=8

# exclude the standard benchmarks (or a part of it) from the problemset
# for low-memory platforms like telosa it might be necessary
#CFLAGS += -DEXCLUDE_STANDARD
#CFLAGS += -DEXCLUDE_STANDARD_THROUGHPUT
#CFLAGS += -DEXCLUDE_STANDARD_COLLISION
CFLAGS += -DEXCLUDE_STANDARD_FORWARDING

# exclude the user defined benchmarks from the problemset
CFLAGS += -DEXCLUDE_USERDEFINED

# codeprofile support
CFLAGS += -Icodeprofile

# channel settings
CFLAGS += -DCC2420_DEF_CHANNEL=$(DEF_CHANNEL)
CFLAGS += -DRF230_DEF_CHANNEL=$(DEF_CHANNEL)

ifneq (,$(findstring USE_32_BITS,$(CFLAGS)))
  CFLAGS += -DTOSH_DATA_LENGTH=97
else
  CFLAGS += -DTOSH_DATA_LENGTH=69
endif

include Makefile.Mig
include $(MAKERULES)
include Makefile.Pdetect