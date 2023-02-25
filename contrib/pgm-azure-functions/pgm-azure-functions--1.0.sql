\echo Use "CREATE EXTENSION pgm-azure-functions" to load this file. \quit

CREATE OR REPLACE FUNCTION create_schema_table_view_and_permissions()
RETURNS BOOLEAN AS $$
DECLARE
  result BOOLEAN;
BEGIN
  BEGIN;

  CREATE SCHEMA IF NOT EXISTS pgm;

  SET search_path TO pgm;
  CREATE TABLE IF NOT EXISTS azure_functions (
    uri             TEXT  NOT NULL,
    auth_token      TEXT  NOT NULL,
    function_alias  TEXT  NOT NULL,
    PRIMARY KEY(function_alias)
  );

  CREATE OR REPLACE VIEW v_azure_functions AS
  SELECT uri, function_alias FROM azure_functions;

  GRANT SELECT ON pgm.azure_functions TO PUBLIC;

  result := TRUE;
  RAISE NOTICE 'Values stored in pgm.azure_functions and pgm.azure_functions created with permissions granted';
  
  COMMIT;
  
  RETURN result;
EXCEPTION WHEN OTHERS THEN
  result := FALSE;
  ROLLBACK;
  RAISE WARNING 'Error creating schema, table, view and granting permissions: %', SQLERRM;
  RETURN result;
END;
$$ LANGUAGE plpgsql;

DO $$
BEGIN
  PERFORM create_schema_table_view_and_permissions();
END $$;

CREATE OR REPLACE FUNCTION add_azure_function(function_uri text, token text, alias text)
RETURNS BOOLEAN AS $$
DECLARE
  result BOOLEAN;
BEGIN
  BEGIN;
  INSERT INTO pgm.azure_functions (
    uri,
    auth_token,
    function_alias
  )
  VALUES (
    function_uri,
    token,
    alias
  );
  result := TRUE;
  RAISE NOTICE 'Added azure function % into azure_functions', alias;
  COMMIT;
  RETURN result;
EXCEPTION WHEN OTHERS THEN
  result := FALSE;
  ROLLBACK;
  RAISE WARNING 'Error adding azure function % into azure_functions: %', alias, SQLERRM;
  RETURN result;
END;
$$ LANGUAGE plpgsql;

CREATE FUNCTION pgm_curl_post(function_alias text, payload text) RETURNS text
AS 'MODULE_PATHNAME'
LANGUAGE C;
