# Simulacion Wokwi - Firmware collar gato

Esta carpeta contiene la prueba de concepto del firmware para el collar de bienestar animal. La simulacion usa PlatformIO + Arduino sobre ESP32 en Wokwi para validar la logica antes de llevarla al firmware real del ESP32-C3.

## Componentes simulados

- ESP32 DevKit para ejecutar la prueba en Wokwi.
- IMU MPU6050 como equivalente funcional del BMI270.
- Sensor NTC como equivalente funcional del TMP117.
- Potenciometro para simular porcentaje de bateria.
- LED BLE para estado de conexion.
- LED GNSS para ubicacion bajo demanda.
- LED Actividad para evento de alta actividad.
- Botones para forzar eventos: IMU Wake, BLE perdido, temperatura alta, inactividad y bateria baja.

## Logica principal

Cada ciclo lee sensores, calcula alertas acumulables y genera un paquete JSON para analisis. Los eventos no son excluyentes: temperatura alta, inactividad, bateria baja, alta actividad y perdida BLE pueden aparecer juntos en el mismo paquete.

El GNSS solo se activa cuando existe perdida BLE mayor al umbral configurado. Si BLE esta conectado, el paquete se envia por `ble_notification`. Si BLE esta perdido, el paquete se guarda en buffer local y se imprime como `local_buffer`.

## Ejecutar

```bash
pio run
```

Luego abrir Wokwi desde VS Code. El monitor debe mostrar el paquete JSON que se enviaria a la app para analisis.

## Estados y umbrales

- Muestreo periodico: cada 5 segundos en simulacion.
- Inactividad: alerta desde 1 hora.
- Perdida BLE: alerta desde mas de 30 segundos.
- Bateria baja: alerta desde 20% o menos.
- Evento termico: diferencia de temperatura desde 2 C contra la linea base.
- Alta actividad: actividad RMS alta o actividad continua mayor al umbral configurado.
