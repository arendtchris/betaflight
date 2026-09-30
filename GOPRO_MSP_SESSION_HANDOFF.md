# GoPro MSP / Configurator Handoff

Stand: 2026-09-30

## Aktueller Aufbau

- Firmware-Repository: `/workspaces/betaflight`
- Der Configurator ist ein eigenes Git-Repository, liegt aber derzeit noch unter `/workspaces/betaflight/betaflight-configurator`.
- Der Benutzer verschiebt den Configurator manuell. Danach liegt er voraussichtlich neben dem Firmware-Repo unter `/workspaces/betaflight-configurator`.
- Vor und nach dem Verschieben beide Git-Statusstände prüfen; `.git`, lokale Änderungen und installierte `node_modules` erhalten.

## Implementiert

Firmware:
- `src/main/msp/msp.c`: `MSP2_GP_GET_SETTINGS` liefert `[settingId, valid, revision, value]` aus dem FC-Cache und fragt parallel den ESP32 ab.
- `MSP2_GP_SET_SETTING` wird an den ESP32 weitergeleitet und vom FC bestätigt, wenn das Senden gelingt.
- `GOPRO_MSP.md` dokumentiert Ports-Tab, Antwortpayload und Bestätigung per Polling.

Configurator:
- `src/components/tabs/ports/PortsTilesView.vue`: GoPro-UART- und Baud-Auswahl im Ports-Tab, Konfliktbestätigung sowie Save & Reboot.
- `src/components/tabs/OsdTab.vue`: GoPro-Port wird nur angezeigt; Auflösung und FPS bleiben im OSD-Tab.
- `src/composables/useGoproSettings.js`: GET/SET, Antwortprüfung, Cache-Polling und Bestätigung gesetzter Werte.
- `src/composables/ports/useFeaturePort.js`: nullable Baud-Refs typisiert.
- `src/js/msp/MSPCodes.ts`, `locales/en/messages.json` und MSP-Tests ergänzt.

## Validierung

- `make MATEKF722SE` erfolgreich.
- `npm run typecheck` erfolgreich nach der letzten Typkorrektur.
- Gezielte Vitest-Läufe: 22 Tests bestanden.
- Der abschließende ESLint-/Diff-Check wurde nach einer Benutzerunterbrechung nicht abgeschlossen.
- `npm ci` wurde ausgeführt; npm meldete 3 Audit-Funde. Node ist benutzerlokal in `~/.local/node-v24.21.0-linux-arm64` installiert (Node 24.21.0, npm 11.19.0). Für neue Shells diesen `bin`-Ordner in `PATH` aufnehmen.
- Der lokale Vite-Server wurde beendet.

## Nächste Schritte

1. Nach dem manuellen Verschieben beide Repositories prüfen.
2. Im Configurator `npm run lint` und die gezielten MSP-Tests ausführen; danach `git diff --check` in beiden Repositories.
3. Im Browser den Ports-Tab öffnen und UART 2/Baud ändern, speichern und nach dem Reboot kontrollieren. Konfliktbestätigung und GoPro-Controls im OSD-Tab prüfen.
4. Die MSP-Bridge mit angeschlossenem ESP32 verifizieren, insbesondere Cache-Revision, ungültigen Cache und SET-Bestätigung.
