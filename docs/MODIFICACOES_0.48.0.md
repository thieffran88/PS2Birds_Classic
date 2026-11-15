# Modificações 0.48.0

- **Onda de Choque (Shockwave, novo no jogo, `Powerup_Shockwave.lua`)**
  - Fora a animação de introdução, o power-up troca o pássaro do estilingue por um pássaro de choque (`replaceBird("BlackBird_Shockwave")`).
  - No Tutorial 13, L2 durante a mira faz a troca, uma vez por fase; a tela mostra "L2: ONDA DE CHOQUE". O resto da fila não muda e o pássaro trocado é o que sai no lançamento.
  - Os sprites do pássaro de choque entram no banco do Tutorial (fases com esse power-up).
  - Com isso, os cinco tutoriais da loja estão completos (a Mira não muda nada no port).
- **Testes**
  - `globe_test`: a troca só acontece uma vez, mantém o resto da fila e o pássaro lançado é o de choque.
  - `sprite_test`: banco do Tutorial com 96 quadros.
  - Suíte com 71 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.48.0; README com o controle da Onda de Choque; captura do Tutorial 13 com o pássaro trocado; documentos e captura da 0.47.0 substituídos; `LACUNAS_PORT.md` atualizado.
  - Entregues ISO, ELF (com os .BIN), Gerador para Windows e Código-Fonte; os da 0.47.0 removidos da pasta de trabalho.
