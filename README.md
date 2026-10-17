# DIYZ SCARA — Fase 0, versão estável v0.0.11

Firmware para ESP32-S3 N16R8, PlatformIO/Arduino. **Fase 0: sem acionamentos físicos.**

## Alterações frente à v0.0.10-nvs-test

- Removidos os comandos destrutivos `NVSCORRUPT`, `NVSBOTHCRC` e `NVSBOTHSCHEMA`, incluindo as funções de injeção de falhas no `ConfigManager`.
- Removido `DIAGTEST`, pois introduzia deliberadamente atraso bloqueante de 25 ms; retirado também o contador `INJECTED_TESTS` do `DIAG`.
- Unificada a constante de versão em `VERSION` e `EVENT TYPE=BOOT`: `0.0.11-fase0-stable`.
- Mantidos: fila TX não bloqueante de 8192 bytes, limite de linha de 2048 bytes, contrapressão, parser PROTO=1, NVS A/B CRC, métricas do escalonador, `DIAG`, `TRACE`, `SERTRACE`, `CONFIGSAVE` explícito.

## Instalação e validação na placa

1. Abra `DIYZ_SCARA_Fase0` no VS Code/PlatformIO, selecione ambiente `esp32-s3-serial`.
2. Conecte **somente a placa ESP32-S3 via USB-to-UART**; não energize motores, bomba, servo ou drivers.
3. Build e Upload **sem Erase Flash**, para preservar NVS. Monitor COM5 a 115200 baud.
4. Execute `VERSION`, `STATUS`, `CONFIG`, `HELP`, `DIAG`, `DIAG`, `TRACE`, `SERTRACE`.
5. Confirme `FW=0.0.11-fase0-stable`, `ACTUATORS=DISABLED`, `NVS=OK`, `SCHEMA=1`, `SEQ=2`, `SLOT=B`, se a NVS não foi alterada desde o último teste.
6. Teste `NVSBOTHCRC ID=CONFIRM_CRC`, `NVSBOTHSCHEMA ID=CONFIRM_SCHEMA`, `NVSCORRUPT ID=CONFIRM_B` e `DIAGTEST` apenas como comandos **não reconhecidos** (`ERR CODE=UNKNOWN_COMMAND`). Eles não devem existir nesta versão.
7. Reinicie; repita `VERSION`, `STATUS` e `CONFIG` e confirme preservação.

## Segurança e escopo

O comando `CONFIGSAVE` permanece disponível e grava NVS **somente sob solicitação explícita**. Não executá-lo inadvertidamente. Segurança `ACTUATORS=DISABLED` é lógica; validar eletricamente as saídas antes de energizar os drivers. Não há leitura física de sensores nem controle motorizado nesta fase.

## Histórico de testes (v0.0.8–v0.0.10 de laboratório)

- Testes Serial SAFE e BURST: 20/20 respostas, `TX_DROPPED=0`, sem perdas de período nas condições ensaiadas.
- Estabilidade com mais de 3h: nenhum `MISS` no firmware com tarefas sintéticas.
- Parser: 14/14 testes aprovados.
- NVS A/B: alternância, persistência, fallback CRC do slot mais recente, ambos CRC inválidos, ambos esquemas incompatíveis e reconstrução confirmados na placa.
- **Ainda pendente:** ensaio de interrupção de alimentação durante gravação; temporização com atuadores e segurança elétrica.

Compilação PlatformIO e validação desta release específica dependem de teste na placa. Teste host: `bash tools/test_host.sh`.
