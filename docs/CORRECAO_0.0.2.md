# Correção v0.0.2

Corrigida a colisão de `DriveState::DISABLED` com o macro `DISABLED` do framework Arduino ESP32. O valor do enum agora se chama `DriveState::DRIVE_DISABLED`, mantendo a representação textual `DISABLED` nas respostas Serial. Teste de regressão acrescentado ao teste de host com macro `DISABLED` definido.

Nenhuma mudança de pinagem nem habilitação de atuadores. Compilação no ESP32-S3 ainda precisa ser comprovada pelo usuário.
