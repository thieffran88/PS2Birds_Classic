# Modificações — 0.94.3

Versão de **correções do Rio**, a partir do teste da ISO do Rio: peças que flutuavam ou caíam sozinhas e paredes invisíveis. O jogo Classic não mudou.

## Adicionado
- **Juntas do Rio** (`tools/rio/rio_import_level.py`): as fases do Rio usam juntas (soldas, dobradiças, motores, destruição por tempo) em 450 das 459 fases, e elas eram descartadas, então todas as peças ligadas por juntas caíam ou ficavam soltas. Agora as 730 juntas das 45 fases entram no `CAMPAIGN.BIN`, pelo mesmo formato do Classic.
- Peças espelhadas (escala negativa) e peças sem colisão (as âncoras `BLOCK_DUMMY_STATIC`, que viravam paredes invisíveis) agora levam os sinalizadores do jogo.
- `docs/MODIFICACOES_0.94.3.md`, `docs/VALIDACAO_0.94.3.md` e `docs/rio/MODIFICACOES_RIO_0.42.0.md`.

## Editado
- **Peças fixas** (densidade zero, como as pedras e o terreno `BLOCK_STATIC_*`) passam a ser terreno fixo do jogo; não são mais desenhadas (o cenário do tema é a arte); os sprites delas saíram do banco.
- `tools/rio/extract_sprites_rio.py`, `pack_sprites_rio.py` (tabela de máscaras de chão, vazia por enquanto), `ps2/src/main.c` (ramo do Rio) e versão 0.94.3.

## Excluído
- Da pasta do Drive e do repositório: documentos, código-fonte, ISO, ELF e kits da 0.94.2 (substituídos) e a documentação 0.41.0 do Rio.

## Observações
- Conferido no emulador: sem tiros, as fases do galpão e da selva ficam paradas (as estruturas não caem sozinhas); as peças quadradas flutuantes e as paredes brancas sumiram.
- Ainda não resolvidos: a mira, o som e a gravidade (ver `docs/rio/MODIFICACOES_RIO_0.42.0.md`).
