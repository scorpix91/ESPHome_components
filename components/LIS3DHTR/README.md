# LIS3DH ESPHome Component

An ESPHome external component for the **STMicroelectronics LIS3DH / LIS3DHTR** — ultra-low-power 3-axis MEMS accelerometer with an optional on-chip temperature sensor.


---

## Features

- Accéléromètre 3 axes X / Y / Z (unité : g)
- Capteur de température on-chip (via ADC3 interne, unité : °C)
- Plage de mesure configurable : ±2g / ±4g / ±8g / ±16g
- Fréquence d'échantillonnage (ODR) configurable : 1 Hz → 1344 Hz
- Mode de fonctionnement configurable : high-resolution (12-bit), normal (10-bit), low-power (8-bit)
- Interface I²C (adresse par défaut `0x18`, secondaire `0x19`)
- Setup synchrone immédiat (pas de firmware à uploader, pas de machine d'états asynchrone)

---

## Configuration minimale

```yaml
external_components:
  - source: github://scorpix/ESPHome_components/components/LIS3DHTR

i2c:
  sda: GPIO21
  scl: GPIO22

sensor:
  - platform: lis3dh
    acceleration_x:
      name: "Accel X"
    acceleration_y:
      name: "Accel Y"
    acceleration_z:
      name: "Accel Z"
```

---

## Configuration complète

```yaml
sensor:
  - platform: lis3dh
    # Adresse I²C : 0x18 (SDO→GND, défaut) ou 0x19 (SDO→VDD)
    address: 0x18

    # Intervalle de publication des valeurs (PollingComponent)
    update_interval: 60s

    # Plage de mesure de l'accéléromètre
    # Options : 2g (défaut) | 4g | 8g | 16g
    accel_range: 2g

    # Fréquence d'échantillonnage hardware
    # Options : 1hz | 10hz | 25hz | 50hz | 100hz (défaut) | 200hz | 400hz | 1344hz
    accel_odr: 100hz

    # Mode de fonctionnement
    # Options : high_res (défaut, 12-bit) | normal (10-bit) | low_power (8-bit, conso minimale)
    operating_mode: high_res

    # Canaux (tous optionnels)
    acceleration_x:
      name: "Accel X"
    acceleration_y:
      name: "Accel Y"
    acceleration_z:
      name: "Accel Z"
    temperature:
      name: "IMU Temperature"
```

---

## Référence des options

### `accel_range`

| Valeur | Plage  | Résolution (HR, 12-bit) |
|--------|--------|-------------------------|
| `2g`   | ±2 g   | 1 mg/LSB                |
| `4g`   | ±4 g   | 2 mg/LSB                |
| `8g`   | ±8 g   | 4 mg/LSB                |
| `16g`  | ±16 g  | 12 mg/LSB               |

**Défaut :** `2g`

### `accel_odr`

Fréquence d'échantillonnage hardware. Indépendant de `update_interval` (qui contrôle uniquement la fréquence de publication vers Home Assistant).

| Valeur    | Fréquence  | Consommation typ. (HR) |
|-----------|------------|------------------------|
| `1hz`     | 1 Hz       | 2 µA                   |
| `10hz`    | 10 Hz      | 4 µA                   |
| `25hz`    | 25 Hz      | 6 µA                   |
| `50hz`    | 50 Hz      | 11 µA                  |
| `100hz`   | 100 Hz     | 22 µA                  |
| `200hz`   | 200 Hz     | 45 µA                  |
| `400hz`   | 400 Hz     | 83 µA                  |
| `1344hz`  | 1344 Hz    | ~185 µA                |

**Défaut :** `100hz`

### `operating_mode`

| Valeur       | Résolution | LPen | HR  | Usage recommandé            |
|--------------|------------|------|-----|-----------------------------|
| `high_res`   | 12-bit     | 0    | 1   | Précision maximale (défaut) |
| `normal`     | 10-bit     | 0    | 0   | Usage général               |
| `low_power`  | 8-bit      | 1    | 0   | Autonomie maximale          |

**Défaut :** `high_res`

> **Note :** En mode `low_power`, l'ODR `1344hz` n'est pas disponible (il est automatiquement limité à 400 Hz par le hardware).

---

## Câblage

| Broche LIS3DH | ESP32 (exemple) | Notes                        |
|---------------|-----------------|------------------------------|
| VDD           | 3.3 V           |                              |
| GND           | GND             |                              |
| SDA           | GPIO21          |                              |
| SCL           | GPIO22          |                              |
| SDO/SA0       | GND             | Adresse `0x18` (défaut)      |
| SDO/SA0       | 3.3 V           | Adresse `0x19` (alternative) |
| CS            | VDD ou NC       | Maintenir haut pour mode I²C |

---

## Notes techniques

- Le composant active le **BDU** (*Block Data Update*) par défaut : les registres de sortie ne sont mis à jour qu'après lecture complète des deux octets d'un axe, ce qui évite de lire un mot partiellement mis à jour.
- La lecture des 6 octets XYZ se fait en une seule transaction I²C grâce au bit d'auto-incrément d'adresse (bit 7 du registre d'adresse).
- Le capteur de température est **relatif** (offset fabrique à 25°C) et sa précision est de ±1°C typ. Il nécessite l'activation des bits `ADC_EN` et `TEMP_EN` dans `TEMP_CFG_REG` — ce composant le fait automatiquement si le canal `temperature` est déclaré.

---

## Licence

MIT
