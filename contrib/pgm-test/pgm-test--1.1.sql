/* contrib/pgm-test/pgm-test--1.1.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use '''CREATE EXTENSION "pgm-test"''' to load this file. \quit

CREATE FUNCTION pgm_test_add(a integer, b integer) RETURNS integer
    LANGUAGE SQL
    IMMUTABLE
    RETURNS NULL ON NULL INPUT
    RETURN a + b;

-- CREATE FUNCTION pgm_test_curl()
-- RETURNS TEXT
-- AS '$libdir/pgm-test'
-- LANGUAGE C IMMUTABLE STRICT;