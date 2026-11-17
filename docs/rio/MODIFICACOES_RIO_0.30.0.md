# PS2Birds Rio — modificações desta etapa (0.30.0, investigação)

Nenhum ELF, ISO ou kit do Classic foi publicado nesta etapa; o Classic 0.93.0 publicado não mudou. O gerador do Rio continua recusando ISO. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- Macros `RED_RADIUS` e `RED_RESTITUTION` em `ps2/src/game.cpp` (Classic 0,75 e 0,25; Rio 0,85 e 0,43).
- `docs/rio/MODIFICACOES_RIO_0.30.0.md`.

## Editado
- `ps2/src/game.cpp`: o corpo do pássaro vermelho usa as macros (valores do Classic inalterados).
- `ps2/Makefile`: `PS2BIRDS_RIO` também para o C++; o jogo em construção fica em `build/.game` e os objetos e ELFs são removidos quando `GAME` muda.
- `docs/rio/ORDEM_DAS_FASES.md`: seção "Parâmetros dos pássaros e correção do build do Rio".

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.29.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- Azul, amarelo e preto do Rio já coincidem com os valores do runtime; só o vermelho difere (raio 0,85 e restituição 0,43), agora tratado na variante do Rio.
- **Correção de um erro anterior:** o ELF do Rio gerado sobre objetos do Classic ainda tinha os caminhos `/PS2BIRDS`. O build agora separa os dois jogos; conferido: Rio só `/PS2BRIO`, Classic só `/PS2BIRDS`.

## O que falta
1. `UI.BIN`, `FONT.BIN` e `MENU.BIN` do Rio; ligar tema e música no runtime; conferir no PCSX2.
2. Habilidades dos pássaros do Rio (o azul é bomba de cachos).
3. Decodificador de WebP.
4. Macacos, Luiz e chefes.
