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

    // Lokomotive mit Adresse 3 mit halber Geschwindigkeit fahren
    bidib.drive(3, 64, 0); // Adresse, Geschwindigkeit (0-127), Funktionen

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

## Belegtmeldungen empfangen

Die Bibliothek kann Rückmeldungen von Belegtmeldern und anderen Sensoren am Bus über Callback-Funktionen empfangen.

```cpp
// Callback für einzelne Belegtereignisse
void handleOccupancy(uint8_t detectorNum, bool occupied) {
  Serial.print("Melder ");
  Serial.print(detectorNum);
  Serial.println(occupied ? " ist belegt." : " ist frei.");
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
  bidib.onAddress(handleAddress);
}

void loop() {
  // Die Update-Schleife löst die Callbacks automatisch aus, wenn Nachrichten empfangen werden
  bidib.update();
  if (bidib.messageAvailable()) {
    bidib.handleMessages();
  }
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

### Funktionen zur Callback-Registrierung

-   `onDriveAck(callback)`: Registriert eine Funktion zur Behandlung von Fahrbefehl-Quittungen.
-   `onAccessoryAck(callback)`: Registriert eine Funktion zur Behandlung von DCC-Zubehör-Quittungen.
-   `onPomAck(callback)`: Registriert eine Funktion zur Behandlung von PoM-Schreib-Quittungen.
-   `onOccupancy(callback)`: Registriert eine Funktion zur Behandlung von Belegtmeldungen (`belegt`/`frei`).
-   `onAddress(callback)`: Registriert eine Funktion zur Behandlung von Adressmeldungen von Meldern.
-   `onAccessoryState(callback)`: Registriert eine Funktion zur Behandlung von Zustandsmeldungen von nativem BiDiB-Zubehör.
-   `onBoosterStatus(callback)`: Registriert eine Funktion zur Behandlung von Booster-Statusmeldungen.
-   `onBoosterDiagnostic(callback)`: Registriert eine Funktion zur Behandlung von Booster-Diagnosemeldungen.
