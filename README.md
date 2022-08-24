# GPS Implementation Internet of Things

# Introduction

The Internet of Things is the field of technology that deals with physical objects with computational capabilties, sensors and actuators that can connect to any form of network, including the internet, to exchange data with other devices, servers or the cloud.

In brief, the code in [TouchyThing.ino](https://github.com/abhisheknair10/GPS-Implementation-IoT/-/blob/main/LA2/ProjectThing/ProjectThing.ino) acts as a workout tracker that can trace the location, pace and timing of a run based workout. 

The aim of this assignment is to:
- Receive GPS signals from GPS satellites using a GPS module
- Decode and calculate the GPS coordinates of the microcontroller to high accuracy
- Connect to the Internet via WiFi or a Mobile Hotspot
- Send the GPS coordinates data to a server hosted on the cloud
- Serve workout data to a Run Analysis website

A demonstration of the working system is available [here](https://gitlab.com/anair11/com3505-student-2022/-/blob/main/LA2/Demonstration%20video.mp4).

# 1. GPS Module

## 1.1. Adafruit GPS FeatherWing
![Adafruit GPS Module](https://cdn-learn.adafruit.com/assets/assets/000/032/503/large1024/gps_3133_top_ORIG.jpg?1463520663)

The [Adafruit GPS FeatherWing](https://learn.adafruit.com/adafruit-ultimate-gps-featherwing) is a low power GPS based module that can receive GPS signals from GPS based satellites, using triangulation, to calculate GPS coordinates of the FeatherWing to a high accuracy while also keeping track with time once synced with overhead satellites.

Some technical details:
- Satellites: 22 tracking, 66 searching
- Update Rate: 1-10 Hz
- Position Accuracy: 1.8m
- Velocity Accuracy: 0.1m/s
- Output: 9600 BAUD Rate
- Vin Range: 3.0-5.5 VDC

More detailed documentation can be found [here](https://learn.adafruit.com/adafruit-ultimate-gps-featherwing).

## 1.2. Circuit Setup
The GPS module makes use of a 3V input and sends GPS and other data collected from satellites via the 'TX' (Transmission) port. 

The 3V supply from the ESP32 module is connected to the 3V input of the GPS module while the Ground (GND) port in the ESP32 module is connected to the Ground (GND) port of the GPS module. This ensures the GPS module is able to receive power for ideal functioning.

The TX port of the GPS module is connected to the RX port of the ESP32 module. This means that the tranmission part of the module sends digital signals to the receiving port of the ESP32 signal. It is analogous to transmission and receiving in radio.

As displayed below, the yellow wire is connected to the 3V supply, the brown wire is connected to Ground, and the blue wire is connected to the TX port of the GPS module and the RX port of the ESP32 microcontroller.

![GPS Setup 1](https://gitlab.com/anair11/com3505-student-2022/-/raw/main/LA2/img3.png)

![GPS Setup 2](https://gitlab.com/anair11/com3505-student-2022/-/raw/main/LA2/img2.png)

For the LED lights, the red wire outputs a digital signal to the red LED light while the green wire outputs a digital signal to the green LED light.

More details on the ports for the [ESP32](https://makeabilitylab.github.io/physcomp/esp32/esp32.html#esp32-pin-list) and the [Adafruit GPS FeatherWing](https://cdn-learn.adafruit.com/downloads/pdf/adafruit-ultimate-gps-featherwing.pdf) can be found.

## 1.3. The Code
The main function of the code uses the [Adafruit_GPS.h](https://github.com/adafruit/Adafruit_GPS) Arduino library. Using in-built functions, it streamlines the process of connecting to satellites, receiving GPS signals, and calculate the coordinates to a high accuracy.

Under the setup function, the GPS variable is initalized to read signals from the module at a BAUD rate of 9600.

Under the loop function, the GPS data is read approximately every second. All of the triangulation calculations are handled under the hood by the library. These are simply accessed using the variables stored in the GPS class initialized:

```cpp
// Connect to the GPS on the hardware port - init
Adafruit_GPS GPS(&GPSSerial);

// Access the latitude and longitude in Degrees to an accuracy of 5 decimals
int lat = (GPS.latitudeDegrees, 5);
int lon = (GPS.longitudeDegrees, 5);
```

This data is then sent to the server (Chapter 3), provided the ESP32 is conencted to the internet, through a GET request with the latitude and longitude data (15 decimal accuracy) passed as parameters to the server (accessed via it's IPv4 address under port 3000):

```cpp
if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;

    String urlreq = "http://176.58.116.164:3000/append/";
    urlreq += runNum;
    urlreq += "/";
    urlreq += String(GPS.latitudeDegrees, 15);
    urlreq += "/";
    urlreq += String(GPS.longitudeDegrees, 15);

    http.begin(urlreq);
    int httpCode = http.GET();
}
```

The code that is deployed onto the microcontroller is available in [ProjectThing.ino](https://gitlab.com/anair11/com3505-student-2022/-/blob/main/LA2/ProjectThing/ProjectThing.ino).
The code used is heavily influenced by the [Example Code](https://github.com/adafruit/Adafruit_GPS/blob/master/examples/GPS_HardwareSerial_Timing/GPS_HardwareSerial_Timing.ino) provided by Adafruit.

# 2. Connecting to the Internet
There are many methods that can be used to provision WiFi for an ESP32 controller. For this project the network username is set to "network" and the password "12345678". These are the credentials for my personal mobile hotspot using the [WiFiClient.h](https://www.arduino.cc/reference/en/libraries/wifi/) Arduino Library.

Using these credentials, the ESP32 connects to the internet:

```cpp
#include <WiFiClient.h>

// Network Credentials Definition
const char* ssid = "network";
const char* password =  "12345678";

// WiFi Setup in setup()
void setup() {
    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the WiFi network");
}
```

# 3. Cloud based Server
The main aim of this project is to provide useful insights and data in a visually appealing manner to a user. Using GPS coordinates collected during a run, one can extract a huge amount of workout insights including workout path, average pace, total distance, time taken and more. 

To complete this task, a server was rented from [Linode](https://www.linode.com), a platform providing cloud based services. Linux based servers running other Linux distros can also be rented from other providers such as [DigitlOcean](https://www.digitalocean.com/products/droplets), [Amazon Web Services](https://aws.amazon.com/ec2/?nc2=h_ql_prod_cp_ec2), [Microsoft Azure](https://azure.microsoft.com/en-gb/services/virtual-machines/), [Google Cloud](https://cloud.google.com/compute), etc. Most providers offer free credits for the first few months.

## 3.1. Server Setup
The server in question is a Linux based Ubuntu server running a Node.js application to accept [RESTful API]() requests. 

The server was setup to run the Node.js application through the following steps:

1. SSH(Secure Shell) into the server using the [command]((https://phoenixnap.com/kb/ssh-to-connect-to-remote-server-linux-or-windows)):
    ```bash
    ssh root@[IPv4 address]
    ```

2. Install NVM which is the [Node Version Manager](https://github.com/nvm-sh/nvm) using the command:
    ```bash
    curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.1/install.sh | bash
    ```

3. Reload the bash preferences with the command:
    ```bash
    source ~/.bashrc
    ```

4. Install the correct Node.js version:
    - Get the latest available version:
        ```bash
        nvm ls-remote
        ```
    
    - Install the specified Node.js version:
        ```bash
        nvm install 18.0.0
        ```

5. Install [PM2](https://pm2.keymetrics.io) which is a Node.js application Process Manager using the command:
    ```bash
    npm i -g pm2
    ```

After completing the above steps, the server is ready to host a Node.js application.

The above steps were sourced from the following [YouTube Video](https://www.youtube.com/watch?v=fJ4x00SR7vo).

## 3.2. The Node.js Application
The Node.js application developed is reponsible for accepting GET requests, process them based on the endpoint and parameters, and return the necessary data to the client. [Express.js](https://expressjs.com) is a popular Node.js web application framework to design API GET, POST, and other endpoints for a URL that is used in this project.

Install Express.js:
```bash
npm i -g express
```

The application directory also consists of a [run.json](https://gitlab.com/anair11/com3505-student-2022/-/blob/main/LA2/ProjectThing/run.json) that stores run data along with the an array of recorded latitudes and longitudes.

```json
{
    "runid": 1,
    "lat": [],
    "lon": []
}
```

The following endpoints were setup for their respective purposes:

1. This endpoint resets the run JSON file containing some default workout data:
    ```js
    app.get('/resetfile', (req, res) => {
        // Endpoint: /resetfile
    });
    ```

2. This endpoint initializes a new run in the run JSON file:

    ```js
    app.get('/newrun', (req, res) => {
        // Endpoint: /newrun
    });
    ```

3. This endpoint receives GPS latitudes and longitudes and stores in JSON format to the file run.json for the workout being recorded:

    ```js
    app.get('/append/:runid/:lat/:lon', (req, res) => {
        // Endpoint: /append/:runid/:lat/:lon
    });
    ```

4. This endpoint calculates the number of currently stored data for all the recorded runs in the run.json file:

    ```js
    app.get('/getNumRuns', (req, res) => {
        // Endpoint: /getNumRuns
    });

5. This endpoint collects latitude and longitude data for a specific run to display to the front end to the user for workout analysis:

    ```js
    app.get('/analyze-run/:runNum', (req, res) => {
        // Endpoint: /analyze-run/:runNum
    });
    ```

The app was hosted on port 3000:
```js
app.listen(port, () => {
    console.log('Server is up on port ' + port)
});
```

For more detailed documentation on setting up an Express.js application, check the [Express.js Official Documentation](https://expressjs.com/en/starter/hello-world.html) and helpful documentation by [Tutorials Point](https://www.tutorialspoint.com/nodejs/nodejs_express_framework.htm).

The detailed code accepting and processing these requests is available in the file [app.js](https://gitlab.com/anair11/com3505-student-2022/-/blob/main/LA2/ProjectThing/app.js).

The Node.js file is run using the command:
```bash
node app.js
```

The application can be deployed for production using PM2 using the command:
```bash
pm2 start app.js
```
Logs can be displayed using:
```bash
pm2 logs
```

# 4. Analysis Tool
## 4.1. Client-Side Website Developement
The analysis tool is a client side website written in HTML5, CSS, and Vanilla JavaScript. The code is available [here](https://gitlab.com/anair11/com3505-student-2022/-/tree/main/LA2/ProjectThing/pages).

An example of a workout under analysis:

![Frontend](https://gitlab.com/anair11/com3505-student-2022/-/raw/main/LA2/img1.png)

The website is not served via the Internet due to certain restrictions set by the Google Maps JavaScript API regarding Client-Side requests.

Hence, the website is booted locally using LocalHost via the following steps:

1. Download [Visual Studio Code](https://code.visualstudio.com).

2. Open up Visual Studio Code. Navigate to Extensions on the left hand side.

3. Install the Live Server extension with Extension ID: ritwickdey.LiveServer

4. Open the directory containing this repository.

5. Right click on the main.html file and click 'Open with Live Server'.

## 4.2. Google Maps JavaScript API
Displaying the workout data on a map as path on the map was possible with the help of Google's [Maps JavaScript API](https://developers.google.com/maps/documentation/JavaScript/overview).

With a [Google Cloud Account](https://cloud.google.com) created, navigate to 'Credentials' and create a new API key. The next step is to enable the Maps JavaScript API under 'Enabled APIs & Services'. Navigate back to 'Credentials' and save the LocalHost IPv4 Address as a verified HTTP Referrer.

The API is now initialized and accessible.

The map is initialized using the following code:

```js
var map = new google.maps.Map(document.getElementById("map"), {
    zoom: 17,
    center: new google.maps.LatLng(latitude, longitude),
    mapTypeId: "terrain",
});
```

The coordinates are marked on the map using the following code:
```js
const runPath = new google.maps.Polyline({
    path: runCoords,
    geodesic: true,
    strokeColor: "#e34444",
    strokeOpacity: 1.0,
    strokeWeight: 4,
});

runPath.setMap(map);
```

Marking and plotting lines on the map in detail can be found in the [source code](https://gitlab.com/anair11/com3505-student-2022/-/tree/main/LA2/ProjectThing/pages/main.html).