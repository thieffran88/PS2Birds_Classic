# Modificações — 0.94.0

Versão das **cenas (quadrinhos)**: todas as 47 cenas do roteiro do jogo original estão no Port, com animação e gatilhos.

## Adicionado
- **18 cenas novas:** Short Fuse (abertura, duas do meio e final), Bad Piggies (abertura e uma ao fechar cada mundo), Piggy Farm e Dino (abertura e final), Birdday (mundos 2 e 3), Mighty Feathers e Egg Defender (abertura e final de cada).
- **Painéis animados** (Egg Defender e Mighty Feathers): cada painel entra com escala e posição (suave ou com quique) e, na abertura do Mighty Feathers, desvanece; o jogo compõe as camadas por software.
- **Piscar do Rei Porco** nas 8 cenas de fim de mundo (olhos trocados nos tempos do roteiro).
- Gatilhos: Mighty Feathers e Egg Defender abrem na primeira fase e fecham ao completar as 15 fases.
- `tools/vp8l.py`: modo sem perdas do WebP (conferido byte a byte com a biblioteca Pillow).
- Formato de cenas versão 5: duas rolagens, painéis animados (bit 2) e sprites sobre a imagem (bit 3).
- `docs/MODIFICACOES_0.94.0.md` e `docs/VALIDACAO_0.94.0.md`.

## Editado
- **Rolagem fiel ao original:** Big Setup, Surf and Turf e Short Fuse seguem o destino parcial/duplo do roteiro (antes rolavam até o fim).
- **Gatilhos:** abertura de cada episódio na primeira fase, final ao fechar o mundo; Birdday 7 tem abertura própria; a cena do Birdday 3 passou para depois do Birdday 4 (estava no mundo errado).
- Leitores de WebP movidos de `tools/rio/` para `tools/`; `tools/extract_menu.py` lê folhas WebP; os pacotes Windows e Android incluem esses leitores.
- `docs/CUTSCENES.md` atualizado. Versão 0.94.0.

## Excluído
- Da pasta do Drive e do repositório: ISO, ELF, gerador, código-fonte e documentos da 0.93.4 (substituídos).

## Observações
- O brilho da árvore do Short Fuse usa um sprite que não existe no APK: o original também não mostra nada.
- Dino: o modo "tamanho de referência" foi interpretado como fração da imagem ajustada à altura (provável, não confirmado).
- Testado: extração das 47 cenas, desenho dos painéis em simulação no PC e `make test`. Não testado no emulador/console: o disparo das cenas dentro do jogo.
