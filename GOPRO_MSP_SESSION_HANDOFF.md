# GoPro MSP / Configurator Handoff

Stand: 2026-09-30

## Aktueller Aufbau

- Firmware-Repository: `/workspaces/betaflight`
- Der ESP32-Sketch liegt unter `/workspaces/betaflight/GoProBridgeBle`.
- Der Configurator ist aktuell nicht unter `/workspaces` auffindbar. Der Benutzer hatte ihn manuell verschoben; vor Configurator-Arbeiten den tatsächlichen Workspace-Pfad erneut öffnen.

## Implementiert

Firmware:
- `src/main/msp/msp.c`: `MSP2_GP_GET_SETTINGS` liefert `[settingId, valid, revision, value]` aus dem FC-Cache und fragt parallel den ESP32 ab.
- `src/main/msp/msp.c`, `src/main/msp/msp_gopro.c`: MSP `0x4005/0x4006` fragen Resolution-/FPS-Capabilities beim ESP32 ab und cachen die BLE-Optionsbytes.
- `src/main/cms/cms_menu_gopro.c`: baut dynamische CMS-Tabs aus den Capability-Listen, mappt BLE-Bytes lokal auf Labels und fragt FPS-Capabilities nach Resolution-Wechsel erneut ab.
- `MSP2_GP_SET_SETTING` wird an den ESP32 weitergeleitet und vom FC bestätigt, wenn das Senden gelingt.
- `GOPRO_MSP.md` dokumentiert Ports-Tab, Antwortpayload und Bestätigung per Polling.

ESP32-Bridge:
- `GoProBridgeBle/src/Camera.cpp` speichert Capabilities aus GoPro Query `0x32` und stellt Werte/Revision bereit.
- `GoProBridgeBle/src/Esp32CamBridge.cpp` startet die BLE-Capability-Abfrage.
- `GoProBridgeBle/src/msp.cpp` sendet `0x4006` als MSP-Reply; `GoProBridgeBle/GoProBridgeBle.ino` korreliert asynchrone BLE-Antworten mit FC-Anfragen.

## Validierung

- `make MATEKF722SE` erfolgreich.
- `make --directory=/workspaces/betaflight MAMBAF722` erfolgreich nach den Capability-/CMS-Änderungen.
- `make --directory=/workspaces/betaflight/GoProBridgeBle PYTHON=python3 test` erfolgreich (Preset-Parser Hosttests).
- `g++ -std=c++17 -Wall -Wextra -Isrc -Isrc/protobuf/generated -Inanopb -fsyntax-only src/Camera.cpp` erfolgreich.
- `test-nanopb` ist blockiert, weil `GoProBridgeBle/tools/generate_nanopb.py` im Checkout fehlt.
- Kein vollständiger Arduino/ESP32-Build: Arduino CLI/PlatformIO/ESP-IDF-Toolchain sind nicht im PATH.
- Configurator-Typecheck/Vitest aus der früheren Arbeit bestanden, aber die verschobene Kopie ist aktuell nicht im Workspace auffindbar.

## Nächste Schritte

1. Configurator-Repo wieder im Workspace öffnen und dessen MSP-Code-Tabelle, Capability-Decoder sowie Optionslisten an `0x4005/0x4006` anbinden.
2. Mit `git diff --check` und `make MAMBAF722` die Firmware-Änderungen abschließend prüfen.
3. ESP32-Projekt mit seiner Arduino-Toolchain bauen, sobald diese verfügbar ist.
4. Mit GoPro-Hardware testen, dass die BLE-Capability-Liste über MSP ankommt und sich FPS-Optionen nach einer Auflösungsänderung aktualisieren.
