# DIYZ SCARA — Correção de compilação v0.0.6a

- Arquivo `include/LoopTrace.h` reescrito de forma explícita e legível, com estrutura `TraceEvent`, campo `preceding` e atributo `lastPhase_` declarados antes dos métodos que os usam.
- Corrigida a string de versão no evento `EVENT TYPE=BOOT` de 0.0.5 para 0.0.6.
- Preservadas a versão reportada por `VERSION`, a NVS e as interfaces do protocolo.
- Testes locais de host: aprovados. Compilação com toolchain PlatformIO ESP32-S3 e gravação em placa: ainda pendentes.

Para aplicação: substituir toda a pasta do projeto pela versão v0.0.6a ou, no mínimo, substituir integralmente `include/LoopTrace.h` e `src/main.cpp`, executar PlatformIO Clean e Build.
