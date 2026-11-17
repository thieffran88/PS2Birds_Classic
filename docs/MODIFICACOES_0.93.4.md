# Modificações — 0.93.4

Versão das **cenas (quadrinhos)**: 47 das 48 cenas do jogo original agora existem no Port, com os gatilhos corrigidos.

## Adicionado
- **18 cenas novas:** Short Fuse (abertura, duas do meio e final), Bad Piggies (abertura e uma ao fechar cada mundo), Piggy Farm e Dino (abertura e final), Birdday (mundos 2 e 3), Mighty Feathers e Egg Defender (abertura e final de cada).
- **Cenas em painéis** (Egg Defender e Mighty Feathers): os painéis aparecem juntos, nos lugares finais, como imagem parada pelo tempo da cena. A animação de entrada de cada painel não foi reproduzida.
- `tools/vp8l.py`: modo sem perdas do WebP, necessário para as folhas do Classic (conferido byte a byte com a biblioteca Pillow).
- Segunda rolagem nas cenas (formato `CUTSCENE.BIN` versão 5).
- `docs/MODIFICACOES_0.93.4.md` e `docs/VALIDACAO_0.93.4.md`.

## Editado
- **Rolagem fiel ao original:** cenas que rolam só até o meio da imagem ou em duas etapas (Big Setup, Surf and Turf, Short Fuse) seguem o destino do roteiro; antes rolavam até o fim.
- **Gatilhos:** abertura de cada episódio na primeira fase; final ao fechar o mundo; Birdday 7 tem abertura própria; a cena `birdday3_complete` passou para depois do Birdday 4 (estava no mundo errado).
- `tools/webp.py`, `tools/vp8l.py`, `tools/vp8_tables.py` saíram de `tools/rio/` para `tools/` (usados pelo Classic e pelo Rio); `tools/extract_menu.py` lê folhas WebP.
- `tools/make_windows_kit.py` (e o pacote Android): incluem os leitores de WebP.
- `docs/CUTSCENES.md` atualizado. Versão 0.93.4.

## Excluído
- Da pasta do Drive e do repositório: ISO, ELF, gerador, código-fonte e documentos da 0.93.3 (substituídos).

## Limitações conhecidas
- Cenas do Dino: o modo "tamanho de referência" do original foi estimado.
- Brilho piscante da árvore (Short Fuse) e olhos do Rei Porco não são animados.
- Só a cena `eggdefender`/`rmf` por painéis perdeu a animação; falta ainda testar as novas cenas no emulador.
