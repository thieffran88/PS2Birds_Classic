# PS2Birds Rio — modificações desta etapa (0.42.0)

Resposta ao teste da ISO do Rio: caixas flutuando, caindo sozinhas, mira errada, som com defeito, paredes invisíveis.

## Causas encontradas e corrigidas
- **Caindo sozinhas e flutuando:** o importador descartava todas as juntas das fases (soldas e dobradiças ligam as peças entre si e ao terreno; 730 nas 45 fases entregues). Agora são importadas.
- **Paredes invisíveis:** as âncoras `BLOCK_DUMMY_STATIC` têm `collision = false` e colidiam. Agora levam o sinalizador sem colisão. Peças espelhadas (escala negativa) também passaram a espelhar.
- **Blocos quadrados e círculos brancos:** os blocos fixos (`BLOCK_STATIC_*`) são só física; o desenho é o cenário. Não são mais desenhados.
- Peças de densidade zero são terreno fixo do jogo, não peças quebráveis.

## Ainda errado (causas conhecidas)
- **Mira:** a velocidade de lançamento usa o valor do Classic. O Rio calcula o impulso pelo esticar da borracha, pela `defaultForce` do pássaro e pela massa (`gamelogic.lua`); a `defaultForce` e o `rubberBandMaximumLength` ficam em tabelas nativas que não consegui ler. A trajetória mostrada e o voo concordam entre si (usam a mesma conta), mas não necessariamente com o jogo original.
- **Gravidade:** continua +20 (valor do Classic, sem medida no original).
- **Som:** os efeitos usam a lista de sons do Classic, preenchida com o arquivo equivalente do Rio (por exemplo, o som do pássaro azul é o `special group`). Sem ouvir o resultado não consegui conferir; peças soltas caindo geravam sons em excesso, o que as juntas devem reduzir. O Rio tem sons próprios por pássaro (`bird_0N_collision`, `_select`, `_unselect`) que o jogo ainda não usa.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.41.0.md` (e a cópia do Drive), substituído por este.
