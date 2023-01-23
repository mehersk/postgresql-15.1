\echo Use "CREATE EXTENSION pgm-curl" to load this file. \quit

CREATE FUNCTION pgm_curl_c(url text) RETURNS text
AS '$libdir/pgm-curl'
LANGUAGE C IMMUTABLE STRICT