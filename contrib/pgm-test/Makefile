# contrib/pgm-test/Makefile

MODULE_big = pgm-test
OBJS = \
	$(WIN32RES) \
	pgm-test.o

EXTENSION = pgm-test
DATA = pgm-test--1.1.sql pgm-test--1.0--1.1.sql
PGFILEDESC = "pgm-test - UUID generation"

REGRESS = uuid_ossp

SHLIB_LINK += $(UUID_LIBS)

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/pgm-test
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
