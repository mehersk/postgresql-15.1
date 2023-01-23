MODULES = pgm-curl
EXTENSION = pgm-curl
DATA = pgm-curl--1.0.sql
PGFILEDESC = "pgm-curl - PGMahouts curl example"
# REGRESS = pgm-curl

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
CFLAGS = -I/var/lib/postresql-15.1
LDFLAGS = -lcurl
else
subdir = contrib/pgm-curl
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif