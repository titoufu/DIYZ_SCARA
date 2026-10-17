# Fase 0 v0.0.4 — ensaio pendente no ESP32-S3

- UART0 pela USB-to-UART; não habilita USB CDC no boot.
- Medições adicionais de intervalos máximo/mínimo entre execuções e inícios atrasados.
- `CONFIG`: consulta NVS sem escrever. `CONFIGSAVE`: gravação explícita de registro binário versionado (schema 1), CRC32 e sequência, alternando slots A/B; verifica após gravação.
- Não contém dados de calibração e não altera nenhum indicador de validade.
- Estado UNINITIALIZED até primeiro `CONFIGSAVE` em instalação nova.
- Diagnóstico de jitter continua medindo atraso de início, não tempo de execução. MISS conta períodos inteiros ignorados; atraso menor que um período pode exceder um deadline sem aumentar MISS.
- Testes: `VERSION`, `STATUS`, `CONFIG`, `CONFIGSAVE`, `CONFIG`, reset, `CONFIG`; `DIAG` imediatamente e após 10 minutos.
- Não executar `CONFIGSAVE` caso haja dados NVS legados importantes no namespace `diyz`: a gravação usa as chaves `cfg_a`/`cfg_b`.
- Hardware real e compile PlatformIO ainda não ensaiados nesta revisão.
