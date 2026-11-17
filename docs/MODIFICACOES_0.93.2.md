# Modificações — 0.93.2

Versão de **diagnóstico** da tela de dados ausentes. A 0.93.1 foi relatada com a tela "ARQUIVOS DE DADOS AUSENTES" no console e no emulador, mesmo com os dez arquivos na pasta. Não consegui reproduzir (a 0.93.0, a 0.93.1 e a ISO gerada pelo pacote do Windows abrem normalmente aqui, e os dados da 0.93.1 são idênticos aos da 0.93.0), então esta versão passa a dizer **por que** a leitura falhou.

## Adicionado
- Na tela de dados ausentes: o código de erro de cada arquivo (`CAMPAIGN.BIN: ERRO n`, `SPRITES.BIN: ERRO n`), a versão do programa, a pasta de onde tentou ler (ou "lendo do disco") e a legenda dos códigos (1 abrir, 2 ler, 3 memória, 4 formato, 5 tamanho, 6 limites; nos sprites também 7 leitura de um banco). O mesmo vai para o console (`PS2Birds: data files missing ...`).
- `game_campaign_error()` e `sprites_error()`: motivo da última falha de leitura.

## Editado
- `ps2/src/game.cpp`, `ps2/src/sprites.c` (e seus cabeçalhos): registram o motivo da falha; o comportamento de leitura é o mesmo.
- `ps2/src/main.c`: a tela de dados ausentes mostra os detalhes.
- Versão 0.93.2 (README e tela do jogo).

## Excluído
- Do Drive e do repositório: ISO, ELF, gerador, código-fonte e registros da 0.93.1 (substituídos).

## Verificação
- Sem os dados, o ELF mostra os códigos; com os dados, abre normalmente no emulador (conferido nos dois casos).
- Dados da 0.93.2 idênticos aos da 0.93.0 e da 0.93.1.
