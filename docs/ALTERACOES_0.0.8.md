# DIYZ SCARA — Fase 0 v0.0.8

## Mudança

- Respostas de comandos e BOOT encaminhadas para fila TX de 8192 bytes.
- Mensagens montadas em linha de 2048 bytes, inserção atômica no anel.
- `serialPoll` deixa de consumir RX quando há menos de 2048 bytes livres na fila TX.
- `flushTo` usa `availableForWrite()` e orçamento de 128 bytes por chamada.
- `DIAG`: `TX_PENDING`, `TX_HIGH`, `TX_CAP`, `TX_DROPPED`, `TX_FLUSH_MAX_US`, `TX_BACKPRESSURE_POLLS`.
- Baud permanece 115200; nenhum motor é habilitado.

## Limitações e verificações

- `TX_PENDING` conta bytes na fila de aplicação, não os já entregues ao driver/FIFO.
- A fila tem um só proprietário: `loopTask`; não pode ser chamada de outras tarefas ou interrupções.
- `availableForWrite()` limita a quantidade solicitada, mas seu efeito temporal deve ser aferido na placa.
- A contrapressão interna **não** impede estouro do RX se o host enviar comandos sem controle de fluxo. O ESP32 não tem garantia de receber 20 `DIAG` enviados simultaneamente; no teste confiável, envie `DIAG ID=...` e aguarde confirmação antes do próximo.
- Uma linha maior que 2048 bytes ou fila cheia continua sendo contabilizada em `TX_DROPPED`; a reserva de 2048 presume que nenhuma resposta exceda o limite de linha.
- Para comparar com v0.0.7, envie `DIAG` duas vezes e examine `ACQ_MISS`, `MAX_SERIAL_US`, `DISPATCH_MAX_US`, `TX_FLUSH_MAX_US`, `TX_DROPPED`.
- `DIAGTEST` continua introduzindo atraso artificial de 25 ms: não usar no ensaio temporal natural.
