# PS2Birds Rio — modificações desta etapa (0.36.0)

Nenhum ELF, ISO ou kit do Rio foi publicado (o gerador continua recusando ISO do Rio); o Classic 0.94.1 não mudou.

## Adicionado
- **Pássaros do Rio ligados aos tipos do runtime** (`tools/rio/rio_campaign.py`): vermelho, azul pequeno, amarelo, preto, **branco, bumerangue, vermelho grande, globo e rosa**, com as variantes `R2_*` (mesmo raio e densidade do Classic, conferidos no catálogo do Rio). Fases que cabem nos pássaros conhecidos: **409 das 458** (antes 112).
- Sprites dos 9 tipos em três estados (`bird_sprites`, indexada pelo tipo do jogo, em `ps2/src/rio/definition_sprites.h`).
- **Um tema por fase:** o tema de cada fase vem do próprio arquivo da fase (`theme2`, `theme3`...), e o `THEMES.BIN` leva os temas usados. As folhas WebP dos temas (fundos da selva etc.) são lidas por `tools/webp.py`.
- `docs/rio/MODIFICACOES_RIO_0.36.0.md`.

## Editado
- `tools/rio/build_rio_data.py`: aceita qualquer pacote de fases (`--packs`), dá título por pacote e escolhe os temas pelas fases.
- `tools/rio/extract_themes_rio.py`: lê sheets PVR e WebP, e o chão em PVR ou WebP.
- `tools/rio/extract_sprites_rio.py`, `pack_sprites_rio.py` e `ps2/src/main.c` (só o ramo do Rio): tabela de pássaros por tipo; tipos sem sprite usam o vermelho.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.35.0.md` (e a cópia do Drive), substituído por este.

## Resultado no emulador
- A primeira fase da selva (jungle1) carrega com o fundo da selva, pássaros brancos na fila e a física rodando; o galpão continua certo. O disparo foi simulado por um ELF de teste fora do repositório.
- Ainda feio: as peças de madeira da selva aparecem como barras laranja finas, a fila de pássaros brancos aparece como círculos brancos sem rosto (as partes do corpo são sprites separados) e macacos e folhas não têm arte.

## O que falta
1. Arte das peças e dos pássaros por partes (corpo, rosto, bico); macacos, folhas e frutas.
2. Os pássaros que ainda não têm tipo: `BIRD_RIO_BLUE_ALONE` (27 fases), `RioBirds` (13), `RocketBird` (6) e `SUPERPOWERUP_BIRD` (3); são 49 fases.
3. Vitória por gaiolas confirmada na tela, gravidade real, interface/fonte/menu do Rio, música da fase.
4. Gerador do Rio de ponta a ponta (`gerar_iso.py --jogo rio`) e primeira ISO.
