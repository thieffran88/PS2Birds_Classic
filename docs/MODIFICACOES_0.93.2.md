# Modificações — 0.93.2

Versão de **diagnóstico** da tela de dados ausentes. A 0.93.1 foi relatada com a tela "ARQUIVOS DE DADOS AUSENTES" no console e no emulador, mesmo com os dez arquivos na pasta. Não consegui reproduzir (a 0.93.0, a 0.93.1 e a ISO gerada pelo pacote do Windows abrem normalmente aqui, e os dados da 0.93.1 são idênticos aos da 0.93.0), então esta versão passa a dizer **por que** a leitura falhou.

## Adicionado
- Na tela de dados ausentes: o código de erro de cada arquivo (`CAMPAIGN.BIN: ERRO n`, `SPRITES.BIN: ERRO n`), a versão do programa, a pasta de onde tentou ler (ou "lendo do disco") e a legenda dos códigos (1 abrir, 2 ler, 3 memória, 4 formato, 5 tamanho, 6 limites; nos sprites também 7 leitura de um banco). O mesmo vai para o console (`PS2Birds: data files missing ...`).
- `game_campaign_error()` e `sprites_error()`: motivo da última falha de leitura.

- **Pasta padrão `apps/PS2Birds`:** o gerador grava ELF e dados em `apps\PS2Birds` (antes `PS2Birds_Classic_USB`; a opção `--pasta` continua permitindo trocar), e o ELF, rodando solto (USB, HDD, emulador), procura os dados primeiro nessa pasta (`mass:/apps/PS2Birds/`, `mass0:/apps/PS2Birds/`, `host:apps/PS2Birds/`, com `apps` em maiúsculas também) e depois ao lado do programa, em `host:`, `mass:`, `mass0:` e `./`. A ISO não muda (lê direto do disco).
- **Busca dos dados mais robusta no ELF solto:** se o `CAMPAIGN.BIN` não estiver no local informado pelo carregador (relatado como "a ISO abre, o ELF não"), o programa tenta os locais acima e usa o primeiro onde o encontra; a tela de erro lista o que tentou.

## Editado
- `ps2/src/game.cpp`, `ps2/src/sprites.c` (e seus cabeçalhos): registram o motivo da falha; o comportamento de leitura é o mesmo.
- `ps2/src/main.c`: a tela de dados ausentes mostra os detalhes.
- Versão 0.93.2 (README e tela do jogo).

## Excluído
- Do Drive e do repositório: ISO, ELF, gerador, código-fonte e registros da 0.93.1 (substituídos).

## Verificação
- Sem os dados, o ELF mostra os códigos e os locais tentados; com os dados ao lado, abre; com os dados só em `apps/PS2Birds`, abre (conferido no emulador nos três casos).
- Dados da 0.93.2 idênticos aos da 0.93.0 e da 0.93.1.

- `GERAR_ISO.bat`, `tools/LEIA-ME_GERADOR.txt` e `tools/gerar_iso.py`: pasta de saída `apps\PS2Birds`.
