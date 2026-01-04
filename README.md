# TuBondi Tracker - ESP32 Professional Edition 🚌💨

Este proyecto es un rastreador de colectivos en tiempo real para ESP32, utilizando una pantalla **ILI9341** de 2.8" o 3.2" con controlador táctil **XPT2046**. La interfaz tiene una estética de **Matriz LED** naranja clásica de las paradas inteligentes.

## ✨ Características

- **Visualización en Tiempo Real**: Muestra los próximos arribos, distancias y tiempos de espera.
- **Reloj NTP**: Hora sincronizada automáticamente vía internet (GMT-3 Argentina).
- **Configuración Dinámica**: Portal WiFi cautivo para cambiar la Parada (StopCode) y credenciales WiFi sin tocar el código.
- **Doble Bus SPI**: Optimizado con VSPI para la pantalla y HSPI para el táctil, garantizando fluidez y evitando interferencias.
- **Filtro de Ruido Táctil**: Algoritmo para ignorar "ghost touches" y pulsaciones accidentales.

---

## 🔌 Diagrama de Conexiones (Wiring)

> [!IMPORTANT]
> **La tabla de abajo es la "Verdad Absoluta"**. Las imágenes generadas por IA pueden tener errores en las etiquetas de los pines. Usá la tabla para hacer tus conexiones.

### Tabla de Pines (Source of Truth)

| Función | Pin ESP32 | Pin Pantalla | Cable (Tu Hardware) | Bus / Tipo |
| :--- | :--- | :--- | :--- | :--- |
| **VCC** | **5V / VIN** | VCC | **Rojo** | Alimentación |
| **GND** | GND | GND | **Negro** | Tierra |
| **TFT CS** | **GPIO 5** | CS | **Amarillo** | VSPI |
| **TFT RESET**| **GPIO 4** | RESET | **Azul** | Digital |
| **TFT DC** | **GPIO 22** | D/C | **Blanco** | Digital |
| **TFT MOSI** | **GPIO 23** | SDI (MOSI) | **Naranja** | VSPI |
| **TFT SCK** | **GPIO 18** | SCK | **Amarillo** | VSPI |
| **TFT LED** | **3.3V** | LED / BL | **Blanco** | Brillo |
| **TFT MISO** | **GPIO 19** | SDO (MISO) | **Azul** | VSPI |
| **T_CLK** | **GPIO 14** | T_CLK | **Amarillo** | HSPI (Touch) |
| **T_CS** | **GPIO 15** | T_CS | **Violeta** | HSPI (Touch) |
| **T_DIN** | **GPIO 13** | T_DIN (MOSI)| **Naranja** | HSPI (Touch) |
| **T_OUT** | **GPIO 12** | T_DO (MISO) | **Azul** | HSPI (Touch) |

---

### Guía Visual (Esquema Técnico)

Para una referencia rápida del ruteo, podés usar este diagrama generado con **Mermaid CLI**. Representa los dos buses SPI (VSPI para pantalla y HSPI para táctil).

![Schematic Final](wiring_diagram_final.png)

> [!TIP]
> **Recordá usar siempre la TABLA DE ARRIBA** para conectar los cables, ya que es la única fuente de verdad que coincide exactamente con el código del programa.

---

## 🛠️ Configuración Inicial

1. **Primer Inicio**: La placa intentará conectarse. Si no hay WiFi configurado o falla, se pondrá en modo **Portal**.
2. **Conexión Local**: Conectate desde tu celular a la red WiFi llamada `BondiTracker-Setup`.
3. **Portal Web**: Entra a `http://192.168.4.1` en tu navegador.
4. **Parámetros**:
   - Configura tu red WiFi.
   - Ingresa el **Stop Code** (ej. `A911`).
   - Guarda los cambios. El dispositivo se reiniciará solo.

## 🖱️ Controles de UI

- **Botón REFRESH (Naranja)**: Actualiza los arribos manualmente en cualquier momento.
- **Icono Engranaje (Gris)**: Fuerza al dispositivo a entrar en modo configuración. Sugerido para cuando quieras cambiar la parada o la red WiFi.
- **Botón VOLVER (Rojo)**: Dentro del modo configuración, permite salir y volver a la pantalla principal sin guardar cambios (reinicia el dispositivo).

---

## 🚀 Instalación (PlatformIO)

Asegúrate de tener instaladas las siguientes librerías en tu `platformio.ini`:

```ini
lib_deps = 
    bodmer/TFT_eSPI @ ^2.5.43
    https://github.com/PaulStoffregen/XPT2046_Touchscreen
    bblanchon/ArduinoJson @ ^7.0.4
    https://github.com/tzapu/WiFiManager @ ^2.0.17
```

*Desarrollado para la comunidad de entusiastas de ESP32 y transporte público.* 🚍✨
