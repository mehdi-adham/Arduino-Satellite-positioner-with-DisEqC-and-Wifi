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

    .formdiv {
        text-align: left;
        border: 1px solid #ccc;
        border-radius: 12px;
        padding: 12px;
        margin-top: 24px;
        width: 30%;
    }

    @media screen and (max-width:768px) {

        .formdiv {
            width: 55%;
        }
    }

    @media screen and (max-width:500px) {
        .formdiv {
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
                <div class="formdiv">
                    <h2>Add Network </h2>

                    <b> Network name:</b> <BR>
                    <input id="netname" type="text" name="netname" placeholder="Enter network name" maxlength="14"
                        minlength="4"><BR>

                    <b> password:</b> <BR>
                    <input id="netpassword" type="password" name="netpassword" placeholder="Enter password"
                        maxlength="14" minlength="8"><BR>
                    <div id="netmsg"></div>
                    <button id="netbtn" style="margin-top:24px;" type="button" class="button">Connect</button><BR>
                </div>

                <div class="formdiv">
                    <h2> Access Point </h2>

                    <b> ssid:</b> <BR>
                    <input type="text" name="ssid" placeholder="Positioner" maxlength="14" minlength="4"><BR>
                    <b> psk:</b> <BR>
                    <input type="password" name="psk" placeholder="12345678" maxlength="14" minlength="8"><BR>
                    <button style="margin-top:24px;" type="submit" class="button">Change</button><BR>

                </div>

                <div class="formdiv">
                    <h2>Login parameter</h2>
                    <b> username:</b> <BR>
                    <input type="text" name="username" placeholder="admin" maxlength="14" minlength="4"><BR>
                    <b> password:</b> <BR>
                    <input type="password" name="password" placeholder="admin" maxlength="14" minlength="4"><BR>
                    <button style="margin-top:24px;" type="submit" class="button">Change</button><BR>

                </div>
            </div>
            <br>
        </center>
    </form>
</body>
<script>
    const Enetname = document.getElementById("netname");
    const Enetpassword = document.getElementById("netpassword");
    const Enetmsg = document.getElementById("netmsg");
    const Enetbtn = document.getElementById("netbtn");

    Enetbtn.addEventListener("click", F_netbtn);

    function F_netbtn() {
        var xhttp = new XMLHttpRequest();
        xhttp.timeout = 30000;
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                const data = xhttp.responseText;
                if (data == "1") {
                    Enetmsg.innerText = "Successfull! Now, Using your phone, connect to new network. then Using your phone browser, enter ip address URL: 192.168.1.184 ";
                    Enetmsg.style.color = "Green";
                }
                else if (data == "0") {
                    Enetmsg.innerText = "Not find! Try again.";
                    Enetmsg.style.color = "Red";
                } else {
                    Enetmsg.innerText = "Invalid input, Try again.";
                    Enetmsg.style.color = "Red";
                }
            }
        };
        Enetmsg.innerText ="wait for connection...";
        Enetmsg.style.color = "gray";
        xhttp.open("GET", "F_addnet?netname=" + Enetname.value + "&netpassword=" + Enetpassword.value, true);
        xhttp.send();
    }
</script>

</html>
)=====";

#endif