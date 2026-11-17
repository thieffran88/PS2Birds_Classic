# Modificações completas — PS2Birds Classic 0.86.0

## Primeira cutscene em quadrinhos: abertura de Poached Eggs (`cutscenes.lua`, `pack1_intro`)
- **Levantamento:** o APK tem as histórias de todos os episódios (início, meio e fim), montadas pelo `CUTSCENES.compo.json` e tocadas pelo `menus/cutscene.lua`. A lista do que falta está em `LACUNAS_PORT.md`.
- **O que foi feito:** a abertura de Poached Eggs.
  - É uma cena larga que a câmera percorre da esquerda para a direita, como no original.
  - Tamanho de referência 2048×659, ajustado à altura da tela.
  - Rolagem de 9,6 s com a curva `cubic_in_out`; a cena termina em 15,6 s.
  - Toca a música `birds_intro`.
- **Quando aparece:** ao entrar na fase 1-1 enquanto ela ainda não tem recorde, uma vez por sessão.
- **Para pular:** X ou Start.

## Como funciona
- **Gerador:** a nova ferramenta `tools/extract_cutscene.py` monta a cena a partir do APK no momento da geração.
  - As peças são posicionadas pelo pivô e desenhadas de trás para a frente.
  - Os blocos de gradiente do céu e da grama ficam opacos, sem emendas.
  - A imagem é reduzida para 1392×448 e gravada no novo `CUTSCENE.BIN`.
  - O repositório continua sem dados do jogo.
- **No PS2:** o novo módulo `cutscene.c` guarda a cena na RAM (1,25 MB) e copia as 640 colunas visíveis para a tela a cada quadro, do mesmo jeito que os fundos das fases. Não ocupa memória de vídeo.
- **Música:** `birds_intro` entra no `MUSIC.BIN` como faixa 13 (`MUSIC_INTRO`), tocada uma vez a partir do disco.
- **Gerador e kit para Windows:** passam a criar e copiar o `CUTSCENE.BIN`. A ISO e a pasta USB agora têm 9 arquivos de dados.

## Conferido
- 81 verificações no PC aprovadas.
- No PCSX2, pelo log do console:
  - o `CUTSCENE.BIN` é aberto e a cena carrega (1392×448) ao entrar na 1-1;
  - com X a cena é pulada e a fase começa normalmente.

## Limpeza
- Drive: os zips e documentos da 0.85.0 foram substituídos pelos da 0.86.0.
- Repositório: removidos os documentos e a captura da 0.85.0.
- Área temporária: removidas a pasta e os zips da 0.85.0, as prévias e os logs.

## Versão
- Tela de pausa, "Sobre" e README: 0.86.0.
