#include "ServidorWeb.h"
#include "Globals.h"
#include "CtrlRuedas.h"
#include <Arduino.h>
void ConfigurarServidorWeb() {
  servidorHTTP.on("/", manejadorRaiz);

  servidorHTTP.on("/adelante", [](){
    movimientoActual = ADELANTE;
    servidorHTTP.send(200, "text/plain", "OK");
  });
  servidorHTTP.on("/atras", [](){
    movimientoActual = ATRAS;
    servidorHTTP.send(200, "text/plain", "OK");
  });
  servidorHTTP.on("/izquierda", [](){
    movimientoActual = IZQ;
    servidorHTTP.send(200, "text/plain", "OK");
  });
  servidorHTTP.on("/derecha", [](){
    movimientoActual = DER;
    servidorHTTP.send(200, "text/plain", "OK");
  });
  servidorHTTP.on("/parar", [](){
    movimientoActual = PARADO;
    servidorHTTP.send(200, "text/plain", "OK");
  });

  servidorHTTP.on("/ping", [](){
    servidorHTTP.send(200, "text/plain", "pong");
  });

  servidorHTTP.begin();
  Serial.println("[HTTP] Servidor iniciado");

  servidorHTTP.on("/telemetria", [](){
    long distancia = distanciaCm; 
    int  angulo = anguloActual;
    char json[64];
    snprintf(json, sizeof(json), "{\"ang\":%d,\"dist\":%ld}", a, d);
    servidorHTTP.send(200, "application/json", json);
  });
}

void ActualizarServidorWeb() {
  servidorHTTP.handleClient();
}

void manejadorRaiz() {
  servidorHTTP.send(200, "text/html", generarHTML());
}

String generarHTML() {
  return String(R"HTML(
<!doctype html>
<html lang="es">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Panel de Control del Tanque</title>
<style>
  :root {
    --bg: #0b1220;
    --fg: #ffffff;
    --accent: #00bf63;
    --panel: #121a2b;
    --btn: #1b2741;
    --btn-hover: #24385d;
    --stop: #3a1c1c;
    --stop-hover: #4b2525;
  }

  * { box-sizing: border-box; margin: 0; padding: 0; }

  body {
    height: 100vh;
    background: var(--bg);
    color: var(--fg);
    font-family: system-ui, sans-serif;
    display: flex;
    flex-direction: row;
  }

  /* ==== PANEL DE CONTROL ==== */
  .control {
    width: 50%;
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    background: var(--panel);
    border-right: 2px solid #18223a;
    padding: 20px;
  }

  h1 {
    font-size: 28px;
    margin-bottom: 10px;
    color: var(--accent);
  }

  .status {
    font-size: 14px;
    margin-bottom: 15px;
    color: #aaa;
  }

  .pad {
    display: grid;
    grid-template-areas:
      ". up ."
      "left stop right"
      ". down .";
    gap: 10px;
  }

  button {
    width: 90px;
    height: 80px;
    border-radius: 10px;
    border: none;
    font-size: 26px;
    background: var(--btn);
    color: var(--fg);
    cursor: pointer;
    transition: 0.15s;
  }

  button:hover {
    background: var(--btn-hover);
  }

  .stop {
    background: var(--stop);
  }

  .stop:hover {
    background: var(--stop-hover);
  }

  .up { grid-area: up; }
  .down { grid-area: down; }
  .left { grid-area: left; }
  .right { grid-area: right; }
  .stop { grid-area: stop; }

  .telemetria {
    margin-top: 20px;
    font-size: 18px;
  }

  /* ==== RADAR ==== */
  .radar-container {
    width: 50%;
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
  }

  canvas {
    width: 100%;
    height: auto;
    max-width: 600px;
    aspect-ratio: 2/1;
  }

  h2 {
    margin-bottom: 10px;
    font-size: 22px;
  }
</style>
</head>
<body>

  <!-- Panel de Control -->
  <div class="control">
    <h1>Control del Tanque</h1>
    <div class="status">
      IP: <span id="ip">—</span> | Estado: <span id="estado">Conectando...</span>
    </div>

    <div class="pad">
      <button class="up" onclick="sendCmd('adelante')">▲</button>
      <button class="left" onclick="sendCmd('izquierda')">◀</button>
      <button class="stop" onclick="sendCmd('parar')">■</button>
      <button class="right" onclick="sendCmd('derecha')">▶</button>
      <button class="down" onclick="sendCmd('atras')">▼</button>
    </div>

    <div class="telemetria">
      Ángulo: <span id="ang">—°</span> | Distancia: <span id="dist">— cm</span>
    </div>
  </div>

  <!-- Radar -->
  <div class="radar-container">
    <h2>Radar Ultrasónico (180°)</h2>
    <canvas id="radar" width="600" height="300"></canvas>
  </div>

<script>
  const radar = document.getElementById('radar');
  const ctx = radar.getContext('2d');
  const MAX_RANGE = 200;
  let lastData = { ang: 0, dist: -1 };

  function sendCmd(cmd) {
    fetch('/' + cmd).catch(() => {});
  }

  // Dibuja el radar base (semicírculo)
  function drawBase() {
    const w = radar.width, h = radar.height;
    ctx.clearRect(0, 0, w, h);
    const cx = w / 2, cy = h;
    const r = h - 10;

    // Fondo
    const grad = ctx.createRadialGradient(cx, cy, 10, cx, cy, r);
    grad.addColorStop(0, '#0b1220');
    grad.addColorStop(1, '#08101d');
    ctx.fillStyle = grad;
    ctx.fillRect(0, 0, w, h);

    // Arcos concéntricos
    ctx.strokeStyle = '#1f334d';
    ctx.lineWidth = 2;
    for (let i = 1; i <= 4; i++) {
      ctx.beginPath();
      ctx.arc(cx, cy, (r * i) / 4, Math.PI, 2 * Math.PI);
      ctx.stroke();
    }

    // Líneas cada 30°
    ctx.strokeStyle = '#26466d';
    ctx.lineWidth = 1;
    for (let a = 0; a <= 180; a += 30) {
      const rad = (Math.PI * a) / 180;
      const x = cx + r * Math.cos(rad);
      const y = cy - r * Math.sin(rad);
      ctx.beginPath();
      ctx.moveTo(cx, cy);
      ctx.lineTo(x, y);
      ctx.stroke();
    }
  }

  function drawSweepAndObject() {
    const w = radar.width, h = radar.height;
    const cx = w / 2, cy = h;
    const r = h - 10;

    // Línea de barrido
    const angRad = (Math.PI * lastData.ang) / 180;
    const x = cx + r * Math.cos(angRad);
    const y = cy - r * Math.sin(angRad);
    const grad = ctx.createLinearGradient(cx, cy, x, y);
    grad.addColorStop(0, '#00bf63aa');
    grad.addColorStop(1, '#00bf6300');
    ctx.strokeStyle = grad;
    ctx.lineWidth = 4;
    ctx.beginPath();
    ctx.moveTo(cx, cy);
    ctx.lineTo(x, y);
    ctx.stroke();

    // Dibuja punto si hay obstáculo
    if (lastData.dist >= 0) {
      const d = Math.min(lastData.dist, MAX_RANGE);
      const rr = (d / MAX_RANGE) * r;
      const ox = cx + rr * Math.cos(angRad);
      const oy = cy - rr * Math.sin(angRad);
      ctx.fillStyle = '#00bf63';
      ctx.beginPath();
      ctx.arc(ox, oy, 6, 0, 2 * Math.PI);
      ctx.fill();
    }
  }

  function animate() {
    drawBase();
    drawSweepAndObject();
    requestAnimationFrame(animate);
  }
  animate();

  // Actualizar telemetría desde el ESP32
  async function poll() {
    try {
      const r = await fetch('/telemetria', { cache: 'no-store' });
      if (!r.ok) throw new Error(r.status);
      const j = await r.json();
      lastData = j;
      document.getElementById('ang').textContent = j.ang + "°";
      document.getElementById('dist').textContent = j.dist + " cm";
      document.getElementById('estado').textContent = "Conectado";
    } catch {
      document.getElementById('estado').textContent = "Sin conexión";
    } finally {
      setTimeout(poll, 150);
    }
  }

  document.getElementById('ip').textContent = location.hostname;
  poll();
</script>
</body>
</html>
)HTML");
}

