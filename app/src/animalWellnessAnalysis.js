export function analyzeAnimalData(data, history = []) {
  if (!data || typeof data !== "object") {
    throw new Error("Los datos de la mascota no son validos.");
  }

  const alerts = [];
  const insights = [];
  const technicalEvents = [];
  let score = 100;

  const avg = (key, fallback) => {
    const values = history
      .map((item) => item[key])
      .filter((value) => typeof value === "number" && Number.isFinite(value));

    if (values.length === 0) return fallback;
    return values.reduce((acc, value) => acc + value, 0) / values.length;
  };

  const baselineTemp = avg("temp", data.temp);
  const baselineActivity = avg("movement", data.movement);
  const baselineSleep = avg("sleepHours", data.sleepHours);
  const baselinePlay = avg("playMinutes", data.playMinutes);

  const activityDelta =
    baselineActivity > 0
      ? ((data.movement - baselineActivity) / baselineActivity) * 100
      : 0;

  if (data.temp > 39.5) {
    alerts.push({
      type: "health",
      title: "Temperatura elevada",
      detail: "La temperatura superficial esta por encima del rango esperado.",
      reason: `Lectura actual: ${data.temp} C.`,
    });
    score -= 30;
  } else if (data.temp < 37) {
    alerts.push({
      type: "health",
      title: "Temperatura baja",
      detail: "La temperatura superficial esta por debajo del rango esperado.",
      reason: `Lectura actual: ${data.temp} C.`,
    });
    score -= 30;
  } else if (Math.abs(data.temp - baselineTemp) >= 2) {
    alerts.push({
      type: "trend",
      title: "Cambio termico frente al basal",
      detail: "La temperatura cambio de forma importante frente a su historial.",
      reason: `Basal: ${baselineTemp.toFixed(1)} C. Actual: ${data.temp} C.`,
    });
    score -= 20;
  }

  if (!data.ate || data.meals === 0) {
    alerts.push({
      type: "care",
      title: "No ha comido hoy",
      detail: "El registro de alimentacion indica ausencia de comidas.",
      reason: "La app recibio 0 comidas para el dia.",
    });
    score -= 25;
  } else if (data.meals === 1) {
    alerts.push({
      type: "care",
      title: "Ha comido menos de lo habitual",
      detail: "Conviene observar si mantiene bajo apetito.",
      reason: "Solo se registro una comida.",
    });
    score -= 10;
  }

  if (data.sleepHours < 8) {
    alerts.push({
      type: "behavior",
      title: "Ha dormido poco",
      detail: "El descanso esta por debajo de lo esperado.",
      reason: `${data.sleepHours} horas de sueno registradas.`,
    });
    score -= 10;
  } else if (data.sleepHours > 18) {
    alerts.push({
      type: "behavior",
      title: "Esta durmiendo mas de lo habitual",
      detail: "El descanso prolongado puede indicar cambio de comportamiento.",
      reason: `${data.sleepHours} horas de sueno registradas.`,
    });
    score -= 12;
  }

  if (data.playMinutes < 10 && data.movement < 0.1) {
    alerts.push({
      type: "behavior",
      title: "Muy poca actividad y juego",
      detail: "La actividad fisica esta baja y casi no hay juego registrado.",
      reason: `Movimiento: ${data.movement}. Juego: ${data.playMinutes} min.`,
    });
    score -= 15;
  }

  if (data.inactiveHours >= 6) {
    alerts.push({
      type: "behavior",
      title: "Inactividad prolongada",
      detail: "El collar detecto varias horas sin movimiento significativo.",
      reason: `${data.inactiveHours} horas de baja actividad.`,
    });
    score -= 20;
  }

  if (data.continuousActivityMinutes >= 10 || data.movement >= 0.8) {
    technicalEvents.push({
      title: "Actividad intensa detectada",
      detail: "La IMU detecto un periodo continuo de alta actividad.",
    });
    score -= 5;
  }

  if (Math.abs(activityDelta) >= 30) {
    alerts.push({
      type: "trend",
      title: "Cambio abrupto de actividad",
      detail: "La actividad cambio mas del 30% frente a su patron habitual.",
      reason: `Variacion estimada: ${activityDelta.toFixed(0)}%.`,
    });
    score -= 15;
  }

  if (
    typeof data.heartRateBpm === "number" &&
    typeof data.baselineHeartRateBpm === "number"
  ) {
    const heartRateDelta =
      ((data.heartRateBpm - data.baselineHeartRateBpm) /
        data.baselineHeartRateBpm) *
      100;

    if (Math.abs(heartRateDelta) >= 20) {
      alerts.push({
        type: "health",
        title: "Frecuencia cardiaca fuera del basal",
        detail: "La frecuencia cardiaca cambio de forma relevante.",
        reason: `Variacion estimada: ${heartRateDelta.toFixed(0)}%.`,
      });
      score -= 20;
    }
  }

  if (data.bleConnected === false && data.bleDisconnectedSeconds > 30) {
    alerts.push({
      type: "safety",
      title: "Perdida de conexion BLE",
      detail: "El collar pudo salir de la zona segura o perder conexion.",
      reason: `${data.bleDisconnectedSeconds} segundos sin conexion.`,
    });
    technicalEvents.push({
      title: "Buffer y ubicacion",
      detail: "Se almacenan datos localmente y se habilita GNSS si el evento persiste.",
    });
    score -= 10;
  }

  if (data.battery <= 20) {
    alerts.push({
      type: "device",
      title: "Bateria baja",
      detail: "Carga el collar pronto para mantener el monitoreo.",
      reason: `Bateria actual: ${data.battery}%.`,
    });
    score -= 5;
  }

  score = Math.max(0, Math.min(100, score));

  let generalState = "Normal";
  if (score < 85 && score >= 65) generalState = "Observar";
  if (score < 65) generalState = "Atencion requerida";

  let activityLabel = "Actividad normal";
  if (data.movement < 0.08) activityLabel = "Muy baja actividad";
  else if (data.movement < 0.3) activityLabel = "Actividad ligera";
  else if (data.movement < 0.8) activityLabel = "Actividad normal";
  else activityLabel = "Muy activo";

  let mood = "Tranquilo";
  if (data.temp > 39.5 || (!data.ate && data.playMinutes < 10)) {
    mood = "Decaido";
  } else if (data.playMinutes >= 60 && data.meals >= 2) {
    mood = "Feliz y jugueton";
  } else if (data.sleepHours > 18 && data.playMinutes < 10) {
    mood = "Somnoliento";
  } else if (data.playMinutes >= 20 && data.sleepHours >= 10 && data.meals >= 2) {
    mood = "Relajado";
  }

  const sleepTrend =
    data.sleepHours > baselineSleep + 2
      ? "Durmio mas de lo habitual"
      : data.sleepHours < baselineSleep - 2
        ? "Durmio menos de lo habitual"
        : "Sueno dentro de lo esperado";

  const playTrend =
    data.playMinutes > baselinePlay + 15
      ? "Jugo mas de lo habitual"
      : data.playMinutes < baselinePlay - 15
        ? "Jugo menos de lo habitual"
        : "Juego dentro de lo esperado";

  if (Math.abs(data.temp - baselineTemp) < 2) {
    insights.push("La temperatura esta cerca del patron historico.");
  }

  insights.push(sleepTrend);
  insights.push(playTrend);

  const recommendation =
    generalState === "Normal"
      ? "Tu mascota se encuentra estable. Los indicadores actuales no muestran senales importantes de malestar."
      : generalState === "Observar"
        ? "Se detectan cambios respecto al comportamiento esperado. Observa su evolucion durante el dia."
        : "Se detectan senales que podrian indicar malestar o riesgo. Observa con atencion y considera consultar con un veterinario si el comportamiento persiste.";

  return {
    score,
    generalState,
    recommendation,
    mood,
    activityLabel,
    alerts,
    insights,
    technicalEvents,
    sleepTrend,
    playTrend,
    baselines: {
      temp: Number(baselineTemp.toFixed(1)),
      movement: Number(baselineActivity.toFixed(2)),
      sleepHours: Number(baselineSleep.toFixed(1)),
      playMinutes: Number(baselinePlay.toFixed(0)),
    },
  };
}
