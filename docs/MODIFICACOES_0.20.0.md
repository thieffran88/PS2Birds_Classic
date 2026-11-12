# Modificações 0.20.0

- **Juntas (novo)**
  - Importador lê juntas dos níveis: tipos 1 (distância), 2 (solda) e 3 (dobradiça); converte âncoras relativas (coordType 2) para coordenadas do mundo.
  - Motor cria `b2DistanceJoint`, `b2WeldJoint` e `b2RevoluteJoint`; índice −1 prende ao chão.
  - Soldas quebráveis rompem quando a força de reação passa do limite (aproximação do port); juntas somem com os corpos destruídos.
  - Até 64 juntas por fase; novo teste `joint-test`.
- **Estrelas (novo)**
  - Limites prata/ouro de `starLimits.lua` extraídos para `ps2/data/star_limits.json` (84 fases) e gravados na tabela da campanha.
  - Função `game_level_stars`: vitória = 1, prata = 2, ouro = 3.
  - Estrelas mostradas na faixa de vitória e ao lado do recorde no menu de fases; novo glifo `*` na fonte.
- **Campanha**
  - Quatro fases mudaram com as juntas (2-14, 3-6, 4-8, 4-9) e ganharam novos roteiros vencedores; continuam 83 de 84 fases vencíveis (3-20 segue sem roteiro).
  - Teste da campanha lista as fases que não vencem quando `CAMPAIGN_LIST` está definido.
- **Investigação**
  - `docs/LACUNAS_PORT.md`: cobertura dos 762 níveis do Classic, definições e pássaros faltantes, juntas, câmera, dano nativo, pontuação e música.
- **Versão e documentos**
  - Tela e log em 0.20.0; README, validação, histórico e captura do menu (`docs/pcsx2_0.20.0_menu.png`).
- **Drive**
  - Arquivos 0.19.0 (ELF, ISO, código-fonte e modificações) substituídos pelos 0.20.0.
