# Modificações 0.38.0

- **Episódio Bird Island (página 38, 21 fases)**
  - Ordem do `episodes.lua` (Level232_Movie a Level265_t_Movie) e limites do `starLimits.lua`; banco de texturas 34.
  - A campanha passa de 525 para 546 fases; 19 das 21 novas têm roteiro vencedor (faltam 1-10 e 1-15).
  - Ao carregar, nenhuma peça cai sozinha nas 21 fases.
- **17 peças novas pela `tools/add_definitions.py --material-kinds`**
  - Estátua de pássaro em cinco partes, sacos de areia, figuras e montes de areia, flor, manchas de tinta, ingresso, pôsteres.
  - **Rei porco barbudo (`BeardedKingPiglette`)**: material `piglette` e `levelGoal` no `blocks.lua`, então entra como porco (tipo 4, conta como alvo), raio 2,825, resistência 100, com o sprite próprio.
- **Correção na `add_definitions.py`**
  - A busca do sprite também casava com `spriteScore`; quando a pontuação vinha antes no `blocks.lua`, a peça recebia o sprite de pontos (o rei barbudo apareceu como "5K_GREEN"). Agora só `sprite = ...` conta. Nenhuma peça já importada mudou.
  - O material `piglette` passa a dar o tipo porco com `--material-kinds`; o jogo desenha porcos importados com o sprite próprio.
- **Testes**
  - `profile_test`: Bird Island (ordem, banco, 17 peças, o rei barbudo como porco com o sprite certo).
  - `sprite_test`: 35 bancos, 508 quadros; peças do episódio só no banco 34.
  - Suíte com 62 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.38.0; README com 546 fases; captura do Bird Island 1-1; documentos e captura da 0.37.0 substituídos.
  - Drive: zips da 0.36.0 e documentos da 0.37.0 removidos.
