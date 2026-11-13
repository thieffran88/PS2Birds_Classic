# Modificações 0.34.0

- **Episódio Birdday 7 (página 28, 15 fases)**
  - Ordem do `episodes.lua` (Level263_BirdDay7 a Level301_BirdDay7) e limites do `starLimits.lua`; banco de texturas 24.
  - A campanha passa de 375 para 390 fases; 13 das 15 novas têm roteiro vencedor (faltam 7-7 e 7-14).
- **36 peças novas pela `tools/add_definitions.py`**
  - Conjunto de madeira temático (11 blocos `WoodBlock*_BD7`), cinco balões com a força do material `balloon`, cupcake, bandeiras e adesivo sem colisão, arbustos, montes de areia e paredes de cenário, flores e folhas da ilha, barril, motor e chave inglesa de Bad Piggies.
- **Tipo do material nas peças temáticas (`--material-kinds`)**
  - Peças de madeira, vidro e pedra adicionadas a partir do Birdday 7 ficam com o tipo do material (dano por pássaro, som, pontuação), como no original; antes toda peça automática virava "extra".
  - Material `immovableFragile` (flores e folhas) vira cenário frágil, que estoura ao ser tocado.
  - As peças dos episódios anteriores não mudaram (nenhum roteiro afetado).
  - O jogo desenha esses blocos temáticos com o sprite próprio.
- **Testes**
  - `profile_test`: Birdday 7 (ordem, banco, 36 peças contra o `blocks.lua`, tipo pelo material, força dos balões); verificação de Birdday 6 restrita às fases dele.
  - `sprite_test`: 25 bancos, 399 quadros; peças do Birdday 7 só no banco 24.
  - Suíte com 54 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.34.0; README com 390 fases; captura da Birdday 7-6; documentos e captura da 0.33.0 substituídos.
  - Drive: zips da 0.32.0 e documentos da 0.33.0 removidos.
