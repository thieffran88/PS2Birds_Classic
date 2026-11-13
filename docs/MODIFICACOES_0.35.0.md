# Modificações 0.35.0

- **Episódio Surf and Turf (páginas 29 a 31, 45 fases)**
  - Ordem do `episodes.lua` (LevelS001 a LevelS045) e limites do `starLimits.lua`; bancos de texturas 25 a 27.
  - A campanha passa de 390 para 435 fases; 42 das 45 novas têm roteiro vencedor (faltam 2-15, 3-5 e 3-14).
  - Ao carregar, nenhuma peça cai sozinha nas 45 fases.
- **14 peças de praia pela `tools/add_definitions.py --material-kinds`**
  - Boia, balde, castelo de areia, óculos, martelo, chapéus, rede, vara de pesca, pá, graveto e as duas partes do guarda-sol.
  - Cada peça fica com o tipo do seu material (madeira, vidro, pedra ou extra).
- **Investigação**
  - O campo `chains` que aparece nas fases deste episódio está vazio em todas as fases do APK: não há correntes a portar.
- **Testes**
  - `profile_test`: Surf and Turf (ordem das 45 fases, bancos 25-27, 14 peças contra o `blocks.lua`).
  - `sprite_test`: 28 bancos, 413 quadros; peças de praia só nos bancos 25-27.
  - Suíte com 56 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.35.0; README com 435 fases; captura do Surf and Turf 1-1; documentos e captura da 0.34.0 substituídos.
  - Drive: zips da 0.33.0, um código-fonte da 0.31.0 que tinha ficado para trás e documentos da 0.34.0 removidos.
