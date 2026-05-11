# Bienestar Animal App

Prueba conceptual de una app para visualizar el analisis de bienestar de mascotas a partir de datos recolectados por un collar wearable.

## Que incluye

- Interfaz tipo app movil para consultar mascotas.
- Escenarios simulados de bienestar animal.
- Algoritmo de analisis de temperatura, actividad, sueño, comida, BLE, bateria, buffer y GNSS.
- Alertas explicadas para el usuario.
- Historial reciente por mascota.

## Estructura

```txt
src/data.js
Datos simulados: mascotas, escenarios e historial.

src/animalWellnessAnalysis.js
Algoritmo que interpreta los datos recolectados.

src/App.jsx
Interfaz principal de la app.

src/styles.css
Estilos visuales de la prueba conceptual.
```

## Ejecutar localmente

```bash
npm install
npm run dev
```

Luego abrir:

```txt
http://127.0.0.1:5173/
```

## Compilar

```bash
npm run build
```

## Nota

Esta app es una prueba conceptual. No realiza diagnosticos medicos ni reemplaza la evaluacion veterinaria.
