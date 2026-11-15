# Modificações 0.52.0

- **Sugador dos veículos dos porcos (novo no jogo, `EggDefenderGadget_Sucker.lua` + `RMFClassic_Sucker.lua`)**
  - O fator que faltava foi recuperado: no modo clássico a sucção é multiplicada por 5 (constante do `RMFClassic_Sucker.lua`), e o `RMFClassic.lua` liga todos os sugadores assim que a física começa.
  - Puxa toda peça móvel (porcos inclusive) a até 35 unidades e dentro de um cone de 55° voltado para trás da peça, com força 8 × (35 − distância) / 35 × massa × 5, aplicada só acima de 10, na direção do sugador.
  - Peça puxada que encosta no sugador é destruída (`onCollision`).
  - 21 sugadores no `CAMPAIGN.BIN` (marca `OBJECT_SUCKER`, bit 4, sem mudança de formato). Nenhum roteiro vencedor quebrou.
  - Falta prender o pássaro sugado (`onBirdTrapped`).
- **Investigação**
  - Canhão, foguetes, fole e inflador de balão existem no `blocks.lua`, mas nenhuma fase do port usa essas peças; com a arma de bolhas e o sugador, as engenhocas das fases estão cobertas.
- **Testes**
  - `globe_test`: peças à frente de um sugador chegam mais perto (ou são destruídas) em 3 s.
  - `profile_test`: 21 sugadores marcados no `CAMPAIGN.BIN`.
  - Suíte com 72 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.52.0; README descreve o sugador; captura do Mighty Feathers 13; documentos e captura da 0.51.0 substituídos; `LACUNAS_PORT.md` atualizado.
  - Entregues ISO, ELF (com os .BIN), Gerador para Windows e Código-Fonte; os da 0.51.0 removidos da pasta de trabalho.
