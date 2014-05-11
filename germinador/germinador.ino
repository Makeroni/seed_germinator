//Ethernet and Xively
#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Xively.h>

//DHT sensor for temperature and humidity
#include <dht.h>
dht DHT;
#define DHT11_PIN A5
//--

// MAC address for your Ethernet shield
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Your Xively key to let you upload data
char xivelyKey[] = "QLCgAl1udlvt19iBiS9RbNb3Fqdm1Y9tMcw5ZwAPPVXL5LLf";
//Xively feed if:
long xively_feed_id  = 767893140;

// Analog pin which we're monitoring (0 and 1 are used by the Ethernet shield)
int sens_temperatura_pin = 2;
int sens_humedad_pin = 3;
// number of datastreams
int num_datastreams = 2; 

// Define the strings for our datastream IDs
//char sensorId[] = "temperatura";
XivelyDatastream datastreams[] = {
  XivelyDatastream("temperatura", strlen("temperatura"), DATASTREAM_FLOAT),
  XivelyDatastream("humedad", strlen("humedad"), DATASTREAM_FLOAT),
};
// Finally, wrap the datastreams into a feed
XivelyFeed feed(xively_feed_id, datastreams, num_datastreams);

EthernetClient client;
XivelyClient xivelyclient(client);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("Starting single datastream upload to Xively...");
  Serial.println();
  while (Ethernet.begin(mac) != 1)
   {
   Serial.println("Error getting IP address via DHCP, trying again...");
   delay(15000);
   }
  Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)");
}

void loop() {
  int x, n_values;
  n_values = 5;
  float average_temperature=0.0, average_humidity=0.0;

  for(x=1; x <= n_values; x++)
  {
    // READ DATA
    Serial.print("DHT11, \t");
    int chk = DHT.read11(DHT11_PIN);
    switch (chk)
    {
      case DHTLIB_OK:
        Serial.print("OK,\t");
        break;
      case DHTLIB_ERROR_CHECKSUM:
        Serial.print("Checksum error,\t");
        break;
      case DHTLIB_ERROR_TIMEOUT:
        Serial.print("Time out error,\t");
        break;
      default:
        Serial.print("Unknown error,\t");
        break;

    }

    //Debug
    Serial.print("T:");
    Serial.print(DHT.temperature);
    Serial.print(" H:");
    Serial.println(DHT.humidity);

    average_temperature = average_temperature + DHT.temperature;
    average_humidity = average_humidity + DHT.humidity;
    delay(1000);
  }




  //Load the values of the sensor in the datastream
  datastreams[0].setFloat(average_temperature/n_values);
  datastreams[1].setFloat(average_humidity/n_values);
  //--

  Serial.print("Read sensor value: Temperatura ");
  Serial.println(datastreams[0].getFloat());

  Serial.print("Read sensor value: Humedad ");
  Serial.println(datastreams[1].getFloat());

  Serial.println("Uploading it to Xively");
  int ret = xivelyclient.put(feed, xivelyKey);
  Serial.print("xivelyclient.put returned ");
  Serial.println(ret);

  Serial.println();
  delay(60000);
}


