# Modificações — Rio 0.57.0 (física da água) e Classic 1.7.1

## O que é
O efeito de água do Rio (`LevelEffectWater.lua`, decompilado do bytecode com as constantes) agora age sobre a física, não só sobre o som.

## Adicionado
- **Superfície:** o `y` do bloco marcador `BLOCK_INDESTRUCTIBLE_WATER_LEVEL`. Um corpo está na água abaixo dela e sai quando passa 0,05 acima.
- **Amortecimento linear ao entrar:** pássaro 0,3, pássaro globo inflado 8, ovo 1,3, qualquer peça 1,6; volta a 0 ao sair.
- **Flutuação** a cada passo: `-20 × massa × min(profundidade, 4) / 4 × flutuabilidade` (o peso da peça a partir de 4 de profundidade). A **flutuabilidade** vem do `blocks.lua`, por definição (barris 2,6, pássaros 2,5, ovos 4,2, globo grande 19); entra no `CAMPAIGN.BIN` (versão 11: o registro de força ganhou o campo `floatability`).
- **Peças:** arrasto quadrático (0,5·v²·seção, na escala do port 0,1) e sem empuxo extra quando sobem a mais de 18; sem força depois de 2 s paradas, como no original.
- **Macacos afundam:** um macaco-alvo na água morre (o original zera a resistência).
- Pássaro: flutuação de 2,5 e mais 10% quando sobe.

## Editado
- `tools/build_campaign.py` (formato v11), `tools/rio/build_rio_data.py` (flutuabilidade por definição), `ps2/src/campaign_types.h` e `game.cpp` (`water_forces`, `water_check`), fixtures dos testes. Versões: Rio 0.57.0, Classic 1.7.1 (só o formato novo do `CAMPAIGN.BIN`).

## Conferido
- 10 fases com água na simulação de 5 s sem tiro: com a flutuabilidade, as pilhas sobre barris ficam de pé como sem água; antes de usá-la, as torres sobre barris afundavam e os macacos se afogavam (7 -> 4 alvos em `ri_03`). Resta um macaco de parede que cai na água em `bd_05` (já caía sem água: o comportamento de agarrar a parede, R5, não existe).
- No emulador, Blossom River 3 (barco com macacos na linha d'água).

## Limites
- O arrasto é uma escala minha (0,1): o script usa áreas de seção calculadas por uma função que não reproduzi. Sem bolhas nem partículas de respingo.
- Não joguei as fases aquáticas até o fim.
