# GoPro MSP Port

The GoPro CMS menu communicates with the ESP32 through a dedicated serial port function, `GOPRO_MSP`. Configure the UART connected to the ESP32 in the Betaflight CLI:

```text
get gopro_msp
set gopro_msp_uart = UART2
set gopro_msp_baud = 115200
save
```

Replace `UART2` with the UART name shown by the `serial` command for your flight controller. `gopro_msp_uart` defaults to `NONE` (disabled); `gopro_msp_baud` defaults to `115200`. The baud rate must match the ESP32. GoPro MSP may share a UART assigned to regular MSP because both use the same parser; in that case, the MSP and GoPro baud rates must match. Do not assign RX, GPS, or another non-MSP function to the GoPro UART.

The Configurator exposes the GoPro MSP port and baud rate in the Ports tab. Saving a port or baud change requires a reboot. The legacy MSP serial-config write commands are not supported by this firmware's feature-based port configuration.

The OSD menu is available under **Features > GOPRO** and exposes resolution and frame rate. The Configurator's OSD tab exposes the same settings when a GoPro MSP port is assigned. Betaflight proxies Configurator requests to the ESP32 and returns the latest cached value and revision; after setting a value, the Configurator polls until the ESP32 reports the new value.

## MSP2 Messages

The values below are GoPro BLE setting IDs and option bytes. Each field is one byte.

| Command | ID | Payload |
| --- | --- | --- |
| `MSP2_GP_SET_SETTING` | `0x4001` | Request: `[settingId, value]` |
| `MSP2_GP_SET_RESULT` | `0x4002` | Response to SET; payload is not currently interpreted by Betaflight |
| `MSP2_GP_GET_SETTINGS` | `0x4003` | Request: `[settingId]` |
| `MSP2_GP_SETTINGS_REPORT` | `0x4004` | Response: `[settingId, value]` |
| `MSP2_GP_GET_SETTING_CAPABILITIES` | `0x4005` | Request: `[settingId]` |
| `MSP2_GP_SETTING_CAPABILITIES_REPORT` | `0x4006` | Response: `[settingId, count, value...]` |

For Configurator requests to the flight controller, `MSP2_GP_GET_SETTINGS` replies with `[settingId, valid, revision, value]`. The flight controller also forwards the request to the ESP32; `valid` is zero until a camera response has been received. `MSP2_GP_GET_SETTING_CAPABILITIES` replies to the Configurator with `[settingId, valid, revision, count, value...]` and is also forwarded to the ESP32. The ESP32 obtains currently supported options using GoPro BLE Query `0x32` (Get Setting Capabilities) and sends them in `MSP2_GP_SETTING_CAPABILITIES_REPORT`. `MSP2_GP_SET_SETTING` is acknowledged by the flight controller once forwarded, and the Configurator confirms the write by polling the cached value.

The CMS filters its locally labeled options against the camera's reported capability values. FPS capabilities are requested again after changing resolution because GoPro's available FPS values depend on the active mode and camera state.

The GoPro BLE value-label mapping is:

| Setting | BLE ID | Available values (option byte) |
| --- | --- | --- |
| Resolution | `2` | `4K` (`1`), `2.7K` (`4`), `2.7K 4:3` (`6`), `1440` (`7`), `1080` (`9`), `720` (`12`), `4K 4:3` (`18`), `5.6K` (`21`), `5K` (`24`), `5K 4:3` (`25`), `5.3K 8:7` (`26`), `5.3K 4:3` (`27`), `4K 8:7` (`28`), `8K` (`31`), `5.3K 21:9` (`35`), `4K 21:9` (`36`), `4K 1:1` (`37`), `900` (`38`), `4K SPH` (`39`), `8K 4:3` (`40`), `1440 4:3` (`44`), `5.3K` (`100`), `5.3K 8:7` (`107`), `4K 8:7` (`108`), `4K 9:16` (`109`), `1080 9:16` (`110`), `2.7K 4:3` (`111`), `4K 4:3` (`112`), `5.3K 4:3` (`113`) |
| FPS | `3` | `240` (`0`), `120` (`1`), `100` (`2`), `90` (`3`), `60` (`5`), `50` (`6`), `30` (`8`), `25` (`9`), `24` (`10`), `200` (`13`), `400` (`15`), `360` (`16`), `300` (`17`), `480` (`18`), `960` (`19`), `800` (`20`) |

These tables describe the BLE enum superset, not a promise that every camera or mode supports every option. The camera capability response is authoritative.
