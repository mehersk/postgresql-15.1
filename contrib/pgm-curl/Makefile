MODULE_big = pgm-curl

EXTENSION = pgm-curl
DATA = pgm-curl--1.0.sql
PGFILEDESC = "pgm-curl - PGMahouts curl example"
SHLIB_LINK += -lcurl
OBJS = pgm-curl.o

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/pgm-curl
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
LDFLAGS = -lcurl
SHLIB_LINK = -lcurl
endif
