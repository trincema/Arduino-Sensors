#include <EtherCard.h>

static byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
const char website[] PROGMEM = "www.cabana-rogoz.ro";
char websiteIP[] = "188.241.222.25";
byte Ethernet::buffer[500];
BufferFiller bfill;

bool reading = true;
String meter = "buc";

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  initEthernet();
  // sendData(876);
}

void loop() {
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  if (pos) { // check if valid tcp data is received
    //ether.httpServerReply(homePage());
    bfill = ether.tcpOffset();
    // storing buffer on data variable
    char *data = (char *) Ethernet::buffer + pos;
    if (strncmp("GET /", data, 5) != 0) {
      // Unsupported HTTP request
      // 304 or 501 response would be more appropriate
      http_Unauthorized();
    } else {
      data += 5;
      if (strncmp("?cmd=start", data, 12) == 0) {
        
        homepage();
      } else {
        // Page not found
        http_Unauthorized();
      }
    }
    
  }
}

void startPump() {
  
}

static word homePage() {
  long t = millis() / 1000;
  word h = t / 3600;
  byte m = (t / 60) % 60;
  byte s = t % 60;
  bfill.emit_p(PSTR(
   "HTTP/1.0 200 OK\r\n"
   "Content-Type: text/html\r\n"
   "Pragma: no-cache\r\n"
   "\r\n"
   "<meta http-equiv='refresh' content='10'/>"
   "<title>RBBB server</title>"
   "<h1>$D$D:$D$D:$D$D</h1>"),
     h/10, h%10, m/10, m%10, s/10, s%10);
  ether.httpServerReply(bfill.position());
}

void initEthernet() {
  // Change 'SS' to your Slave Select pin, if you arn't using the default pin
  if (ether.begin(sizeof Ethernet::buffer, mac, SS) == 0)
    Serial.println( "Failed to access Ethernet controller");
  else
    Serial.println("Ethernet controller initialized");
  if (!ether.dhcpSetup())
    Serial.println("Failed to get configuration from DHCP");
  else
    Serial.println("DHCP configuration done");
  ether.printIp("IP: ", ether.myip);

  ether.parseIp(ether.hisip, websiteIP);
  ether.printIp("Server: ", ether.hisip);
}

void sendData(int reading) {
  delay(5000);
  String params = "?";
  params.concat(meter);
  params.concat("=");
  params.concat(reading);
  ether.browseUrl(PSTR("/ap/logdata.php"), params.c_str(), website, my_callback);
  while (reading) {
    ether.packetLoop(ether.packetReceive());
  }
}

// Called when the client request is complete
static void my_callback (byte status, word off, word len) {
  Ethernet::buffer[off + 300] = 0;
  String data = (char*) Ethernet::buffer + off;
  // Extract data from response
  
  if (data.indexOf("Start P1") >= 0) {
    Serial.println(">> Start pump 1");  
  } else if (data.indexOf("Stop P1") >= 0) {
    Serial.println(">> Stop pump 1");
  }
  Serial.println(data);
  reading = false;
}
