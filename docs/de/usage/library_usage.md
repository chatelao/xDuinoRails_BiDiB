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

## Wichtige Funktionen

-   `begin(Stream &serial)`: Initialisiert die Bibliothek mit einer seriellen Schnittstelle.
-   `update()`: Liest und verarbeitet eingehende Daten von der seriellen Schnittstelle. Rufen Sie dies in Ihrer Hauptschleife `loop()` auf.
-   `handleMessages()`: Interpretiert eine vollständig empfangene Nachricht.
-   `isLoggedIn()`: Gibt `true` zurück, wenn der Knoten erfolgreich am BiDiB-Bus angemeldet ist.
-   `setTrackState(uint8_t state)`: Setzt den Zustand der Gleisspannung (`BIDIB_CS_STATE_OFF`, `BIDIB_CS_STATE_STOP`, `BIDIB_CS_STATE_GO`).
-   `drive(uint16_t address, int8_t speed, uint8_t functions)`: Sendet einen Fahrbefehl an eine Lokomotive.
