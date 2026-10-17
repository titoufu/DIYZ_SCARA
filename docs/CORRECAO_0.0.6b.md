# Correção v0.0.6b

Evita colisão com macro `SERIAL` do Arduino ESP32, renomeando enumerador para `SERIAL_IO` e preservando etiqueta textual `SERIAL` no TRACE. Teste de regressão compila com `#define SERIAL 0x0`. Sem mudança funcional ou de armazenamento.
