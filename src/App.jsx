import React, { useEffect, useMemo, useState } from "react";
import { analyzeAnimalData } from "./animalWellnessAnalysis.js";
import { cats as starterCats, initialHistories, scenarios } from "./data.js";

function classNames(...values) {
  return values.filter(Boolean).join(" ");
}

function Button({ children, active = false, variant = "soft", ...props }) {
  return (
    <button
      className={classNames(
        "button",
        active && "buttonActive",
        variant === "primary" && "buttonPrimary"
      )}
      {...props}
    >
      {children}
    </button>
  );
}

function Panel({ children, className = "" }) {
  return <section className={classNames("panel", className)}>{children}</section>;
}

function Metric({ label, value, detail, tone = "neutral" }) {
  return (
    <Panel className="metric">
      <div className={classNames("metricMark", `tone-${tone}`)} />
      <p className="eyebrow">{label}</p>
      <p className="metricValue">{value}</p>
      <p className="muted">{detail}</p>
    </Panel>
  );
}

function EmptyState({ children }) {
  return <div className="emptyState">{children}</div>;
}

export default function App() {
  const [pets, setPets] = useState(() => {
    const savedPets = window.localStorage.getItem("animal-wellness-pets");
    return savedPets ? JSON.parse(savedPets) : starterCats;
  });
  const [selectedCat, setSelectedCat] = useState("luna");
  const [scenarioKey, setScenarioKey] = useState("normal");
  const [histories, setHistories] = useState(() => {
    const savedHistories = window.localStorage.getItem("animal-wellness-histories");
    return savedHistories ? JSON.parse(savedHistories) : initialHistories;
  });
  const [newPet, setNewPet] = useState({
    name: "",
    species: "Gato",
    age: "",
    note: "",
  });

  useEffect(() => {
    window.localStorage.setItem("animal-wellness-pets", JSON.stringify(pets));
  }, [pets]);

  useEffect(() => {
    window.localStorage.setItem(
      "animal-wellness-histories",
      JSON.stringify(histories)
    );
  }, [histories]);

  const cat = pets[selectedCat] || Object.values(pets)[0];
  const scenario = scenarios[scenarioKey];
  const currentData = scenario.data;
  const currentHistory = histories[cat.id] || [];

  const analysis = useMemo(
    () => analyzeAnimalData(currentData, currentHistory),
    [currentData, currentHistory]
  );

  const averages = useMemo(() => {
    const withCurrent = [...currentHistory, currentData];
    const average = (key) =>
      (
        withCurrent.reduce((acc, item) => acc + Number(item[key] || 0), 0) /
        withCurrent.length
      ).toFixed(1);

    return {
      temp: average("temp"),
      sleep: average("sleepHours"),
      play: average("playMinutes"),
      score:
        currentHistory.length > 0
          ? (
              currentHistory.reduce((acc, item) => acc + item.score, 0) /
              currentHistory.length
            ).toFixed(0)
          : analysis.score,
    };
  }, [analysis.score, currentData, currentHistory]);

  const addPet = (event) => {
    event.preventDefault();

    const trimmedName = newPet.name.trim();
    if (!trimmedName) return;

    const id = `${trimmedName.toLowerCase().replace(/[^a-z0-9]+/g, "-")}-${Date.now()}`;
    const avatar = newPet.species === "Perro" ? "🐶" : newPet.species === "Otro" ? "🐾" : "🐱";
    const pet = {
      id,
      name: trimmedName,
      avatar,
      species: newPet.species,
      age: newPet.age.trim() || "Edad no registrada",
      note: newPet.note.trim() || "Sin notas adicionales",
    };

    setPets((prev) => ({ ...prev, [id]: pet }));
    setHistories((prev) => ({ ...prev, [id]: [] }));
    setSelectedCat(id);
    setNewPet({ name: "", species: "Gato", age: "", note: "" });
  };

  const addCurrentRecord = () => {
    const newRecord = {
      time: currentData.updatedAt,
      temp: currentData.temp,
      movement: currentData.movement,
      sleepHours: currentData.sleepHours,
      meals: currentData.meals,
      playMinutes: currentData.playMinutes,
      mood: analysis.mood,
      state: analysis.generalState,
      score: analysis.score,
    };

    setHistories((prev) => ({
      ...prev,
      [cat.id]: [newRecord, ...(prev[cat.id] || [])].slice(0, 8),
    }));
  };

  const resetCurrentCatHistory = () => {
    setHistories((prev) => ({
      ...prev,
      [cat.id]: initialHistories[cat.id] || [],
    }));
  };

  return (
    <main className="appShell">
      <section className="workspace">
        <aside className="sidebar">
          <div>
            <p className="eyebrow">Mascotas</p>
            <h1>Bienestar animal</h1>
          </div>

          <div className="petList">
            {Object.values(pets).map((item) => (
              <button
                key={item.id}
                className={classNames("petButton", selectedCat === item.id && "selected")}
                onClick={() => setSelectedCat(item.id)}
              >
                <span className="avatar">{item.avatar}</span>
                <span>
                  <strong>{item.name}</strong>
                  <small>{item.age}</small>
                </span>
              </button>
            ))}
          </div>

          <form className="petForm" onSubmit={addPet}>
            <p className="sectionLabel">Registrar mascota</p>
            <input
              value={newPet.name}
              onChange={(event) => setNewPet((prev) => ({ ...prev, name: event.target.value }))}
              placeholder="Nombre"
              aria-label="Nombre de la mascota"
            />
            <div className="formGrid">
              <select
                value={newPet.species}
                onChange={(event) => setNewPet((prev) => ({ ...prev, species: event.target.value }))}
                aria-label="Especie"
              >
                <option>Gato</option>
                <option>Perro</option>
                <option>Otro</option>
              </select>
              <input
                value={newPet.age}
                onChange={(event) => setNewPet((prev) => ({ ...prev, age: event.target.value }))}
                placeholder="Edad"
                aria-label="Edad"
              />
            </div>
            <input
              value={newPet.note}
              onChange={(event) => setNewPet((prev) => ({ ...prev, note: event.target.value }))}
              placeholder="Nota de comportamiento"
              aria-label="Nota de comportamiento"
            />
            <Button variant="primary" type="submit">Agregar mascota</Button>
          </form>

        </aside>

        <section className="content">
          <header className="topbar">
            <div>
              <p className="eyebrow">Prueba conceptual</p>
              <h2>{cat.name}</h2>
              <p className="muted">
                {cat.species} · {cat.age} · {cat.note}
              </p>
            </div>
            <div className={classNames("connection", currentData.bleConnected ? "online" : "offline")}>
              <span />
              {currentData.bleConnected ? "BLE conectado" : "BLE desconectado"}
            </div>
          </header>

          <section className="hero">
            <div className="heroCopy">
              <p className="eyebrow">Estado general</p>
              <h3>{analysis.generalState}</h3>
              <p>{analysis.recommendation}</p>
            </div>
            <div className="scoreDial" aria-label={`Puntaje ${analysis.score} de 100`}>
              <strong>{analysis.score}</strong>
              <span>/100</span>
            </div>
          </section>

          <section className="metricsGrid">
            <Metric
              label="Temperatura"
              value={`${currentData.temp} C`}
              detail={`Promedio: ${averages.temp} C`}
              tone="red"
            />
            <Metric
              label="Movimiento"
              value={analysis.activityLabel}
              detail={`IMU RMS: ${currentData.movement}`}
              tone="green"
            />
            <Metric
              label="Sueno"
              value={`${currentData.sleepHours} h`}
              detail={analysis.sleepTrend}
              tone="blue"
            />
            <Metric
              label="Juego"
              value={`${currentData.playMinutes} min`}
              detail={analysis.playTrend}
              tone="yellow"
            />
          </section>

          <div className="twoColumn">
            <Panel>
              <div className="panelHeader">
                <div>
                  <p className="eyebrow">Analisis de datos</p>
                  <h3>Alertas explicadas</h3>
                </div>
                <span className="countBadge">{analysis.alerts.length}</span>
              </div>

              {analysis.alerts.length === 0 ? (
                <EmptyState>No se detectan alertas importantes en este escenario.</EmptyState>
              ) : (
                <div className="stack">
                  {analysis.alerts.map((alert) => (
                    <article className={classNames("alertItem", `alert-${alert.type}`)} key={alert.title}>
                      <strong>{alert.title}</strong>
                      <p>{alert.detail}</p>
                      <small>{alert.reason}</small>
                    </article>
                  ))}
                </div>
              )}
            </Panel>

            <Panel>
              <div className="panelHeader">
                <div>
                  <p className="eyebrow">Collar wearable</p>
                  <h3>Estado tecnico</h3>
                </div>
                <span className="battery">{currentData.battery}%</span>
              </div>

              <div className="deviceGrid">
                <div>
                  <span>Modo</span>
                  <strong>{currentData.deviceMode}</strong>
                </div>
                <div>
                  <span>Buffer</span>
                  <strong>{currentData.bufferRecords} registros</strong>
                </div>
                <div>
                  <span>GNSS</span>
                  <strong>{currentData.gnssActive ? "Activo" : "Inactivo"}</strong>
                </div>
                <div>
                  <span>Actualizado</span>
                  <strong>{currentData.updatedAt}</strong>
                </div>
              </div>

              <div className="stack compact">
                {analysis.technicalEvents.length === 0 ? (
                  <EmptyState>Sin eventos tecnicos relevantes.</EmptyState>
                ) : (
                  analysis.technicalEvents.map((event) => (
                    <article className="techItem" key={event.title}>
                      <strong>{event.title}</strong>
                      <p>{event.detail}</p>
                    </article>
                  ))
                )}
              </div>
            </Panel>
          </div>

          <div className="twoColumn">
            <Panel>
              <div className="panelHeader">
                <div>
                  <p className="eyebrow">Basal individual</p>
                  <h3>Comparacion historica</h3>
                </div>
              </div>
              <div className="baselineGrid">
                <div>
                  <span>Temp. basal</span>
                  <strong>{analysis.baselines.temp} C</strong>
                </div>
                <div>
                  <span>Actividad basal</span>
                  <strong>{analysis.baselines.movement}</strong>
                </div>
                <div>
                  <span>Sueno basal</span>
                  <strong>{analysis.baselines.sleepHours} h</strong>
                </div>
                <div>
                  <span>Juego basal</span>
                  <strong>{analysis.baselines.playMinutes} min</strong>
                </div>
              </div>
              <div className="insights">
                {analysis.insights.map((insight) => (
                  <p key={insight}>{insight}</p>
                ))}
              </div>
            </Panel>

            <Panel>
              <div className="panelHeader">
                <div>
                  <p className="eyebrow">Resumen de hoy</p>
                  <h3>Datos recolectados</h3>
                </div>
              </div>
              <div className="summaryList">
                <p>Comidas: {currentData.ate ? currentData.meals : "No ha comido"}</p>
                <p>Inactividad: {currentData.inactiveHours} h</p>
                <p>Actividad continua: {currentData.continuousActivityMinutes} min</p>
                <p>BLE sin conexion: {currentData.bleDisconnectedSeconds} s</p>
              </div>
            </Panel>
          </div>

          <Panel>
            <div className="panelHeader">
              <div>
                <p className="eyebrow">Historial reciente</p>
                <h3>{cat.name}</h3>
              </div>
              <div className="historyStats">
                <span>{currentHistory.length} registros</span>
                <span>Score medio {averages.score}</span>
              </div>
            </div>

            <div className="historyList">
              {currentHistory.map((item, index) => (
                <article key={`${item.time}-${index}`} className="historyItem">
                  <div>
                    <strong>{item.time}</strong>
                    <p>{item.temp} C · {item.sleepHours} h sueno · {item.playMinutes} min juego</p>
                  </div>
                  <div>
                    <strong>{item.score}/100</strong>
                    <p>{item.state}</p>
                  </div>
                </article>
              ))}
            </div>
          </Panel>

          <section className="scenarioBlock">
            <p className="sectionLabel">Probar escenarios</p>
            <div className="scenarioGrid">
              {Object.entries(scenarios).map(([key, item]) => (
                <Button
                  key={key}
                  active={scenarioKey === key}
                  onClick={() => setScenarioKey(key)}
                >
                  {item.label}
                </Button>
              ))}
            </div>
          </section>

          <div className="actions">
            <Button variant="primary" onClick={addCurrentRecord}>
              Guardar registro
            </Button>
            <Button onClick={resetCurrentCatHistory}>Reiniciar historial</Button>
          </div>
        </section>
      </section>
    </main>
  );
}
