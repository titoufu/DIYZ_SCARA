# DIYZ SCARA Fase 0 — v0.0.9: ensaio controlado de fallback NVS

**Firmware exclusivo de laboratório. Não utilizar com motores ou drivers habilitados.**

Partida: preserva a segurança lógica: `ACTUATORS=DISABLED`, sem movimento.

## Objetivo
Testar a recuperação do slot A válido quando o CRC do slot B mais recente é invalidado.

## Pré-condições
- Estado inicial conhecido: `CONFIG STATE=OK SCHEMA=1 SEQ=4 SLOT=B`.
- A deve conter uma configuração válida de sequência 3 (a função valida isso).
- Não realizar `CONFIGSAVE` antes da injeção de falha.

## Procedimento manual (COM5, 115200)
1. Grave o firmware com PlatformIO (Upload sem apagar NVS) e abra o Monitor.
2. Envie `VERSION`, `STATUS`, `CONFIG`. Confirme `SEQ=4 SLOT=B` e atuadores desabilitados.
3. Envie `NVSCORRUPT ID=CONFIRM_B` uma única vez. Esperado `OK ID=CONFIRM_B CMD=NVSCORRUPT TARGET=B CRC_INVALID=1 FALLBACK=A SEQ=3`.
4. Envie `CONFIG`: esperado `STATE=OK SEQ=3 SLOT=A`.
5. Pressione RESET/EN e envie `CONFIG`, `STATUS`: esperado `SEQ=3 SLOT=A`, `ACTUATORS=DISABLED NVS=OK`.
6. Envie `CONFIGSAVE` **uma vez**: esperado `SAVED=1`; `CONFIG` deve retornar `SEQ=4 SLOT=B`, com registro novo e válido.
7. Pressione RESET/EN e confirme `CONFIG STATE=OK SCHEMA=1 SEQ=4 SLOT=B`.

## Limitações
O teste altera intencionalmente `cfg_b` na NVS, mas conserva `cfg_a` válido. Não demonstra resistência a corte de energia, corrupção simultânea, ou compatibilidade de esquema. A gravação de backup/recuperação é validada no runtime, não constitui garantia contra falhas de hardware. O comando de falha deve ser removido das versões de produção.

Se qualquer resposta divergir do esperado, pare e envie o log completo; não execute gravações adicionais.
