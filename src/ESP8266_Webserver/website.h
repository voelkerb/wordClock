const char header[] = R"=====(HTTP/1.1 200 OK
Content-type:text/html
\r\n\r\n
)=====";

const char page[] = R"=====(
<!DOCTYPE html>
<html>
    <head>
        <title>Clocktwo Web Server</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <link rel="icon" href="data:,">
        <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css">
        <script src="https://cdnjs.cloudflare.com/ajax/libs/jscolor/2.0.4/jscolor.min.js"></script>
        <!-- Load from SPIFFS -->
        <!-- <link rel="stylesheet" type="text/css" href="style.css"> -->
        <style>
        html{
            height: 100%;
        }
        html, body {
            background: rgb(54, 54, 54);
        }
        body {
            width: 70%;
            margin: 0 auto;
            flex-direction: column;
            min-height: 100%;
            display: flex;
        }
        main {
            flex: 1 1 auto;     /* 1 */
            position: relative;
            min-width: 200px;
            max-width: 1000px;
            margin: 0 auto;
            z-index: 1;
            width: 100%;        /* 2 */
        }
        h1, form legend {
            font-family: 'Monaco', cursive;
            text-shadow: 2px 2px 6px black;
        }
        h1, form legend, footer, output {
            color: #d3d3d3;
            text-align: center;
        }
        h1 {
            font-size: 3rem;
        }
        form {
            margin: 24px 0 0 0;
        }
        form legend {
            font-size: 2em;
        }
        form fieldset {
            margin: 0 0 12px;
            padding: 8px;
            border: 3px solid #ccc;
        }
        form label {
            color: #d4d4d4;
            float: left;
            text-align: right;
            padding-right: 10px;
            width: 120px;
        }
        form label, form input {
            margin: 0px 0;
        }
        form input {
            width: 100%;
        }
        td {
            width: 100%;
            padding-bottom:10px;
        }
        form span {
            display: block;
            overflow: hidden;
            padding: 0 5px 0 0;
        }
        #submit {
            background-color: rgb(57, 149, 255);
            color: white;
        }
        footer {
            margin-top: 10px;
        }
        </style>
    </head>
    <body>

        <header>
            <h1>Clock Two Configuration</h1>
        </header>
        <main>
            <form action="/general">
                <fieldset>
                    <legend>General: </legend>
                    <label for="Name">Name:</label><span><input type="text" placeholder="$$NAME$$" name="Name"/></span><br />
                    <label for="Time">Time:</label><span><input type="datetime-local" name="Time" placeholder="2000-01-01T00:00:00" /></span><br />
                    <label for="TimeS">Time Server:</label><span><input type="text" name="TimeS"/ placeholder="$$TIMES$$"></span><br />
                    <label for="MQTT">MQTT Server:</label><span><input type="text" name="MQTT" placeholder="$$MQTT$$"/></span><br />
                    <input type="submit" name="submit" id="submit" href="#" value="Update"/>
                </fieldset>
            </form>
            <form action="/wifi">
                <fieldset>
                    <legend>Network Configuration</legend>
                    <label for="SSID">SSID:</label><span><input type="text" name="SSID" placeholder="$$SSID$$" required /></span><br />
                    <label for="PWD">PWD:</label><span><input type="password" name="PWD" minlength="8" required /></span><br />
                    <input type="submit" name="submit" id="submit" href="#" value="Update"/>
                </fieldset>
            </form>
            <form action="/color">
                <fieldset>
                    <legend>Color Configuration</legend>
                    <label for="fgColor">Color:</label><span><input name="fgColor" value="$$FGCOLOR$$" class="jscolor {onFineChange:'update(this)'}" id="fgColor"></span><br/>
                    <label for="bgColor">Back Color:</label><span><input name="bgColor" value="$$BGCOLOR$$" class="jscolor {onFineChange:'update(this)'}" id="bgColor"></span><br />
                    <label for="nightColor">Night Color:</label><span><input name="nightColor" value="$$NIGHTCOLOR$$" class="jscolor {onFineChange:'update(this)'}" id="nightColor"></span><br />
                    <label for="nightColor">Brightness:</label><span>
                    <table>
                        <tr>
                            <td>
                                <input name="brightness" id="brightness" type="range" value="$$BRIGHTNESS$$" min="0" max="100" onchange="document.getElementById('brightOut').value=this.value+'%'">
                            </td>
                            <td>
                            <output id="brightOut">50%</output></td>
                            </span>
                        </tr>
                    </table>
                    <td><input type="submit" name="submit" id="submit" href="#" value="Update"/>
                </fieldset>
            </form>
        </main>
        <footer>
            <p>&copy;Copyright 2022 Benjamin V&ouml;lker. All rights reversed.</p>
        </footer>
    </body>
</html>)=====";
