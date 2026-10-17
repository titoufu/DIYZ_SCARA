# DIYZ — Registro de implementação da Fase 0

Data: 16/10/2026
Versão: 0.0.1-fase0
Documentos-fonte: DIYZ-HW-001 Rev.1.1; DIYZ-FW-001 Rev.1.0; DIYZ-DEV-001 Rev.1.0.

## Contratos de software adotados — provisórios até aceite em hardware

1. **Dados:** valores tipados `RobotState`, `DriveState`, `CalibrationValidity` e snapshot somente-leitura. Flags de calibração iniciam sempre falsas; nenhuma transição para `READY` ocorre nesta versão. Unidades futuras: graus, mm, graus/s, ms e µs nomeadas explicitamente.
2. **Escalonamento:** cooperativo mono-contexto, períodos 5/10/20/200 ms, medição `runs`, `missed`, jitter máximo e tempo de execução máximo. Nenhum RTOS adicional, atraso bloqueante, periférico PWM ou gravação flash em `loop()`.
3. **Comandos:** protocolo textual versionado 1, ID opcional alfanumérico, roteamento central na função de aplicação `processLine`, erros explícitos, comandos informativos e nenhum comando de movimento. Transporte Serial limitado a 64 bytes consumidos por iteração, limite de linha 127 bytes.
4. **Propriedade:** RobotManager é o único detentor do snapshot; demais componentes podem consultar estado imutável. Não há autorização de movimento possível. SafetySupervisor completo e CommandArbiter de múltiplas origens ainda não são necessários nem foram falsamente implementados.
5. **Persistência:** Preferences/NVS lido apenas para compatibilidade de esquema; integridade/CRC, gravação de configuração e transações persistentes ainda pendentes.

## Evidências

- Teste `bash tools/test_host.sh`: PASS (parser, IDs, estados, escalonador).
- Build PlatformIO: PENDENTE (ferramenta indisponível neste ambiente de criação).
- Gravação ESP32-S3: PENDENTE (sem acesso físico).
- Inspeção elétrica de boot: PENDENTE (prevista na Fase 1; sem motores energizados antes disso).

## Próximos passos práticos

1. O usuário abre a pasta no VS Code/PlatformIO e compila.
2. Em caso de erro, enviar log integral de compilação.
3. Gravar por USB com a placa isolada de potência; enviar captura `VERSION`, `STATUS`, `DIAG`, `HELP` e amostras de erros.
4. Confirmar pinagem/porta de USB do módulo e ensaiar reset; somente então declarar parte de bancada aceita.
5. Se validado, tag sugerida `fase0-v0.0.1`; publicar relatório de passagem e avançar à Fase 1.
