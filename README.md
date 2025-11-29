# Detector Acústico de Apnea Neonatal

## Descripción
Este proyecto consiste en un sistema de detección de apnea neonatal que utiliza un **micrófono digital INMP441** y un **sensor piezoeléctrico** para monitorear la respiración de un recién nacido.  
Cuando se detecta apnea (falta de respiración por más de 15 segundos), se activa un **buzzer** como alerta.  
El sistema utiliza **ESP32** y la comunicación **ESP-NOW** para enviar los datos a un monitor remoto.

---

## Diagrama de Estados (FSM)


> Este diagrama muestra los estados de los sensores (Micrófono y Piezo) y la activación del buzzer.

---

## Tecnologías Utilizadas

- **Hardware**:
  - ESP32
  - Micrófono digital INMP441
  - Sensor piezoeléctrico
  - Buzzer
- **Software**:
  - Arduino IDE
  - FreeRTOS (tareas independientes)
  - ESP-NOW (comunicación inalámbrica)
  - C++ modularizado (`main.ino`, `funciones.h`, `funciones.cpp`)

---

## Integrantes y Roles
 Marcia Katherine Santa María G. Desarrollo de FSM y FreeRTOS, lectura de sensores 

---

## Estado Actual del Proyecto

_ Lectura de micrófono INMP441 implementada  
_ Lectura de sensor piezoeléctrico implementada  
- FSM para micrófono y piezo completada  
- FSM para buzzer completada  
_ Modularización del código (`main.ino`, `funciones.h`, `funciones.cpp`)  
_ Implementación de FreeRTOS con tareas independientes  
_ Integración final con ESP-NOW para comunicación con monitor  
_ Pruebas en tiempo real con incubadora o simulador de respiración  
_ Documentación y presentación final  


## Carpeta `img/`


