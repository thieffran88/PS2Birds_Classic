# Modificações 0.46.0

- **Power-ups (novo no jogo)**
  - Cada fase guarda o seu power-up; os cinco tutoriais da loja trazem o deles (`shopTutorialPowerup` do `episodes.lua`).
  - **Estilingue Real (King Sling, `Powerup_KingSling.lua`):** a força do estilingue passa de 1 para 1,3; o pássaro sai 30% mais rápido e a prévia da trajetória acompanha. Fica ativo no Tutorial 11; a tela mostra "ESTILINGUE REAL".
  - **Terremoto (Bird Quake, `Powerup_BirdQuake.lua`):** no Tutorial 12, L2 sacode a fase uma vez por 2 s; a cada passo, toda peça móvel recebe o impulso (dt·cos(t·π)·massa·7, −dt·massa·7,5). A tela mostra "L2: TERREMOTO" enquanto ele estiver disponível.
  - Sling Scope (Tutorial 9) não muda nada no port: a prévia da trajetória já é longa. Power Potion e Shockwave (Tutoriais 10 e 13) ficam registrados, ainda sem efeito.
  - `CAMPAIGN.BIN` versão 6: o registro de cada fase ganhou o power-up (140 bytes).
- **Investigação da decompilação**
  - `AdventureModeCollisions.lua` traz em Lua a fórmula de dano por colisão (pássaro × bloco com `damageMultiplier` e `velocityMultiplier`, bloco × bloco e chão). É do modo Aventura, não do Classic; ficou registrada em `LACUNAS_PORT.md` como referência, sem mudar a física.
- **Testes**
  - `globe_test`: o Estilingue Real lança 1,3 vez mais rápido; o Terremoto só pode ser usado uma vez e move as peças.
  - `profile_test`: os cinco tutoriais carregam o power-up certo no `CAMPAIGN.BIN`.
  - Fixtures de teste com o campo novo.
  - Suíte com 71 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.46.0; README com os controles dos power-ups; captura do Tutorial 12; documentos e captura da 0.45.0 substituídos; `LACUNAS_PORT.md` atualizado.
  - Entregues ISO, ELF (com os .BIN), Gerador para Windows e Código-Fonte; os da 0.45.0 removidos da pasta de trabalho.
