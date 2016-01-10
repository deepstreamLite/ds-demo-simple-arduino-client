#include <Ethernet.h>
#include <SPI.h>
#include <string.h>

const int TEMP_PIN = A0;
const int RED_LED_PIN = 7;

String temp;
boolean sendTemp = false;

byte deepstreamHost[] = { 192,168,0,201 };
int deepstreamPort = 6021;

byte mac[] = { 0x98, 0xD6, 0xF7, 0x5B, 0xA0, 0xD1 };

String part = String( char( 31 ));
String split = String( char( 30 ));

String message = "";
String state = "AWAITING_AUTHENTICATION";

EthernetClient client;

void log( String log ) {
  Serial.println( log );
}

void changeState( String newState ) {
  if( newState != state ) {
    state = newState;
    log( "Connection State: " + newState ); 
  }
}

String createMessage( String type, String action ) {
    return String( type + part + action + split );
}

String createMessage( String type, String action, String data1 ) {
    return String( type + part + action + part + data1 + split );
}

String createMessage( String type, String action, String data1, String data2 ) {
    return String( type + part + action + part + data1 + part + data2 + split );
}

String getTemp() {
  int sensorValue = analogRead(TEMP_PIN);
 
  float voltage = sensorValue * 5.0;
  voltage /= 1024.0; 
 
  return String( (voltage - 0.5) * 100 );
}

void setup()
{
  // disable SD card
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  // enable led pin
  pinMode(RED_LED_PIN, OUTPUT);

  Serial.begin(9600);

  log( "Getting IP..." );
  Ethernet.begin( mac );
  log( Ethernet.localIP() );

  delay(2000);

  log( "Connecting..." );
  if (client.connect( deepstreamHost, deepstreamPort )) {
    log("Connected!");
  } else {
    log("Connection failed");
  }
}

void loop() {

   if (client.connected()) {

     // Send our credentials if we are awaiting authentication
     if( state == "AWAITING_AUTHENTICATION" ) {
        client.print( createMessage( "A", "REQ", "{\"mac\":\"98-D6-F7-5B-A0-D1\"}" ) );
        changeState( "AUTHENTICATING" );
     }

     // If we recieved a message from the server this will return true
     if (client.available()) {
      char c = client.read();
      message += c; 
      
      if( c == char( 30 ) ){

        // Login Ack Recieved
        if( state == "AUTHENTICATING" && message == createMessage( "A", "A" ) ) {
          changeState( "OPEN" );
          log( "Subscribing to event led-red" );
          client.print( createMessage( "E", "S", "led-red" ) );        

          log( "Listen for subscribers to event temperature" );
          client.print( createMessage( "E", "L", "^temperature$" ) );       
        }
        // Event Subscribe Ack Recieved 
        else if( state == "OPEN" && message == createMessage( "E", "A", "S", "led-red" ) ) {
          log( "Subscribe to event led-red Acknowledged" );
        }
        // Event Recieved to switch red led on
        else if( state == "OPEN" && message == createMessage( "E", "EVT", "led-red", "T" ) ) {
          log( "Turn red led On" );
          digitalWrite(RED_LED_PIN, HIGH);
        }
        // Event Recieved to switch red led off
        else if( state == "OPEN" && message == createMessage( "E", "EVT", "led-red", "F" ) ) {
          log( "Turn red led OFF" );
          digitalWrite(RED_LED_PIN, LOW);
        }
        // Event Listen Ack recieved
        else if( state == "OPEN" && message == createMessage( "E", "A", "L", "^temperature$" ) ) {
          log( "Listen to event ^temperature$ Acknowledged" );
        }
        // Event Listen Match recieved
        else if( state == "OPEN" && message == createMessage( "E", "SP", "^temperature$", "temperature" ) ) {
          log( "Match to event ^temperature$ found" );
          sendTemp = true;
        }
        // Event Listen no longer match
        else if( state == "OPEN" && message == createMessage( "E", "SR", "^temperature$", "temperature" ) ) {
          log( "Match to event /^temperature$/ lost" );
          sendTemp = false;
        }
        // Unsolicitated message
        else {
          log( "Unknown message recieved:" + message );
        }

        //Clear message buffer
        message = "";
      }
    }

    // If connected and there is a client interested in the temperature
    if( state == "OPEN" && sendTemp ) {
      String newTemp = getTemp();
      if( newTemp != temp ) {
        client.print( createMessage( "E", "EVT", "temperature", String( "N" + newTemp ) ) );  
        temp = newTemp;
      }
    }
    
   } 
   // Client is no longer connected
   else {
    changeState( "CLOSED" );
   }
}
