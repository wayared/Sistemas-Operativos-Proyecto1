<?php
// Include the bundled autoload from the Twilio PHP Helper Library
require __DIR__ . '/twilio-php-main/src/Twilio/autoload.php';
use Twilio\Rest\Client;
// Your Account SID and Auth Token from twilio.com/console
$account_sid = 'AC694df14ac55c139f993f7a094bd43fe9';
$auth_token = '3f72714f0e3d8524ccac5a083b4f5267';
// In production, these should be environment variables. E.g.:
// $auth_token = $_ENV["TWILIO_ACCOUNT_SID"]
// A Twilio number you own with SMS capabilities
$twilio_number = "+19566064753";
$client = new Client($account_sid, $auth_token);
$client->messages->create(
    // Where to send a text message (your cell phone?)
    //'+593967798691',
    '+593989884022',
    array(
        'from' => $twilio_number,
        'body' => 'Hola que hace, confirma ese AP en SO XD'
    )
);
