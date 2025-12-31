# BiDiB Arduino Library - Benutzerhandbuch

Dieses Handbuch erklärt, wie Sie die BiDiB Arduino Library verwenden, um Ihre eigenen BiDiB-fähigen Geräte zu erstellen.

## Installation

1.  **Bibliothek herunterladen:** Laden Sie die neueste Version der Bibliothek aus dem [GitHub-Repository](https://github.com/your-repo-link) herunter.
2.  **In der Arduino IDE installieren:**
    *   Öffnen Sie die Arduino IDE.
    *   Gehen Sie zu `Sketch` -> `Bibliothek einbinden` -> `.ZIP-Bibliothek hinzufügen...`.
    *   Wählen Sie die heruntergeladene ZIP-Datei aus.
    *   Die Bibliothek wird installiert und ist unter dem Namen "BiDiB" verfügbar.

## "Hallo Welt": Ein minimaler BiDiB-Knoten

Dieses Beispiel zeigt, wie man einen minimalen BiDiB-Knoten erstellt, der sich mit einem BiDiB-Bus verbinden und von einem Master erkannt werden kann.

```cpp
#include <Arduino.h>
#include <BiDiB.h>

// Eine BiDiB-Instanz erstellen
BiDiB bidib;

void setup() {
  // Die serielle Schnittstelle für die BiDiB-Kommunikation öffnen
  Serial.begin(115200);

  // Die BiDiB-Bibliothek mit der seriellen Schnittstelle initialisieren
  bidib.begin(Serial);

  // Eine eindeutige ID für diesen Knoten festlegen (ersetzen Sie sie durch Ihre eigene)
  uint8_t my_unique_id[7] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
  memcpy(bidib.unique_id, my_unique_id, 7);

  // Einige Features setzen (optional)
  bidib.setFeature(BIDIB_FEATURE_FW_UPDATE_SUPPORT, 0); // Firmware-Update nicht unterstützt
  bidib.setFeature(BIDIB_FEATURE_STRING_SIZE, 32);      // Maximale String-Größe
}

void loop() {
  // Die update()-Funktion muss regelmäßig aufgerufen werden, um eingehende Nachrichten zu verarbeiten
  bidib.update();

  // Wenn eine Nachricht verfügbar ist, wird sie verarbeitet
  if (bidib.messageAvailable()) {
    bidib.handleMessages();
  }
}
```

## Eine Lokomotive steuern

Dieses Beispiel zeigt, wie die Bibliothek als Zentrale verwendet werden kann, um eine Lokomotive auf dem Gleis zu steuern.

```cpp
#include <Arduino.h>
#include <BiDiB.h>

BiDiB bidib;

void setup() {
  Serial.begin(115200);
  bidib.begin(Serial);

  // ... (Eindeutige ID und Feature-Setup wie oben) ...
}

void loop() {
  bidib.update();

  if (bidib.messageAvailable()) {
    bidib.handleMessages();
  }

  // Beispiel: Sobald die Anmeldung erfolgt ist, die Gleisspannung einschalten und eine Lok fahren
  if (bidib.isLoggedIn()) {
    // Gleisspannung einschalten
    bidib.setTrackState(BIDIB_CS_STATE_GO);

    // Lokomotive mit Adresse 3 mit halber Geschwindigkeit vorwärts fahren
    bidib.drive(3, 64, 0); // Adresse, Geschwindigkeit (-127 bis 127), Funktionen

    // 5 Sekunden warten
    delay(5000);

    // Lokomotive anhalten
    bidib.drive(3, 0, 0);

    // Weitere 5 Sekunden warten
    delay(5000);
  }
}
```

## DCC-Zubehör steuern

Sie können auch DCC-Zubehör wie Weichen oder Signale steuern, die an einen Zubehördecoder am Gleis angeschlossen sind.

```cpp
// Diese Funktion wird aufgerufen, wenn eine Zubehör-Quittung empfangen wird
void handleAccessoryAck(uint16_t address, uint8_t status) {
  Serial.print("Zubehör-Quittung für Adresse ");
  Serial.println(address);
}

void setup() {
  // ... (Setup-Code wie oben) ...
  bidib.onAccessoryAck(handleAccessoryAck);
}

void loop() {
  // ... (Update-Schleife wie oben) ...

  if (bidib.isLoggedIn()) {
    // Weiche mit Adresse 10 in den 'Ein'-Zustand schalten (z.B. abbiegend)
    bidib.accessory(10, 0, 1);
    delay(2000);

    // Zurück in den 'Aus'-Zustand schalten (z.B. geradeaus)
    bidib.accessory(10, 0, 0);
    delay(2000);
  }
}
```

## Belegtmeldungen empfangen und senden

Die Bibliothek kann Rückmeldungen von Belegtmeldern und anderen Sensoren am Bus über Callback-Funktionen empfangen. Sie kann auch selbst als Melder agieren und Belegtmeldungen an den Master senden.

```cpp
// Callback für einzelne Belegtereignisse
void handleOccupancy(uint8_t detectorNum, bool occupied) {
  Serial.print("Melder ");
  Serial.print(detectorNum);
  Serial.println(occupied ? " ist belegt." : " ist frei.");
}

// Callback für einen Bereich von Belegtereignissen
void handleOccupancyMultiple(uint8_t baseNum, uint8_t size, const uint8_t* data) {
    Serial.print("Mehrfach-Belegtmeldung von Basis ");
    Serial.println(baseNum);
    // Verarbeiten Sie die Bitmap 'data' für 'size' Melder
}


// Callback für Lok-Adressmeldungen (z.B. von einem Railcom-Melder)
void handleAddress(uint8_t detectorNum, uint16_t address) {
  Serial.print("Lok mit Adresse ");
  Serial.print(address);
  Serial.print(" bei Melder ");
  Serial.print(detectorNum);
  Serial.println(" erkannt.");
}

void setup() {
  // ... (Setup-Code wie oben) ...

  // Die Callback-Funktionen registrieren
  bidib.onOccupancy(handleOccupancy);
  bidib.onOccupancyMultiple(handleOccupancyMultiple);
  bidib.onAddress(handleAddress);
  bidib.onSpeedUpdate(handleSpeed);
  bidib.onCvUpdate(handleCv);
}

// Callback für Geschwindigkeitsmeldungen
void handleSpeed(uint16_t address, uint16_t speed) {
  Serial.print("Lok mit Adresse ");
  Serial.print(address);
  Serial.print(" meldet Geschwindigkeit: ");
  Serial.println(speed);
}

// Callback für CV-Rückmeldungen
void handleCv(uint16_t address, uint16_t cv, uint8_t value) {
  Serial.print("Lok mit Adresse ");
  Serial.print(address);
  Serial.print(" meldet CV ");
  Serial.print(cv);
  Serial.print(" = ");
  Serial.println(value);
}

void loop() {
  // Die Update-Schleife löst die Callbacks automatisch aus, wenn Nachrichten empfangen werden
  bidib.update();
  if (bidib.messageAvailable()) {
    bidib.handleMessages();
  }

  // Beispiel für das Agieren als Melder und Senden von Rückmeldungen
  // Dies würde typischerweise durch einen Hardware-Sensor ausgelöst
  static bool meinMelderZustand = false;
  // if (/* eine Hardware-Bedingung, um eine Änderung zu melden */) {
  //     meinMelderZustand = !meinMelderZustand;
  //     bidib.sendOccupancySingle(10, meinMelderZustand); // Zustand für Melder Nummer 10 melden
  // }
}
```

## Booster verwalten

Sie können BiDiB-fähige Booster steuern und überwachen.

```cpp
// Callback für Booster-Statusmeldungen
void handleBoosterStatus(uint8_t status) {
  Serial.print("Booster-Status geändert: ");
  Serial.println(status);
}

void setup() {
  // ... (Setup-Code wie oben) ...
  bidib.onBoosterStatus(handleBoosterStatus);
}

void loop() {
  // ... (Update-Schleife wie oben) ...

  if (bidib.isLoggedIn()) {
    // Alle Booster einschalten (Broadcast an Adresse 0)
    bidib.setBoosterState(true, 0);
    delay(10000);

    // Alle Booster ausschalten
    bidib.setBoosterState(false, 0);
    delay(10000);
  }
}
```

## Lichtsteuerung (Light Control)

Die Bibliothek bietet Funktionen zur Steuerung von Light Control (LC) Ports, die für Signale, Gebäudebeleuchtungen und andere Effekte verwendet werden können.

```cpp
// Callback für LC-Port-Statusmeldungen
void handleLcStat(uint8_t portType, uint8_t portNum, uint8_t state) {
  Serial.print("LC Port Typ ");
  Serial.print(portType);
  Serial.print(", Nummer ");
  Serial.print(portNum);
  Serial.print(" hat jetzt Zustand ");
  Serial.println(state);
}

void setup() {
  // ... (Setup-Code wie oben) ...
  bidib.onLcStat(handleLcStat);
}

void loop() {
  // ... (Update-Schleife wie oben) ...

  if (bidib.isLoggedIn()) {
    // Einen Licht-Port einschalten
    bidib.setLcOutput(BIDIB_PORTTYPE_LIGHT, 1, 255); // Typ, Port-Nummer, Zustand (z.B. Helligkeit)
    delay(2000);

    // Ausschalten
    bidib.setLcOutput(BIDIB_PORTTYPE_LIGHT, 1, 0);
    delay(2000);

    // Einen Servo-Port konfigurieren
    // Die Servo-Geschwindigkeit einstellen
    bidib.setLcConfigX(BIDIB_PORTTYPE_SERVO, 0, BIDIB_PCFG_SERVO_SPEED, 10);
  }
}
```

## Makros verwenden

BiDiB unterstützt komplexe Befehlssequenzen, sogenannte Makros, die auf Zubehörknoten gespeichert und ausgeführt werden können.

```cpp
// Callback für Makro-Zustandsmeldungen
void handleMacroState(uint8_t macroNum, uint8_t state) {
  Serial.print("Makro ");
  Serial.print(macroNum);
  Serial.print(" Zustand ist: ");
  Serial.println(state);
}

void setup() {
  // ... (Setup-Code wie oben) ...
  bidib.onLcMacroState(handleMacroState);
}

void controlMacro(uint8_t targetNode) {
  if (bidib.isLoggedIn()) {
    // Ein einfaches Makro auf dem Zielknoten definieren
    // Schritt 0: Licht-Port 5 einschalten
    bidib.setMacroStep(0, 0, 0, 5, 0, 255); // Makro 0, Schritt 0, Verzögerung 0, Port 5, Zustand 255
    delay(50);
    // Schritt 1: 2 Sekunden warten (20 * 100ms) - BIDIB_MSYS_DELAY_FIXED ist 244
    bidib.setMacroStep(0, 1, 244, 20, 0, 0);
    delay(50);
    // Schritt 2: Licht-Port 5 ausschalten
    bidib.setMacroStep(0, 2, 0, 5, 0, 0);
    delay(50);
    // Schritt 3: Ende des Makros - BIDIB_MSYS_END_OF_MACRO ist 255
    bidib.setMacroStep(0, 3, 255, 0, 0, 0);
    delay(50);

    // Das Makro speichern
    bidib.handleMacro(targetNode, BIDIB_MACRO_SAVE);
    delay(100);

    // Das Makro starten
    bidib.handleMacro(targetNode, BIDIB_MACRO_START);
  }
}
```

## RailcomPlus®

Die Bibliothek kann RailcomPlus-Befehle für erweiterte Decoder-Rückmeldungen und Konfigurationen verarbeiten.

```cpp
// Callback für eingehende RailcomPlus-Befehle vom Master
void handleRcPlus(uint8_t opcode, const uint8_t* data, uint8_t len) {
  Serial.print("RailcomPlus-Befehl mit Opcode empfangen: ");
  Serial.println(opcode);
  // Hier würden Sie den Befehl basierend auf dem Opcode und den Daten verarbeiten.
  // Zum Beispiel könnten Sie mit einer MSG_CS_RCPLUS_ACK antworten.
  uint8_t response_data[] = { 0x01, 0x02 };
  bidib.sendRcPlusAck(BIDIB_CS_RCPLUS_ACK_TID, response_data, sizeof(response_data));
}

void setup() {
  // ... (Setup-Code wie oben) ...
  bidib.onRcPlus(handleRcPlus);
}

void loop() {
  // Die Update-Schleife löst den Callback automatisch aus, wenn Nachrichten empfangen werden
  bidib.update();
  if (bidib.messageAvailable()) {
    bidib.handleMessages();
  }
}
```

## Firmware-Update durchführen

Die Bibliothek unterstützt die Durchführung eines Firmware-Updates für einen Knoten auf dem Bus. Dies ist ein fortgeschrittener Anwendungsfall.

```cpp
// Diese Funktion wird aufgerufen, um den Update-Status zu überwachen
void handleFwUpdateStatus(uint8_t status_code, uint8_t detail) {
  Serial.print("Firmware-Update-Status: ");
  Serial.print(status_code);
  Serial.print(" - Detail: ");
  Serial.println(detail);
}

void setup() {
  // ... (Setup-Code wie oben) ...
  bidib.onFirmwareUpdateStatus(handleFwUpdateStatus);
}

void performFirmwareUpdate(uint8_t targetNodeAddress) {
  // Schritt 1: Update-Modus für den Zielknoten starten
  bidib.enterFirmwareUpdateMode(targetNodeAddress);
  delay(100); // Warten, bis der Knoten bereit ist

  // Schritt 2: Firmware-Daten in Blöcken senden
  // (Dies ist eine vereinfachte Darstellung)
  const uint8_t firmware_data[] = { 0xDE, 0xAD, 0xBE, 0xEF };
  bidib.sendFirmwareUpdateData(targetNodeAddress, firmware_data, sizeof(firmware_data));

  // Schritt 3: Update beenden
  bidib.signalFirmwareUpdateDone(targetNodeAddress);
  delay(100);
  bidib.exitFirmwareUpdateMode(targetNodeAddress);
}
```

## Hersteller-spezifische Konfiguration

BiDiB ermöglicht die Kommunikation mit herstellerspezifischen Konfigurations- oder Befehlsnachrichten.

```cpp
// Callback für eingehende Vendor-Antworten
void handleVendorData(uint8_t node_addr, const char* name, const char* value) {
  Serial.print("Hersteller-Daten von Knoten ");
  Serial.print(node_addr);
  Serial.print(": ");
  Serial.print(name);
  Serial.print(" = ");
  Serial.println(value);
}

void setup() {
  // ... (Setup-Code wie oben) ...
  bidib.onVendorData(handleVendorData);
}

void queryVendorSetting(uint8_t targetNode) {
  // Vendor-Modus aktivieren
  bidib.vendorEnable(targetNode);
  delay(50);

  // Einen Wert abfragen
  bidib.vendorGet(targetNode, "brightness");
  delay(50);

  // Einen Wert setzen
  bidib.vendorSet(targetNode, "mode", "auto");
  delay(50);

  // Vendor-Modus deaktivieren
  bidib.vendorDisable(targetNode);
}
```

## Wichtige Funktionen

-   `begin(Stream &serial)`: Initialisiert die Bibliothek mit einer seriellen Schnittstelle.
-   `update()`: Liest und verarbeitet eingehende Daten von der seriellen Schnittstelle. Rufen Sie dies in Ihrer Hauptschleife `loop()` auf.
-   `handleMessages()`: Interpretiert eine vollständig empfangene Nachricht.
-   `isLoggedIn()`: Gibt `true` zurück, wenn der Knoten erfolgreich am BiDiB-Bus angemeldet ist.
-   `setTrackState(uint8_t state)`: Setzt den Zustand der Gleisspannung (`BIDIB_CS_STATE_OFF`, `BIDIB_CS_STATE_STOP`, `BIDIB_CS_STATE_GO`).
-   `drive(uint16_t address, int8_t speed, uint8_t functions)`: Sendet einen Fahrbefehl an eine Lokomotive.
-   `accessory(uint16_t address, uint8_t output, uint8_t state)`: Sendet einen Befehl an ein DCC-Zubehör.
-   `pomWriteByte(uint16_t address, uint16_t cv, uint8_t value)`: Schreibt einen CV-Wert auf dem Hauptgleis (PoM).
-   `setBoosterState(bool on, uint8_t node_addr)`: Schaltet einen Booster ein oder aus.
-   `queryBooster(uint8_t node_addr)`: Fordert den Status eines Boosters an.
-   `setAccessory(uint8_t accessoryNum, uint8_t aspect)`: Setzt den Zustand eines nativen BiDiB-Zubehörs.
-   `getAccessory(uint8_t accessoryNum)`: Fordert den Zustand eines nativen BiDiB-Zubehörs an.
-   `sendOccupancySingle(uint8_t detectorNum, bool occupied)`: Sendet eine Belegtmeldung für einen einzelnen Melder.
-   `sendOccupancyMultiple(uint8_t baseNum, uint8_t size, const uint8_t* data)`: Sendet eine Belegtmeldung für einen Bereich von Meldern.
-   `setLcOutput(uint8_t portType, uint8_t portNum, uint8_t state)`: Setzt den Zustand eines Light Control Ports.
-   `setLcConfigX(uint8_t portType, uint8_t portNum, uint8_t enumVal, uint8_t value)`: Setzt einen Konfigurationsparameter für einen LC-Port.
-   `getLcConfigX(uint8_t portType, uint8_t portNum)`: Fordert die Konfiguration eines LC-Ports an.
-   `handleMacro(uint8_t macroNum, uint8_t opCode)`: Steuert ein Makro (Start, Stopp, Speichern etc.).
-   `setMacroStep(...)`: Setzt einen einzelnen Schritt in einem Makro.
-   `getMacroStep(uint8_t macroNum, uint8_t stepNum)`: Fordert einen einzelnen Schritt aus einem Makro an.
-   `setMacroParameter(...)`: Setzt einen Parameter für ein Makro.
-   `getMacroParameter(uint8_t macroNum, uint8_t paramNum)`: Fordert einen Parameter eines Makros an.
-   `vendorEnable(uint8_t node_addr)`: Aktiviert den herstellerspezifischen Modus auf einem Knoten.
-   `vendorDisable(uint8_t node_addr)`: Deaktiviert den herstellerspezifischen Modus auf einem Knoten.
-   `vendorGet(uint8_t node_addr, const char* name)`: Liest einen herstellerspezifischen Parameter.
-   `vendorSet(uint8_t node_addr, const char* name, const char* value)`: Setzt einen herstellerspezifischen Parameter.
-   `enterFirmwareUpdateMode(uint8_t node_addr)`: Weist einen Knoten an, in den Firmware-Update-Modus zu wechseln.
-   `exitFirmwareUpdateMode(uint8_t node_addr)`: Weist einen Knoten an, den Firmware-Update-Modus zu verlassen.
-   `sendFirmwareUpdateData(...)`: Sendet eine Zeile Firmware-Daten an den Knoten.

### Funktionen zur Callback-Registrierung

-   `onDriveAck(callback)`: Registriert eine Funktion zur Behandlung von Fahrbefehl-Quittungen.
-   `onAccessoryAck(callback)`: Registriert eine Funktion zur Behandlung von DCC-Zubehör-Quittungen.
-   `onPomAck(callback)`: Registriert eine Funktion zur Behandlung von PoM-Schreib-Quittungen.
-   `onOccupancy(callback)`: Registriert eine Funktion zur Behandlung von Belegtmeldungen (`belegt`/`frei`).
-   `onOccupancyMultiple(callback)`: Registriert eine Funktion für Mehrfach-Belegtmeldungen.
-   `onAddress(callback)`: Registriert eine Funktion zur Behandlung von Adressmeldungen von Meldern.
-   `onAccessoryState(callback)`: Registriert eine Funktion zur Behandlung von Zustandsmeldungen von nativem BiDiB-Zubehör.
-   `onBoosterStatus(callback)`: Registriert eine Funktion zur Behandlung von Booster-Statusmeldungen.
-   `onBoosterDiagnostic(callback)`: Registriert eine Funktion zur Behandlung von Booster-Diagnosemeldungen.
-   `onSpeedUpdate(callback)`: Registriert eine Funktion zur Behandlung von Geschwindigkeitsmeldungen von Meldern.
-   `onCvUpdate(callback)`: Registriert eine Funktion zur Behandlung von CV-Meldungen von Meldern.
-   `onFirmwareUpdateStatus(callback)`: Registriert eine Funktion zur Behandlung von Firmware-Update-Statusmeldungen.
-   `onVendorAck(callback)`: Registriert eine Funktion zur Behandlung von Vendor-Quittungen.
-   `onVendorData(callback)`: Registriert eine Funktion zur Behandlung von Vendor-Datenmeldungen.
-   `onLcStat(callback)`: Registriert eine Funktion für LC-Port-Statusmeldungen.
-   `onLcConfigX(callback)`: Registriert eine Funktion für LC-Konfigurationsmeldungen.
-   `onLcWait(callback)`: Registriert eine Funktion für LC-Wartebenachrichtigungen.
-   `onLcMacroState(callback)`: Registriert einen Callback für Makro-Zustandsmeldungen.
-   `onLcMacro(callback)`: Registriert einen Callback für Makro-Schrittmeldungen.
-   `onLcMacroPara(callback)`: Registriert einen Callback für Makro-Parametermeldungen.
-   `onRcPlus(callback)`: Registriert einen Callback für RailcomPlus-Befehle.
