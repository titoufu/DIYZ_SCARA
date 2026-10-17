# DIYZ — Fase 0 — v0.0.7: diagnóstico granular da Serial

Base: v0.0.6b. Compatibilidade mantida para NVS e comandos existentes. Não controla atuadores.

## Mudanças
- Comando `SERTRACE`: registra tempo máximo de `Serial.available()`, `Serial.read()` e processamento de linha/resposta (`DISPATCH`).
- Distingue consultas sem entrada (EMPTY) e consultas com entrada (BUSY); registra horário em uptime, núcleo e a etapa mais lenta.
- Não há `vTaskDelay(1)` experimental nesta versão.
- Medições por relógio de parede incluem eventuais preempções FreeRTOS; **não** provam CPU ocupada na etapa indicada.
- Monitoramento mínimo em fase ociosa; a instrumentação adiciona chamadas a `micros()` e poderá alterar levemente a temporização.

## Ensaios propostos
1. Clean, Build, Upload no PlatformIO. COM5, 115200, USB-to-UART. Placa sozinha.
2. `VERSION`, `STATUS`, `CONFIG`, `SERTRACE`, `TRACE`, `DIAG`.
3. Reiniciar a placa, deixar 20 a 30 minutos **sem enviar comandos**; depois `SERTRACE`, `TRACE`, `DIAG`.
4. Um `MAX_POLL_US` grande com `POLL_INPUT=0` e `AVAILABLE_MAX_US` alto indica tempo transcorrido durante `Serial.available()` mesmo sem dados. Isso **não distingue** uma função bloqueante de preempção FreeRTOS.
5. Guardar o log bruto para decidir o próximo teste; não repetir gravações `CONFIGSAVE` desnecessariamente.

## Critérios
Apenas instrumentação; a causa dos atrasos de ~63 ms ainda não está demonstrada. Não habilitar motores ou servo.
