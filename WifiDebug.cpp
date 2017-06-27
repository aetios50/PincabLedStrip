#include "WifiDebug.h"

WiFiUDP Udp;
unsigned int serverUdpPort = 4210;
const char* ssid = "XYZ";
const char* password = "XYZ";
const char* server = "192.168.4.254";


WifiDebug::WifiDebug() {
}

void WifiDebug::begin(void)
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  debug_send_msg("Started\n");
}

void WifiDebug::debug_send_msg(char * message) {
  Udp.beginPacket(server, serverUdpPort);
  Udp.write(message);
  Udp.endPacket();
}

void WifiDebug::debug_send_byte(byte b) {
  Udp.beginPacket(server, serverUdpPort);
  Udp.write((int)b);
  Udp.endPacket();

}

void WifiDebug::debug_send_int(int c) {
  char tampon[25];
  sprintf(tampon, "%d", c);
  Udp.beginPacket(server, serverUdpPort);
  Udp.write(tampon);
  Udp.endPacket();

}

void WifiDebug::debug_send_word(word w) {
  char tampon[25];
  Udp.beginPacket(server, serverUdpPort);
  byte low =  w & 0xFF;  // Take just the lowest 8 bits.
  byte high = w >> 8;  // Shift the integer right 8 bits.
  sprintf(tampon, "%d", high);
  Udp.write(tampon);
  sprintf(tampon, "%d", low);
  Udp.write(tampon);
  Udp.endPacket();
}
