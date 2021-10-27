#ifndef WIFICONFIGPAGE_H_
#define WIFICONFIGPAGE_H_

#include <Arduino.h>
#include <pgmspace.h>

const char wifiConfigPage[] PROGMEM =  R"=====(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>MOTOR SETTING</title>
</head>
<style type="text/css">
    body {
        color: #505050;
        font-family: sahel, Sahel-FD, 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
        font-size: 16px;
        line-height: 24px;
        font-weight: 300;
    }

    .button {
        width: 100%;
        background-color: #319fe9;
        border: none;
        color: white;
        padding: 15px 32px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 16px;
        border-radius: 5px;
        font-weight: 600;
        margin: 8px 0;
    }

    .button:hover {
        cursor: pointer;
    }

    .button:active {
        background: #57cc5d;
        border-width: 0;
    }

    .button:focus {
        background: #57cc5d;
        border-width: 0;
    }

    input[type=text],
    input[type=password] {
        width: 100%;
        padding: 12px 20px;
        margin: 8px 0;
        display: inline-block;
        border: 1px solid #ccc;
        border-radius: 4px;
        box-sizing: border-box;
    }

    #configform {
        width: 30%;
    }

    @media screen and (max-width:768px) {
        #configform {
            width: 55%;
        }
    }

    @media screen and (max-width:500px) {
        #configform {
            width: 90%;
        }
    }
</style>

<body style="background-color: #ffffff ">
    <form action="/config" method="post">
        <div>
            <a href="/">Home</a>
        </div>
        <center>
            <div>
                <div id="configform" style="text-align: left; border: 1px solid #ccc;border-radius: 12px; 
            padding: 12px; margin-top: 24px;">
                    <h2>Configuration </h2>
                        <hr>
                        <h3>Access Point Config</h3>
                        <b> ssid:</b> <BR>
                        <input type="text" name="ssid" placeholder="Positioner"><BR>
                        <b> psk:</b> <BR>
                        <input type="password" name="psk" placeholder="12345678"><BR>
                        <br>
                        <h3>Web Server Config</h3>
                        <b> username:</b> <BR>
                        <input type="text" name="username" placeholder="admin"><BR>
                        <b> password:</b> <BR>
                        <input type="password" name="password" placeholder="admin"><BR>
                        <button style="margin-top:24px;" type="submit" class="button">Change</button><BR>

                </div>
            </div>
            <br>
        </center>
    </form>
</body>

</html>
)=====";

#endif