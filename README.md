# Bienestar Animal

Repositorio del proyecto de bienestar animal para un collar inteligente de gato.

## Estructura

```text
bienestar-animal-app/
├── app/
│   └── Interfaz de usuario para visualizar mascotas, alertas y analisis.
└── firmware-collar-gato/
    └── Simulacion Wokwi del firmware del collar.
```

## App

La aplicacion web esta en `app/`.

```bash
cd app
npm install
npm run dev
```

## Firmware Wokwi

La prueba de concepto del firmware esta en `firmware-collar-gato/`.

```bash
cd firmware-collar-gato
pio run
```

El monitor serial de Wokwi muestra el paquete JSON que se enviaria a la app para analisis.
