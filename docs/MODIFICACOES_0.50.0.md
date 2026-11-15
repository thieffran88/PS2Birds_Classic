# Modificações 0.50.0

- **Arma de bolhas dos veículos dos porcos (novo no jogo, `EggDefenderGadget_BubbleGun.lua` + `RMFClassic_BubbleGun.lua`)**
  - A cada 2,5 s a arma faz crescer uma bolha na boca (0,6 s) e a lança com impulso sorteado entre 35 e 50 na direção da arma; a bolha sobe sozinha (força do material `bubble`), estoura no primeiro impacto (resistência 1) e some após 3,5 s (`lifeSpan`).
  - O motor não cria objetos durante a fase: o importador guarda 2 bolhas escondidas por arma, reaproveitadas em revezamento (no máximo duas vivas ao mesmo tempo). São 10 armas em 6 fases (Flock Favorites 2-10, Mighty Feathers 2, 5, 10, 11 e 15).
  - As bolhas não contam como blocos nem dão pontos; a arma para quando é destruída.
  - `CAMPAIGN.BIN`: marca `OBJECT_GUN_BUBBLE` (bit 3) nas bolhas, sem mudança de formato. Atrito e restituição da bolha vêm do `blocks.lua` (4 e 0,95).
- **Roteiros novos**
  - O buscador de tiro único (`tests/solver.cpp`) passou pelas fases novas: 29 ganharam roteiro vencedor (Flock Favorites, Golden Eggs, Tutorial e Mighty Feathers). A busca continua para as demais.
  - Roteiros de fases com arma achados antes das bolhas foram descartados.
- **Testes**
  - `globe_test`: nenhuma bolha antes de 2,5 s, depois elas aparecem e nunca entram na contagem de blocos.
  - `profile_test`: 2 bolhas escondidas por arma no `CAMPAIGN.BIN`; o teste dos bolos do Birdday ignora as bolhas.
  - `make_campaign_table.py`: bolhas fora da contagem de blocos.
  - `sprite_test`: 720 quadros.
  - Suíte com 72 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.50.0; README descreve a arma de bolhas; captura do Mighty Feathers 10 com bolhas no ar; documentos e captura da 0.49.0 substituídos; `LACUNAS_PORT.md` atualizado.
  - Entregues ISO, ELF (com os .BIN), Gerador para Windows e Código-Fonte; os da 0.49.0 removidos da pasta de trabalho.
