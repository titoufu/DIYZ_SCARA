# Fase 0 — correção v0.0.3

- Serial: removidas as flags USB CDC nativo; ensaios na interface USB-to-UART (UART0/COM5).
- ConfigManager: distingue NVS sem namespace/chave (`UNINITIALIZED`), esquema reconhecido (`OK`) e incompatível (`SCHEMA_ERROR`); nenhuma escrita automática.
- Mantidas desabilitação lógica dos atuadores e calibração inválida.
- Não há prova de NVS fisicamente íntegra por esta leitura; falhas de abertura são tratadas conservadoramente como `UNINITIALIZED`, pois `Preferences` não informa a causa detalhada nessa API.
- Persistência com integridade/versionamento completo permanece pendente para conclusão da Fase 0.
- Jitter pontual de 7370 us observado em bancada: repetir ensaio de 10 minutos e aferir distribuição e causa antes da integração de movimentos.
- Recompilar, gravar e executar PING, VERSION, STATUS e DIAG. Esperado em flash sem namespace: `NVS=UNINITIALIZED`.
