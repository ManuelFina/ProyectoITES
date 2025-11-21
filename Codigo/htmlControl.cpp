#include "htmlControl.h"

String generarHTML() {
  return String(R"HTML(
<!doctype html>
<html lang="es">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Control del Tanque</title>
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
    min-height: 100vh;
    background: radial-gradient(circle at top, #141d35, #050711);
    color: var(--fg);
    font-family: system-ui, sans-serif;
    display: flex;
    align-items: center;
    justify-content: center;
    padding: 16px;
  }

  .card {
    background: var(--panel);
    border-radius: 16px;
    padding: 24px 28px;
    box-shadow: 0 18px 40px rgba(0,0,0,0.55);
    max-width: 420px;
    width: 100%;
    text-align: center;
    border: 1px solid #1c2840;
  }

  h1 {
    font-size: 26px;
    margin-bottom: 8px;
    color: var(--accent);
  }

  .subtitle {
    font-size: 13px;
    color: #9ca3af;
    margin-bottom: 18px;
  }

  .status {
    font-size: 13px;
    margin-bottom: 18px;
    color: #b0b4c0;
  }

  .status span {
    font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace;
    font-size: 12px;
  }

  .pad {
    display: grid;
    grid-template-areas:
      ". up ."
      "left stop right"
      ". down .";
    gap: 10px;
    justify-content: center;
  }

  button {
    width: 90px;
    height: 80px;
    border-radius: 12px;
    border: none;
    font-size: 26px;
    background: var(--btn);
    color: var(--fg);
    cursor: pointer;
    transition: transform 0.12s ease, box-shadow 0.12s ease, background 0.12s ease;
    box-shadow: 0 8px 18px rgba(0,0,0,0.45);
  }

  button:hover {
    background: var(--btn-hover);
    transform: translateY(-2px);
    box-shadow: 0 12px 26px rgba(0,0,0,0.55);
  }

  button:active {
    transform: translateY(0);
    box-shadow: 0 6px 12px rgba(0,0,0,0.35);
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

  .hint {
    margin-top: 18px;
    font-size: 11px;
    color: #6b7280;
  }
</style>
</head>
<body>
  <div class="card">
    <h1>Control del Tanque</h1>
    <div class="subtitle">Interfaz web embebida en el ESP32</div>

    <div class="status">
      IP del tanque: <span id="ip">—</span>
    </div>

    <div class="pad">
      <button class="up" onclick="sendCmd('adelante')">▲</button>
      <button class="left" onclick="sendCmd('izquierda')">◀</button>
      <button class="stop" onclick="sendCmd('parar')">■</button>
      <button class="right" onclick="sendCmd('derecha')">▶</button>
      <button class="down" onclick="sendCmd('atras')">▼</button>
    </div>

    <div class="hint">
      Usá estos controles mientras estés en la misma red WiFi que el tanque.
    </div>
  </div>

<script>
  function sendCmd(cmd) {
    fetch('/' + cmd).catch(() => {});
  }

  document.getElementById('ip').textContent = location.hostname || 'esp32-tanque';
</script>
</body>
</html>
)HTML");
}
