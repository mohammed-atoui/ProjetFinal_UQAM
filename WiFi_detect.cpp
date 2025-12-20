#include <WiFi.h>
#include <U8g2lib.h>

// Configuration de l'écran ST7920 en SPI avec pins spécifiques
U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, 18, 23, 5, 17);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true); // Assure qu'on ne reste pas connecté
  delay(1000);
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tf); // Police lisible
}

void loop() {
  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("Aucun réseau trouvé.");
    u8g2.clearBuffer();
    u8g2.drawStr(0, 20, "Aucun reseau");
    u8g2.sendBuffer();
  } else {
    Serial.printf("scan done\n");
    Serial.printf("%d reseaux trouves\n", n); 
    Serial.println("..............................");
    // Trier par RSSI (puissance du signal)
    for (int i = 0; i < n - 1; i++) {
      for (int j = i + 1; j < n; j++) {
        if (WiFi.RSSI(i) < WiFi.RSSI(j)) {
          WiFi.scanNetworks(true); 
        }
      }
    }

    // Afficher des 6 réseaux les plus forts
    int maxNetworks = min(n, 6);
    for (int i = 0; i < maxNetworks; i++) {
      String ssid = WiFi.SSID(i);
      int rssi = WiFi.RSSI(i);
      String mac = WiFi.BSSIDstr(i);
      int encType = WiFi.encryptionType(i);
      int channel = WiFi.channel(i);
      String bande;
      if (channel >= 1 && channel <= 13) bande = "2.4GHz";
      if (channel >= 36 && channel <= 165) bande = "5GHz";

      // Traduire le type de chiffrement en text
      String encStr;
      switch (encType) {
        case WIFI_AUTH_OPEN: encStr = "OPEN"; break;
        case WIFI_AUTH_WEP: encStr = "WEP"; break;
        case WIFI_AUTH_WPA_PSK: encStr = "WPA"; break;
        case WIFI_AUTH_WPA2_PSK: encStr = "WPA2"; break;
        case WIFI_AUTH_WPA_WPA2_PSK: encStr = "WPA/WPA2"; break;
        case WIFI_AUTH_WPA2_ENTERPRISE: encStr = "WPA2-ENT"; break;
        default: encStr = "UNKNOWN"; break;
      }
      // Affichage sur moniteur serie
      Serial.printf("         %d", i+1);
      Serial.printf("\n   Network SSID: %s\n", ssid.c_str());
      Serial.printf("   Encryption type : %s\n", encStr.c_str());
      Serial.printf("   RSSI: (%d) dBm\n", rssi);
      if (i == 0) {
        Serial.println("   Signal Strength : Strongest");
      }
      if (i == maxNetworks - 1) {
        Serial.println("   Signal Strength : Weakest");
      }
      Serial.printf("   MAC Address : %s\n", mac.c_str());
      Serial.printf("   Channel : %d\n", channel);
      Serial.printf("   Bandwidth : %s\n", bande.c_str());
      Serial.println("..............................");

      // Affichage des caractéristiques sur écran
      u8g2.clearBuffer();
      u8g2.drawStr(0, 10, ("SSID: " + ssid).c_str());
      u8g2.drawStr(0, 22, ("ENCRYPTION: " + encStr).c_str());
      u8g2.drawStr(0, 34, (String("RSSI: (") + rssi + ") dBm").c_str());
      u8g2.drawStr(0, 46, ("MAC:" + mac).c_str());
      u8g2.drawStr(0, 58, (String("CH: ") + channel + " Bande:(" + bande + ")").c_str());
      u8g2.sendBuffer();

      delay(5000); // chaque réseau reste affiché pendant 5 secondes
    }
  }
}
