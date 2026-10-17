# DIYZ SCARA — v0.0.6 | Rastreio de bloqueios do loop

## Alterações
- Adicionado `TRACE`: registra a maior lacuna entre o fim de uma passagem do `loop()` e o início da próxima (tempo não executando o código da aplicação).
- Mede os maiores tempos gastos no tratamento de comandos Serial e em cada fase cooperativa, bem como o maior tempo total de uma passagem de `loop()`.
- Registra uptime, núcleo e a **última fase executada antes da lacuna**. Esse campo é contexto e **não comprova causalidade**.
- Mantidos `DIAG`, `DIAGTEST`, comandos e esquema NVS anteriores.
- Nenhum motor ou servo é inicializado para movimento.

## Ensaio
1. Compilar e gravar com a porta USB-to-UART e a COM5, mantendo apenas a placa conectada.
2. Verificar `VERSION`, `STATUS`, `CONFIG`, `DIAG`, `TRACE`.
3. Executar `DIAGTEST` uma vez e consultar `DIAG` e `TRACE`. O atraso de 25ms ocorre dentro de `serialPoll`; assim pode aparecer como `MAX_SERIAL_US`, ao contrário de uma interrupção externa à execução do loop.
4. Pressionar RESET, não executar `DIAGTEST`, aguardar 10–20 minutos e consultar `TRACE` e `DIAG`.
5. Reportar as saídas completas, incluindo campos `MAX_IDLE_GAP_US`, `MAX_SERIAL_US`, `MAX_LOOP_WORK_US` e `LOOP_MAX_GAP_US`.

## Interpretação
- `MAX_IDLE_GAP_US` grande com `MAX_LOOP_WORK_US` baixo: atraso entre retornos ao código da aplicação, compatível com preempção do sistema ou bloqueio externo à rotina medida.
- `MAX_LOOP_WORK_US` e `MAX_SERIAL_US` grandes: trabalho ou espera dentro da execução do loop, incluindo processamento do comando de teste.
- `PRECEDING_PHASE` indica somente qual rotina terminou por último; não identifica o componente responsável por preempção.
- `LOOP_MAX_GAP_US` mede **início a início**, diferente de `MAX_IDLE_GAP_US` (fim a início).

## Estado da validação
Testes unitários C++ de rastreio executados localmente. Build PlatformIO e medições físicas pendentes.
