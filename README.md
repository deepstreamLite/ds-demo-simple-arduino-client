# ds-demo-simple-ardruino-client
An example of how you can connect to deepstream TCP directly using an ardruino for auth and events.

You can see a detailed explanation on how this works [here](http://deepstream.io/blog/writing-a-basic-ardruino-client/).

To run:
-------------

1 ) Run a deepstream server on your localhost. You can find a tutorial on the steps needed [here](http://deepstream.io/tutorials/getting-started.html).

2 ) Open the [index.html](https://raw.githubusercontent.com/hoxton-one/ds-demo-simple-ardruino-client/master/index.html) file in any modern browser.

3) Open the ardruino sketch and change the following:

deepstreamHost: Put you deepstream IP in
deepstreamPort: Put you deepstream TCP port in ( if not using the default )

mac: Put your ardruinos unique mac address in

TEMP_PIN: Put your analogue sensor input here
RED_LED_PIN: The positive pin to drive your red led

Upload sketch and run!

Notes:
------------
* This uses an ethernet shield, but the code should be easily interchangeable with a WiFi shield instead. As long as the client has a .send() API you can just swap the Ethernet shield with one of your choice.

* This isn't the official ardruino client. We are in the process of designing a library that can be used for most embedded systems without Ardruino specific libraries. If you would like to know more please contact us at info@deepstream.io or join our slack slack IoT channel [here](https://deepstream-slack.herokuapp.com/)



