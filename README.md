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

## Desplegar en Vercel

1. Entrar a https://vercel.com/new
2. Importar el repositorio `ma-prietoo/bienestar-animal-app`
3. Framework Preset: `Vite`
4. Build Command: `npm run build`
5. Output Directory: `dist`
6. Deploy

Vercel publicara la app en un dominio HTTPS. Desde ese enlace los usuarios podran abrirla en el navegador e instalarla como app PWA cuando el navegador muestre la opcion de instalacion.

## App instalable

La app incluye:

- `public/manifest.webmanifest`
- `public/sw.js`
- `public/icon.svg`

Estos archivos permiten que la experiencia funcione como PWA instalable cuando se publica en HTTPS.

## Nota

Esta app es una prueba conceptual. No realiza diagnosticos medicos ni reemplaza la evaluacion veterinaria.
