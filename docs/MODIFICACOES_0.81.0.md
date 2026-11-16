# Modificações completas — PS2Birds Classic 0.81.0

## Estrelinhas no fim de fase (`level_end.lua`, `addStarParticles`)
- Como no original (`levelCompleteStars1-3`), quando cada estrela grande aparece, ela solta uma rajada de estrelinhas (`P_STAR_1-5`).
- As estrelinhas:
  - saem em círculo, com velocidade alta;
  - caem com gravidade;
  - giram e somem aos poucos.
- São 20 estrelinhas por estrela, com até 60 na tela ao mesmo tempo.

## Nuvens de explosão (`explosionBuff`)
- Quando o TNT (ou outra peça explosiva) explode, ou quando o pássaro de choque detona, saem nuvens `EXPLOSION_CLOUD_1-6` do ponto da explosão.
- As nuvens se espalham, crescem e somem aos poucos.
- O jogo agora guarda a posição na tela da última explosão (`explosion_x`/`explosion_y`).

## Página de interface
- Foram acrescentados 11 sprites: 5 estrelinhas e 6 nuvens de explosão.
- A página passou a ter 64 sprites e usa 1022 das 1024 linhas.
- As nuvens ficam guardadas em 35% do tamanho e são desenhadas ampliadas.
- O teste `ui_assets_test.py` foi atualizado. As nuvens podem ter menos pixels opacos, porque o último quadro é quase só fumaça.

## Correção
- O ponto da explosão só é usado quando está dentro de uma faixa razoável da tela.
- Sem essa proteção, um valor inválido levava o GS a travar já na tela inicial (tela preta, sem quadros). O problema foi visto e corrigido no PCSX2 durante os testes.

## Conferido no PCSX2
- Captura em `docs/pcsx2_0.81.0_estrelas.png`.
- Na fase 1-1 aparecem as estrelinhas saindo das duas estrelas conquistadas.
- A tela inicial funciona normalmente.

## Limpeza
- Removidos os documentos da 0.80.0 (substituídos pelos da 0.81.0).
- Removidas da área temporária as pastas da 0.80.0, as ISOs e as capturas.

## Versão
- Tela de pausa, "Sobre" e README: 0.81.0.
