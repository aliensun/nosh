<?php

/*
 * Copyright 2011 SinglePlatform
 *
 * Takes the following commandline parameters for location search (order important):
 * - debug
 * - signing key
 * - client id
 * - query string
 *
 */

if( $argv[1] == "-h" || $argv[1] == "--help" || $argc < 5) {
    print("Usage: SP_SearchLocations.php DEBUG SIGNING_KEY CLIENT_ID QUERY_STRING

    DEBUG        Print debug information? Default: false, Valid values: true, false.
    SIGNING_KEY  Your signing key. Used to sign the api URLs.
    CLIENT_ID    Your client ID. Appended to the URLs.
    QUERY_STRING The query to search for. The query must be one argument. Thus,
                 quote your search query if there are spaces, special characters,
                 etc. in your query.

Note: All parameters are required.
");
    return;
}

require_once( "../lib/SP/SP_ApiLibrary.php" );

$debug = false;
if ('true' === $argv[1]) {
    $debug = true;
}
$signingKey = $argv[2];
$clientId = $argv[3];
$query = $argv[4];
$host = 'api.singleplatform.co';
$protocol = 'http';

$sp_api = new SP_ApiLibrary( $signingKey, $clientId, $host, $protocol, $debug );
$value = $sp_api->searchLocation( $query );
print( json_encode( $value, true ) . PHP_EOL );
