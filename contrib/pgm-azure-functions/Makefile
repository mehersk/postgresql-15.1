MODULE_big = pgm-azure-functions

EXTENSION = pgm-azure-functions
DATA = pgm-azure-functions--1.0.sql
PGFILEDESC = "pgm-azure-functions - PGMahouts azure functions extension"
SHLIB_LINK += -lcurl
OBJS = pgm-azure-functions.o

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/pgm-azure-functions
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
LDFLAGS = -lcurl
SHLIB_LINK = -lcurl
endif
