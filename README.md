# 🤖 Tanque ESP32 con Control Web & Radar Ultrasónico

Sistema de vehículo autónomo controlado por ESP32 con interfaz web interactiva, radar ultrasónico de 180° y prevención de colisiones en tiempo real.

![Estado del Proyecto](https://img.shields.io/badge/Estado-Funcional-brightgreen)
![Versión](https://img.shields.io/badge/Versión-1.0.0-blue)
![Licencia](https://img.shields.io/badge/Licencia-MIT-yellow)

## 📋 Descripción

Este proyecto implementa un vehículo tipo tanque con las siguientes características:

- **Control remoto vía WiFi** desde cualquier dispositivo
- **Radar ultrasónico panorámico** con visualización en tiempo real
- **Prevención de colisiones** automática
- **Arquitectura no bloqueante** (sin delays)
- **Interfaz web responsiva** con radar animado en HTML5 Canvas
- **Telemetría JSON** para integración con sistemas externos
- **Soporte MQTT** (opcional) para IoT

## ✨ Características Principales

### 🎮 Control del Vehículo
- ✅ Movimiento en 4 direcciones (adelante, atrás, izquierda, derecha)
- ✅ Botón de parada inmediata
- ✅ Sistema anti-colisión con detección de obstáculos
- ✅ Control PWM preciso de motores

### 📡 Radar Ultrasónico
- ✅ Barrido automático de 0° a 180°
- ✅ Detección de obstáculos hasta 400 cm
- ✅ Visualización gráfica en tiempo real
- ✅ Actualización cada 150ms

### 🌐 Interfaz Web
- ✅ Panel de control intuitivo
- ✅ Visualización de radar animado
- ✅ Estadísticas del sensor en vivo
- ✅ Responsive (móvil, tablet, PC)
- ✅ Estado de conexión en tiempo real

### ⚙️ Arquitectura
- ✅ Código modular y escalable
- ✅ Máquina de estados finitos (FSM)
- ✅ Ejecución no bloqueante
- ✅ Preparado para MQTT + Docker

## 🛠️ Hardware Requerido

| Componente | Cantidad | Función |
|------------|----------|---------|
| ESP32 DevKit V1 | 1 | Controlador principal |
| L298N | 1 | Driver de motores DC |
| Motores DC 3-12V | 2-4 | Tracción del vehículo |
| Servo SG90 | 1 | Barrido del radar |
| HC-SR04 | 1 | Sensor ultrasónico |
| Fuente 5V | 1 | Alimentación servo/sensor |
| Batería 18650 o Powerbank | 1 | Energía portátil |

### ⚡ Notas de Conexión
- **Importante:** El servo debe alimentarse con 5V externo
- **GND común:** Unir GND del ESP32, sensor, servo y motores
- **Evitar:** Sobrecargar el pin Vin del ESP32

## 📦 Instalación

### Requisitos Previos
- Arduino IDE 2.x o superior
- ESP32 Board Package v3.x
- Bibliotecas:
  - `ESP32Servo`
  - `WiFi` (incluida)
  - `WebServer` (incluida)
  - `PubSubClient` (opcional, para MQTT)

### Pasos de Instalación

1. **Clonar el repositorio**
```bash
git clone https://github.com/tu-usuario/tanque-esp32.git
cd tanque-esp32
```

2. **Configurar pines en `Config.h`**
```cpp
// Ajustar según tu cableado
#define PIN_MOTOR_IZQ_A 26
#define PIN_MOTOR_IZQ_B 27
// ... etc
```

3. **Configurar WiFi**
```cpp
#define SSID_AP "Tanque-123"
#define PASSWORD_AP "12345678"
```

4. **Compilar y subir**
- Seleccionar placa: ESP32 Dev Module
- Puerto: el correspondiente a tu ESP32
- Presionar "Upload"

5. **Conectarse al tanque**
- Conectar a la red WiFi "Tanque-123"
- Abrir navegador en `http://192.168.4.1`

## 📂 Estructura del Proyecto

```
/Codigo
├── Codigo.ino              # Loop principal
├── Config.h                # Configuración de pines y parámetros
├── Globals.h / .cpp        # Variables globales compartidas
├── CtrlRuedas.h / .cpp     # Control de motores
├── CtrlServo.h / .cpp      # Control del servo
├── SensorUltrasonico.h/.cpp# FSM del sensor ultrasónico
├── ServidorWeb.h / .cpp    # Servidor HTTP y HTML
├── ConexionWiFi.h / .cpp   # Gestión de red
└── ClienteMQTT.h / .cpp    # Cliente MQTT (opcional)
```

## 🔧 Configuración Avanzada

### PWM de Motores
```cpp
#define PWM_FREQ 5000
#define PWM_RESOLUTION 8
#define PWM_VELOCIDAD 200  // 0-255
```

### Radar
```cpp
#define SERVO_MIN 0
#define SERVO_MAX 180
#define TIEMPO_BARRIDO 2000  // ms
```

### Protección Anti-Colisión
```cpp
#define DISTANCIA_MINIMA 20  // cm
```

## 🌐 API REST

El servidor expone los siguientes endpoints:

| Endpoint | Método | Descripción |
|----------|--------|-------------|
| `/` | GET | Interfaz web principal |
| `/adelante` | GET | Avanzar |
| `/atras` | GET | Retroceder |
| `/izquierda` | GET | Girar izquierda |
| `/derecha` | GET | Girar derecha |
| `/parar` | GET | Detener motores |
| `/telemetria` | GET | JSON con ángulo y distancia |
| `/ping` | GET | Verificar conexión |

### Ejemplo de Telemetría
```json
{
  "angulo": 90,
  "distancia": 45,
  "timestamp": 1234567890
}
```

## 🐛 Solución de Problemas

### El servo hace ruido o vibra
- Verificar alimentación de 5V externa
- Separar GND de motores y servo
- Revisar conexión del pin de señal

### La interfaz web no carga
- Verificar conexión WiFi
- Comprobar IP en monitor serial
- Desactivar firewall temporalmente

### Los motores no responden
- Verificar configuración PWM
- Comprobar cables del L298N
- Revisar alimentación de motores

### Mediciones erráticas del sensor
- Verificar conexión trigger/echo
- Añadir capacitor 100µF en alimentación
- Alejar de fuentes de ruido eléctrico

## 🚀 Roadmap

- [x] Control básico de motores
- [x] Radar ultrasónico 180°
- [x] Interfaz web con canvas
- [x] Sistema anti-colisión
- [ ] Integración MQTT completa
- [ ] Backend Docker (Flask + MySQL)
- [ ] Control por voz
- [ ] Cámara streaming
- [ ] Modo autónomo con IA

## 🤝 Contribuciones

Las contribuciones son bienvenidas. Por favor:

1. Fork el proyecto
2. Crea una rama (`git checkout -b feature/nueva-funcionalidad`)
3. Commit tus cambios (`git commit -m 'Agregar nueva funcionalidad'`)
4. Push a la rama (`git push origin feature/nueva-funcionalidad`)
5. Abre un Pull Request

## 📄 Licencia

Este proyecto está bajo la Licencia MIT. Ver archivo `LICENSE` para más detalles.

## 👥 Autores

- Manuel Fina - *Desarrollo principal*
- Leonardo Renzi - *Colaborar y testing*
- Enzo Cura Sotelo - *Hardware*
- Agustin Ehyeramonho - *Documentacion*

## 🙏 Agradecimientos

- Proyecto desarrollado para EXPO ITES 2025
- Comunidad ESP32 por documentación y soporte
- Todos los colaboradores y testers

## 📧 Contacto

Para preguntas, sugerencias o reportes de bugs:
- Email: finamanuelmateo@gmail.com
- Issues: [GitHub Issues](https://github.com/tu-usuario/tanque-esp32/issues)

---

⭐ Si este proyecto te fue útil, considera darle una estrella en GitHub!
