# DIYZ Fase 0 — v0.0.10 Teste de falhas NVS (EXCLUSIVAMENTE LABORATÓRIO)

Os comandos NVSBOTHCRC e NVSBOTHSCHEMA são **destrutivos** sobre cfg_a e cfg_b no namespace Preferences `diyz`. Eles não apagam outras partições/keys. Ambos exigem registros válidos nos dois slots, para evitar execução inadvertida em estado anormal. **Nunca instalar esta versão em robô com atuadores habilitados.**

## Estado esperado inicial
`VERSION` -> `0.0.10-fase0-nvs-test`, `CONFIG` -> `STATE=OK SCHEMA=1 SEQ=4 SLOT=B` (ou outro seq/slot OK com ambos válidos), `STATUS` -> `ACTUATORS=DISABLED`. O valor do BOOT foi unificado com VERSION.

## Etapa A: dois CRC inválidos
Execute `NVSBOTHCRC ID=CONFIRM_CRC` uma única vez. Esperado `STATE=CRC_ERROR SEQ=0 SLOT=-`; confirme com `CONFIG` e `STATUS` (NVS=CRC_ERROR). Reinicie a placa e consulte `CONFIG` e `STATUS`: erro deve persistir, atuadores desabilitados.

## Recuperação A
Execute `CONFIGSAVE` UMA vez: esperado `SAVED=1` e `CONFIG STATE=OK SCHEMA=1 SEQ=1 SLOT=A`. Reinicie e confirme. Faça `CONFIGSAVE` mais uma vez para criar `SEQ=2 SLOT=B` e confirme; agora ambos os slots devem ser válidos.

## Etapa B: ambos com schema incompatível
Somente após os dois CONFIGSAVE válidos, execute `NVSBOTHSCHEMA ID=CONFIRM_SCHEMA`. Esperado `STATE=SCHEMA_ERROR SEQ=0 SLOT=-`, `STATUS NVS=SCHEMA_ERROR`. Reinicie e confirme persistência do erro e atuadores desabilitados.

## Recuperação B
Execute `CONFIGSAVE` UMA vez: esperado `SEQ=1 SLOT=A STATE=OK`. Reinicie, confirme; `CONFIGSAVE` novamente -> `SEQ=2 SLOT=B`. Reinicie, confirme.

## Limitação crítica
`CONFIGSAVE` após perda de ambos os slots grava registro default de schema 1 e sequência reiniciada. Em Fase 0, ConfigRecord contém apenas metadados, mas em versões com parâmetros reais isto **não pode ser recuperação automática**: exigirá reset explícito, auditoria e recalibração. Este ensaio testa o comportamento existente e não modela perda de alimentação durante escrita.

Se qualquer resposta divergir: pare; não envie outros comandos destrutivos. Esta variante de laboratório deve ser retirada antes do firmware operacional.
