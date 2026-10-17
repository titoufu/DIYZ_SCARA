# DIYZ — Fase 0, firmware 0.0.5

## Alterações

- Preservados os comandos e o formato das respostas existentes da v0.0.4.
- `DIAG`: novas métricas `<ROTINA>_LATE_EPISODES` (execuções com ao menos um período ignorado) e `<ROTINA>_MAX_SKIPPED` (maior número de períodos ignorados numa execução).
- `DIAG`: `LOOP_MAX_GAP_US`, `LOOP_GAP_GT5MS`, `LOOP_GAP_GT10MS`, `INJECTED_TESTS`. Medem o intervalo entre entradas sucessivas no `loop()`, incluindo bloqueios de comunicação.
- `DIAGTEST`: **induz deliberadamente 25 ms de pausa**, apenas para testar a instrumentação. NÃO deve ser utilizado com atuadores conectados nem nas fases de movimento. Esse comando e suas estatísticas não comprovam comportamento em tempo real.
- O agendador continua cooperativo e não faz catch-up: períodos ultrapassados são ignorados e contados.

## Semântica dos contadores

- `RUN`: número de chamadas liberadas pelo gate.
- `MISS`: soma de ticks programados que foram ignorados; um único bloqueio pode produzir vários MISS.
- `LATE_RUN`: chamadas iniciadas com pelo menos 1 us de atraso (qualquer atraso, mesmo sem perda de período).
- `LATE_EPISODES`: chamadas cujo atraso foi >= um período inteiro. Não é detector de causas externas.
- `JITTER_US`: maior atraso em relação ao próximo instante programado (medido no início da passagem pelo loop, não no início exato do callback).
- `INTERVAL_MIN/MAX_US`: intervalos entre passagens do gate autorizadas, não medem duração da tarefa.
- `EXEC_US`: tempo da rotina de teste vazia; não mede a duração total do loop.
- `LOOP_MAX_GAP_US`: maior intervalo entre amostragens no topo do loop.

## Procedimento de bancada

1. Usar **somente o ESP32-S3 via USB-to-UART**, sem fonte de potência ou atuadores conectados.
2. Verificar `VERSION`, `STATUS`, `CONFIG`, `DIAG` antes de alterações.
3. Enviar `DIAGTEST` uma vez. Esperar `OK CMD=DIAGTEST STALL_MS=25 NOTE=ARTIFICIAL`.
4. Enviar `DIAG`: esperar `INJECTED_TESTS=1`, `LOOP_MAX_GAP_US` tipicamente >= 25000 e aumentos de `MISS`/`LATE_EPISODES` em uma ou mais rotinas. Os números exatos dependem da fase do escalonador e do tempo real de pausa.
5. Reiniciar o ESP32-S3 e verificar que `INJECTED_TESTS=0` e NVS se mantém `OK`.
6. Sem `DIAGTEST`, medir em ~30 s e em >=10 min; registrar todos os campos do `DIAG`.

## Limitações / aceite

- Testes de parser/aritmética temporal executados localmente em C++17.
- Compilação PlatformIO, upload e ensaios na placa **pendentes**.
- Nenhum dispositivo de movimento é inicializado pelo comando de ensaio.
- O `DIAGTEST` é uma função **temporária de comissionamento** e deve ser removida ou isolada antes da ativação motorizada.
- Os contadores não identificam a origem de interrupções do FreeRTOS/Arduino.
