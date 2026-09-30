# GoPro MSP Port

The GoPro CMS menu communicates with the ESP32 through a dedicated serial port function, `GOPRO_MSP`. Configure the UART connected to the ESP32 in the Betaflight CLI:

```text
get gopro_msp
set gopro_msp_uart = UART2
set gopro_msp_baud = 115200
save
```

Replace `UART2` with the UART name shown by the `serial` command for your flight controller. `gopro_msp_uart` defaults to `NONE` (disabled); `gopro_msp_baud` defaults to `115200`. The baud rate must match the ESP32. GoPro MSP may share a UART assigned to regular MSP because both use the same parser; in that case, the MSP and GoPro baud rates must match. Do not assign RX, GPS, or another non-MSP function to the GoPro UART.

The Betaflight Configurator Ports tab does not currently expose `GOPRO_MSP` or configure `gopro_msp_uart`. Configure this role through the CLI above. The legacy MSP serial-config write commands are not supported by this firmware's feature-based port configuration.

The menu is available under **Features > GOPRO**. It currently exposes resolution and frame rate. On menu entry, Betaflight requests both current settings from the ESP32; changing a tab sends the selected value.

## MSP2 Messages

The values below are GoPro BLE setting IDs and option bytes. Each field is one byte.

| Command | ID | Payload |
| --- | --- | --- |
| `MSP2_GP_SET_SETTING` | `0x4001` | Request: `[settingId, value]` |
| `MSP2_GP_SET_RESULT` | `0x4002` | Response to SET; payload is not currently interpreted by Betaflight |
| `MSP2_GP_GET_SETTINGS` | `0x4003` | Request: `[settingId]` |
| `MSP2_GP_SETTINGS_REPORT` | `0x4004` | Response: `[settingId, value]` |

Currently supported menu settings:

| Setting | BLE ID | Available values (option byte) |
| --- | --- | --- |
| Resolution | `2` | `4K` (`1`), `2.7K` (`4`), `1080` (`9`) |
| FPS | `3` | `60` (`5`), `50` (`6`), `30` (`8`), `25` (`9`), `24` (`10`) |

The menu uses a fixed common subset. A camera may not support every option in every mode or preset; GoPro's available setting capabilities depend on the camera and its current state.
