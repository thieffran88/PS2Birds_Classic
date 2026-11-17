# PS2Birds Rio — modificações desta etapa (0.29.0, investigação)

Nenhum ELF, ISO ou kit do Classic foi publicado nesta etapa; o Classic 0.93.0 publicado não mudou. O gerador do Rio continua recusando ISO. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `tools/rio/build_rio_data.py`: monta `CAMPAIGN.BIN`, `SPRITES.BIN`, `THEMES.BIN`, `SOUNDS.BIN` e `MUSIC.BIN` do Rio em um passo, a partir do APK do usuário.
- `docs/rio/MODIFICACOES_RIO_0.29.0.md`.

## Editado
- `tools/rio/rio_campaign.py`: os pássaros `SmallBlueBird`, `YellowBird` e `BlackBird` entram nos tipos 1, 2 e 3 do runtime (mesmos nomes do Classic).
- `ps2/tests/rio_smoke.cpp`: percorre todas as fases do arquivo (assenta e dá um tiro em cada).
- `docs/rio/ORDEM_DAS_FASES.md`: seção do `build_rio_data.py` e das 30 fases.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.28.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- As 30 fases de Smugglers' Den entram no `CAMPAIGN.BIN` (antes, 8) e carregam no `game.cpp` do Classic: assentam sem perder alvo e o tiro de teste não gera valores inválidos em nenhuma.
- Aproximações: gravidade +20 provisória; parâmetros dos pássaros do Classic.

## O que falta
1. `UI.BIN`, `FONT.BIN`, `MENU.BIN` do Rio; ligar tema e música no runtime; conferir no PCSX2.
2. Parâmetros dos pássaros do Rio (raio, densidade) e habilidades (azul pequeno = bomba de cachos).
3. Decodificador de WebP.
4. Macacos, Luiz e chefes.
