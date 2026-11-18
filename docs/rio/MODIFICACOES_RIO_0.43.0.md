# PS2Birds Rio — modificações desta etapa (0.43.0)

Primeira fase (Smugglers' Den 1 / `Level190`) conferida peça por peça.

## Causas encontradas e corrigidas
1. **Chão 5 unidades abaixo:** o `Ground` do Rio é um objeto de 10 unidades de altura; o topo é `y - 5`. A importação usava `y`, então as peças caíam ao começar. Em todas as 45 fases a peça dinâmica mais baixa termina em y ≈ 0 (valor esperado do topo do chão).
2. **Sprites com a metade do tamanho:** `physicsToWorld = 20` conta pixels do conjunto HD; o banco do jogo usa o conjunto SD (metade). Com 1 pixel SD = 2/20 unidade, as caixas (83 px HD = 41 px SD, 3,8 unidades de corpo) passam a ter o tamanho do corpo.
3. (da etapa anterior) Juntas importadas; âncoras sem colisão; blocos fixos não desenhados.

## Lançamento (conferido nos scripts)
- `defaultForce = -925` é o mesmo no Classic e no Rio, e o impulso é dividido pela massa, então a velocidade de lançamento é a mesma do Classic (46,25 = 925/20). Não há diferença de potência a corrigir; o que falta é o esticar máximo da borracha, que o jogo mantém como a escala de força do Classic.
- A gravidade não está em nenhum script; segue +20.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.42.0.md` (e a cópia do Drive), substituído por este.

## O que falta
1. Som (conferir com a ISO nova: antes as peças em queda geravam efeitos em excesso), acentos nas telas e a interface do Rio.
2. Outras fases: passar fase por fase como na primeira (as 14 restantes do galpão, depois a segunda etapa e a selva).
3. Habilidades dos pássaros em ação, os 49 níveis sem tipo de pássaro, macacos, folhas e frutas.
