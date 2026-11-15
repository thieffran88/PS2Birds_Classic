# Modificações 0.53.0

- **Pássaro preso pelo sugador (novo no jogo, `EggDefenderGadget_Sucker.onBirdCollision` + `RMFClassic_Sucker.lua`)**
  - O pássaro que encosta num sugador fica preso dentro dele e acompanha o veículo até o sugador ser destruído; a vez passa ao próximo pássaro normalmente.
  - Não ficam presos: o Terence (`canBeTrappedBySucker = false`) e o GlobeBird depois de inflar (`canBeTrappedBySuckerAfterSpecialty = false`).
  - Preso, o GlobeBird ou o pássaro de choque que usa a habilidade destrói o sugador e se solta (`specialtyDestroysSucker`); a Stella não consegue usar a habilidade (`specialtyDisabledInsideSucker`).
  - Com isso o sugador fica completo.
- **Roteiros refeitos**
  - Mighty Feathers 10 e 12 perderam os roteiros (o pássaro passou a ficar preso) e ganharam roteiros novos pelo buscador de tiro único.
- **Testes**
  - `globe_test`: um tiro que acerta um sugador deixa o pássaro preso e parado junto dele.
  - Suíte com 72 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.53.0; README descreve o pássaro preso; captura do menu; documentos e captura da 0.52.0 substituídos; `LACUNAS_PORT.md` atualizado.
  - Entregues ISO, ELF (com os .BIN), Gerador para Windows e Código-Fonte; os da 0.52.0 removidos da pasta de trabalho.
