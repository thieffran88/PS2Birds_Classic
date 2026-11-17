# Modificações completas — PS2Birds Classic 0.85.0

## Correção: tábua de madeira pequena que "crescia" ao ser tocada
- **Sintoma:** a tábua comprida de madeira (`WoodBlock6`) aparecia com metade do tamanho. Quando o pássaro encostava nela sem destruí-la, ela voltava ao tamanho normal.
- **Causa:**
  - A peça decorativa `MISC_THEMED_8X1` do Birdday 5 reaproveita o mesmo desenho da tábua (`BLOCK_WOOD_1_6`).
  - Essa peça estava marcada para ser guardada em meia resolução, para caber na memória de vídeo daquele episódio.
  - Como o desenho é compartilhado, ele foi reduzido pela metade em **todo o jogo**.
  - Ao levar dano, a tábua troca para um quadro de rachadura, que estava em tamanho cheio. Por isso "voltava ao normal".
  - Em vários bancos o primeiro quadro de dano também estava reduzido.
- **Correção:** um desenho usado por qualquer peça em tamanho normal não é mais reduzido. Os quatro quadros da tábua agora têm 168×20 em todos os 65 bancos de textura.
- **Mesmo problema corrigido em outras peças que compartilhavam desenho:**
  - `BLOCK_APPLE`, usado pela bala de canhão de Bad Piggies;
  - `BLOCK_BEACHBALL`, usado pela bola de borracha;
  - `BP_WOODEN_WHEEL`, a roda de madeira: a versão normal e a fraca usavam escalas diferentes para o mesmo desenho, então uma delas saía no tamanho errado;
  - `SHORT_THICK_ROPE`, o pavio do canhão.
- **Prevenção:** a escala de desenho de cada peça (`definition_sprites.h`) agora vem da mesma regra que decide a redução dos desenhos (`extract_sprites.is_half`). Assim o tamanho guardado e o tamanho desenhado não podem mais divergir.

## Arquivos alterados
- `tools/extract_sprites.py`: não reduz desenhos compartilhados com peças de tamanho normal.
- `tools/build_campaign.py`: a escala de desenho usa a mesma regra.
- `ps2/src/definition_sprites.h` e `ps2/src/sprite_ids.h`: regenerados.
- `SPRITES.BIN` e os demais dados foram regenerados pelo gerador completo.

## Conferido
- 81 verificações no PC aprovadas.
- Checagem de todos os blocos com quatro quadros (intacto e três níveis de dano) em todos os bancos: nenhuma diferença de tamanho entre eles.
- No PCSX2, a torre da 1-1 aparece com as tábuas inteiras (captura em `docs/pcsx2_0.85.0_tabuas.png`).

## Limpeza
- Drive: os zips e documentos da 0.84.0 foram substituídos pelos da 0.85.0.
- Repositório: removidos os documentos e a captura da 0.84.0.
- Área temporária: removidas a pasta e os zips da 0.84.0 e a pasta de geração intermediária.

## Versão
- Tela de pausa, "Sobre" e README: 0.85.0.
