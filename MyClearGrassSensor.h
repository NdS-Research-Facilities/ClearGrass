#include "esphome.h"
#include "BLEDevice.h"
#include "esphome/core/log.h"

using namespace esphome;

// 58:2d:34:10:40:06
// 58:2d:34:10:3f:ee


/*

#TEST Cleargrass Zolder
esphome:
  name: 5c58b93a7d80
  platform: ESP32
  board: nodemcu-32s
  includes:
    - MyClearGrassSensor.h
#  libraries:
#    - framework-arduinoespressif32

esp32_ble_tracker:
  scan_interval: 300s
    
wifi:
  ssid: "VHK30"
  password: "9zY6*CK2WfxeC*mZ$"

# Enable logging
logger:
  level: DEBUG
# Enable Home Assistant API
api:
  password: '44f@Q6Z#4sZmY86$gM#'

ota:
  password: '44f@Q6Z#4sZmY86$gM#'

# Example configuration entry
switch:
  - platform: restart
    name: "5C58B93A7D80 Restart"
    
    
# Example configuration entry
status_led:
  pin: GPIO2

sensor:
  - platform: custom
    lambda: |-
      auto my_sensor = new MyClearGrassSensor();
      App.register_component(my_sensor);
      return {my_sensor};
    sensors:
      name: "ClearGrass Sensor"


*/


// The remote service we wish to connect to.
static BLEUUID serviceUUID("22210000-554a-4546-5542-46534450464d");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("0100");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static boolean receivedNotify = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

static const char *TAG = "MyClearGrassSensor";

static void notifyCallbackNdS(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {

  	receivedNotify = true;
  	Serial.println("NDS: in notifyCallbackNdS() %b "+isNotify);
    ////ESP_LOGD(TAG,"Notify callback for characteristic NdS");
    ////ESP_LOGD(TAG,"%s",pBLERemoteCharacteristic->getUUID().toString().c_str());
    ////ESP_LOGD(TAG," of data length ");
    ////ESP_LOGD(TAG,"%d",length);
    ////ESP_LOGD(TAG,"data:");
    //ESP_LOGD(TAG,(char*)pData);
	
    ////const int16_t rest = uint16_t(pData[0]) | (uint16_t(pData[1]) << 8);
    ////const unsigned int temperature = uint16_t(pData[2]) | (uint16_t(pData[3]) << 8);
    ////const int16_t humidity = uint16_t(pData[4]) | (uint16_t(pData[5]) << 8);
    //rest = uint16_t(pData[0]) | (uint16_t(pData[1]) << 8);
    //temperature = uint16_t(pData[2]) | (uint16_t(pData[3]) << 8);
    //humidity = uint16_t(pData[4]) | (uint16_t(pData[5]) << 8);
    //result.temperature = temperature / 10.0f;
    //result.humidity = humidity / 10.0f;
    ////ESP_LOGD(TAG,"%d",temperature);
    ////ESP_LOGD(TAG,"%d",humidity);
    ////ESP_LOGD(TAG,"%d",rest);
    ////ESP_LOGD(TAG,"");
    //ESP_LOGD(TAG,(byte)pData[0]);
    //ESP_LOGD(TAG,(byte)pData[1]);
    //ESP_LOGD(TAG,(byte)pData[2]);
    //ESP_LOGD(TAG,(byte)pData[3]);
    //ESP_LOGD(TAG,(byte)pData[4]);
    //ESP_LOGD(TAG,(byte)pData[5]); 
  }


class MyClearGrassSensor : public PollingComponent, public sensor::Sensor {

	    //const int16_t rest = uint16_t(0);
	    //const unsigned int temperature = uint16_t(0);
	    //const int16_t humidity = uint16_t(0);


	public:
	  // constructor
	  MyClearGrassSensor() : PollingComponent(60000) {
	  	Serial.println("NdS: init constructor");
	  }
	
	  void setup() override {
	    // This will be called by App.setup()
	    Serial.println("NdS: Setup()");
	   	BLEDevice::init("ESP32");
		// Retrieve a Scanner and set the callback we want to use to be informed when we
		// have detected a new device.  Specify that we want active scanning and start the
		// scan to run for 5 seconds.
		BLEScan* pBLEScan = BLEDevice::getScan();
		pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
		pBLEScan->setInterval(1349);
		pBLEScan->setWindow(449);
		pBLEScan->setActiveScan(true);
		pBLEScan->start(10, false);
		// End of setup.
		Serial.begin(115200);
	  }

	  void update() override {
	    // This will be called every "update_interval" milliseconds.
	    // If the flag "doConnect" is true then we have scanned for and found the desired
  		// BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  		// connected we set the connected flag to be true.
  		if (doConnect == true) {
  		  if (connectToServer()) {
  		    ESP_LOGD(TAG,"We are now connected to the BLE Server.");
  		  } else {
  		    ESP_LOGD(TAG,"We have failed to connect to the server; there is nothing more we will do.");
  		  }
  		  doConnect = false;
  		}

		// If we are connected to a peer BLE Server, update the characteristic each time we are reached
		// with the current time since boot.
		if (connected) {
			Serial.println("NDS: Connected...");
		  //String newValue = "Time since boot: " + String(millis()/1000);
		  //ESP_LOGD(TAG,"Setting new characteristic value to \"" + newValue + "\"");
		  
		  // Set the characteristic's value to be the array of bytes that is actually a string.
		  // NdS? pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
		} else if(doScan) {
		  BLEDevice::getScan()->start(0);  // this is just eample to start scan after disconnect, most likely there is better way to do it in arduino
		}
		
		if (receivedNotify) {
			ESP_LOGD(TAG,"RECEIVED receivedNotify() ##################################################");
		}
		ESP_LOGD(TAG,"END OF UPDATE LOOP");

		delay(5000); // Delay a 5 second between loops.
		// End of loop
	    publish_state(24.0);
	  };


	class MyClientCallback : public BLEClientCallbacks {
	  void onConnect(BLEClient* pclient) {
	    ESP_LOGD(TAG,"onConnect callback");
	  }
	
	  void onDisconnect(BLEClient* pclient) {
	    ESP_LOGD(TAG,"onDisconnect callback");
	    connected = false;
	    ESP_LOGD(TAG,"onDisconnect");
	  }
	};


	bool connectToServer() {
	    ESP_LOGD(TAG,"Forming a connection to ");
	    ESP_LOGD(TAG,"%s",myDevice->getAddress().toString().c_str());
	    
	    BLEClient*  pClient  = BLEDevice::createClient();
	    ESP_LOGD(TAG," - Created client");
	
	    pClient->setClientCallbacks(new MyClientCallback());
	
	    // Connect to the remove BLE Server.
	    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
	    ESP_LOGD(TAG," - Connected to server");
	
	    // Obtain a reference to the service we are after in the remote BLE server.
	    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
	    if (pRemoteService == nullptr) {
	      ESP_LOGD(TAG,"Failed to find our service UUID: ");
	      ESP_LOGD(TAG,"%s",serviceUUID.toString().c_str());
	      pClient->disconnect();
	      return false;
	    }
	    ESP_LOGD(TAG," - Found our service");
	
	
	    // Obtain a reference to the characteristic in the service of the remote BLE server.
	    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
	    ESP_LOGD(TAG,"Start !!!!");
	    if (pRemoteCharacteristic == nullptr) {
	      ESP_LOGD(TAG,"Failed to find our characteristic UUID: ");
	      ESP_LOGD(TAG,"%s",charUUID.toString().c_str());
	      pClient->disconnect();
	      return false;
	    }
	
	    if(pRemoteCharacteristic->canRead()) {
	      ESP_LOGD(TAG,"can read...");
	      std::string value = pRemoteCharacteristic->readValue();
	      ESP_LOGD(TAG,"The characteristic value was: ");
	      ESP_LOGD(TAG,"%s",value.c_str());
	    }
	
	    //std::map<std::string, BLERemoteCharacteristic*>* pCharacteristics = pRemoteService-> getCharacteristics();
	    //ESP_LOGD(TAG," - Found our BLERemoteCharacteristic");
	
	    
	
	    ESP_LOGD(TAG," - Start Notify Check");
	
	    if(pRemoteCharacteristic->canNotify()) {
	      ESP_LOGD(TAG," - can Notify");
	      //ESP_LOGD(TAG,pRemoteCharacteristic->canNotify());
	      ESP_LOGD(TAG," - clear Notify");
	      pRemoteCharacteristic->registerForNotify(NULL);
	      ESP_LOGD(TAG," - start Notify register");
	      pRemoteCharacteristic->registerForNotify(notifyCallbackNdS);
	      ESP_LOGD(TAG," - Notify created");
	  	} else {
	  		ESP_LOGD(TAG," - can NOT Notify");
	  		return false;
	  	}
	
	connected = true;
	return true;
	}
		/**
		 * Scan for BLE servers and find the first one that advertises the service we are looking for.
		 */
		class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
		 /**
		   * Called for each advertising BLE server.
		   */
		  void onResult(BLEAdvertisedDevice advertisedDevice) {
		    //ESP_LOGD(TAG,"BLE Advertised Device found: ");
		    ESP_LOGD(TAG,"%s",advertisedDevice.toString().c_str());
		    //ESP_LOGD(TAG,advertisedDevice.getAddress().toString().c_str());
		
		    if ( String(advertisedDevice.toString().c_str()).indexOf("ClearGrass") >= 0 ) {
		      ESP_LOGD(TAG,"ClearGrass Device found: ");
		      BLEDevice::getScan()->stop();
		      myDevice = new BLEAdvertisedDevice(advertisedDevice);
		      doConnect = true;
		      doScan = false;
		    }
		    if (advertisedDevice.haveServiceUUID()) {
		      ESP_LOGD(TAG,"advertisedDevice.haveServiceUUID() == true");
		    }
		    if (advertisedDevice.isAdvertisingService(serviceUUID)) {
		      ESP_LOGD(TAG,"advertisedDevice.isAdvertisingService(serviceUUID) == true");
		    }
		    BLEAddress* pServerAddress = new BLEAddress(advertisedDevice.getAddress());
		    ESP_LOGD(TAG,"%s",pServerAddress);
		    
		    //ESP_LOGD(TAG,advertisedDevice.isAdvertisingService(serviceUUID).toString().c_str());
		
		    // We have found a device, let us now see if it contains the service we are looking for.
		    //if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
		    
		    //if(advertisedDevice.toString().c_str() ) {
		      //ESP_LOGD(TAG,"yep!");
		      //BLEDevice::getScan()->stop();
		      //myDevice = new BLEAdvertisedDevice(advertisedDevice);
		      //doConnect = true;
		      //doScan = true;
		
		  	//} // Found our server
		    //*/
		  } // onResult
		}; // MyAdvertisedDeviceCallbacks

};
