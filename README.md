# PS2Birds Classic — protótipo 0.17.0

Foco exclusivo no Classic. Inclui a primeira página completa de Poached Eggs
e a segunda e a terceira páginas completas (63 fases, todo o episódio Poached Eggs) e a primeira página de Mighty Hoax (84 fases) na ordem declarada pelo próprio APK, com sprites originais, pássaros
vermelho, azul, amarelo, preto e branco, porcos pequeno/médio/grande/capacete, pedra, madeira,
vidro, pedras circulares, baú, bloco sorridente, bola, pato e morango, seleção de fases, desbloqueio
sequencial e recordes persistentes no Memory Card 1. Ainda não é o port completo.

## Jogar

Abra o ELF ou ISO no PCSX2 ou teste no carregador homebrew escolhido. O console
físico ainda não foi validado. Vídeo NTSC 640×448 em 16 bits com dithering, efeitos sonoros originais e salvamento no Memory Card 1.

- R3 liga/desliga os efeitos sonoros; a preferência vale para a sessão.
- Menu: cima/baixo escolhem a fase, esquerda/direita mudam três posições; X inicia se estiver desbloqueada.
- Partida: direcional/analógico esquerdo ajustam ângulo e força; X lança.
- Pássaro azul: após lançar, pressione X novamente antes de colidir para dividir em três.
- Pássaro amarelo: após lançar, pressione X novamente antes de colidir para acelerar.
- Pássaro preto: X detona durante o voo ou após contato; detona automaticamente cerca de um segundo após contato.
- Pássaro branco: X após lançar e antes de colidir solta um ovo verticalmente; o pássaro recua e o ovo explode ao atingir um corpo ou o chão. Uma vez por lançamento.
- Segure Quadrado para ajuste fino; Triângulo reinicia; Start pausa/continua.
- Select abre/fecha a seleção de fases. O jogo pausa enquanto o menu está aberto.
- Vitória: X avança para a próxima fase; após 4-21 retorna à seleção.
- L1 salva/tenta novamente no Memory Card 1. A vitória salva automaticamente.
- Use cartão PS2 previamente formatado pelo navegador do console; o jogo não formata.
- Aguarde o aviso de salvamento terminar antes de remover o cartão.
- Sem cartão, a partida continua com progresso somente da sessão.

## Ordem das fases

`profiles/classic_episode.json` reproduz a lista da primeira página de Poached
Eggs recuperada de `assets/data/scripts/episodes.lua` do APK: Level1, Level57,
Level53, Level3, Level6, Level2, Level4, Level5, Level7, Level8, Level9, Level13,
Level10, Level39, Level12, Level15, Level17, Level14, Level16, Level23 e Level44.
As versões 0.2.0 a 0.8.0 assumiam que a fase 1-N era LevelN; saves dessas versões
são migrados para as novas posições ao serem lidos (ver `ps2/src/save_mc.c`).

## Compilar

```sh
export PS2DEV=/caminho/ps2dev
sh ps2/build.sh -j3
sh ps2/build.sh -j3 test
sh ps2/build.sh iso
```

Requisitos: pacote ps2dev fornecido (PS2SDK/gsKit), make, Python 3.9+, g++ e
OpenSSL e Pillow para os testes de conversão PNG. Instale Pillow com `python -m pip install -r tools/requirements-assets.txt`. A ISO usa genisoimage/mkisofs quando existir ou o pacote
Python `pycdlib` como alternativa. Box2D, audsrv e texturas/efeitos convertidos estão incluídos no
código-fonte; o build não precisa do APK nem de downloads.

## Regenerar dados e gráficos

```sh
python tools/build_campaign.py
python tools/extract_sprites.py /caminho/com.rovio.angrybirds.apk ps2/assets --game classic --sevenzip tools/py7z.py
python tools/build_save_icon.py
python tools/extract_audio.py /caminho/com.rovio.angrybirds.apk ps2/assets --adpenc "$PS2SDK/bin/adpenc"
```

As 84 fontes Lua decompiladas e métricas estão em ps2/data; o importador não
executa Lua. A extração de gráficos exige o APK original e OpenSSL; `tools/py7z.py`
substitui o 7-Zip usando o pacote `py7zr` (um 7z nativo também serve). As três
páginas de atlas usam 2,5 MiB (limite 2,5 MiB) e ficam incorporadas no ELF; o
frame buffer passou a 16 bits para abrir espaço na VRAM. As plataformas estáticas
usam máscaras originais com cor provisória; o preenchimento original ainda está pendente.

Os 20 efeitos usam 22050 Hz mono em ADPCM SPU2 e somam cerca de 377 KiB.
FFmpeg e adpenc são necessários apenas para regenerá-los; os dados convertidos
já estão incluídos. O audsrv é compilado das fontes incluídas. Se o módulo não
inicializar, o jogo continua com SOM INDISPONIVEL. Ainda não há música de fundo.

As ferramentas/perfis de outras séries permanecem no pacote, sem novos ports.
Consulte docs/MODIFICACOES_0.17.0.md e docs/VALIDACAO_0.17.0.md. Documentos das versões
anteriores são históricos. Saves da 0.6.0 à 0.8.0 são aproveitados e regravados no
formato versão 3 (288 bytes, até 64 fases). A seleção mostra três fases por página
e muda automaticamente conforme cima/baixo.

Raios, densidades, restituições e fatores de dano direto (azul no vidro 2,9;
amarelo na madeira 2,4) foram consultados nos dados recuperados. O fator de
aceleração do amarelo (1,8×), a abertura do leque do azul (±10°), o cálculo de
dano e as colisões de cenário (caixas) continuam aproximações. Plataformas
estáticas que o pássaro já sobrepõe ao nascer no estilingue ficam sem colisão
até a separação, porque algumas rampas são aproximadas por caixas giradas.

Faltam as demais páginas e episódios, outros pássaros e habilidades, música e refinamento de áudio,
fidelidade do motor e testes no PS2 físico.

A segunda página inclui Level52, Level34, Level42, Level24, Level88, Level36, Level31, Level21, Level41, Level76, Level38, Level35, Level20 e Level26. O importador lê as tabelas completas de world, incluindo aliases e chaves calculadas; isso recupera objetos omitidos nas versões anteriores. O preto usa raios e parâmetros recuperados, com impulso radial, queda linear e temporização aproximados pelo port, sem oclusão.

WoodBlock7 e LightBlock7 usam colisão circular, conferida nas definições recuperadas; isso também corrige a peça de madeira da fase 1-3.

A física continua em passos de 120 Hz, usando o tempo transcorrido do relógio do PS2SDK (até 100 ms recuperados por quadro). Isso preserva o ritmo da partida e da mira quando uma cena renderiza abaixo de 60 FPS, sem garantir 60 FPS em cenas pesadas.

A versão 0.17.0 lê saves v1 (96 bytes) e v2 (160 bytes) e grava v3 (288 bytes). Os recordes e desbloqueios são preservados; na primeira gravação da migração, a cópia anterior permanece no outro slot. Versões antigas do port reconhecem o cabeçalho v3 como mais novo e recusam sobrescrevê-lo. A capacidade de 64 posições prepara a continuação da campanha; esta versão contém 84 fases jogáveis.

O branco usa raio/densidade/restituição recuperados. O ovo usa densidade/restituição e valores de explosão de EggGranade; raio inferido da largura do sprite, velocidades de queda/recuo e propagação radial são aproximações do port. Melancias e pneus usam discos sólidos simplificados, com o modelo de materiais existente; pneus não reproduzem a elasticidade/forma interna do original. O atlas do ovo vem de PNG e requer Pillow para regeneração. Há dez canais de efeitos: soltar o ovo e explodir usam canais diferentes.

## Novidades da 0.15.0

A segunda página fica completa com Level66, Level85, Level27, Level32, Level72, Level90 e Level96 (fases 2-15 a 2-21), na ordem de episodes.lua.

- Caixas de TNT (ExtraBlockTNT) explodem ao serem destruídas: raio de impulso 10, força 30000, raio de dano 5 e dano 150, valores recuperados. Uma caixa destruída por outra explosão detona 0,1 s depois (aproximação do port), produzindo reações em cadeia. Uma caixa que apenas cai para fora do mundo não explode.
- Novos objetos com sprites originais: porco de bigode (GrandpaPiglette, raio 2,2), madeira redonda WoodBlock8, pilar, diamante, cálice e banana.
- As máscaras de terreno (GROUND_BLOCK_01 a 05) são guardadas em meia resolução, com alfa filtrado 2×2, e desenhadas em escala 2. Como são formas chapadas tingidas em tempo de execução, isso liberou espaço para 12 sprites novos sem aumentar a VRAM: 144 sprites em três páginas de 512 de largura, os mesmos 2,5 MiB.
- Saves da 0.14.0 (35 fases) são aceitos: vencer a 2-14 libera a 2-15.

Trechos das definições recuperadas estão em `ps2/data/tnt_definitions_excerpt.lua`. Faltam a terceira página de Poached Eggs e os demais episódios.

## Novidades da 0.16.0

O episódio Poached Eggs fica completo: a terceira página traz Level43, Level77, Level28, Level29, Level87, Level18, Level91, Level49, Level45, Level75, Level51, Level30, Level79, Level40, Level59, Level58, Level95, Level82, Level22, Level89 e Level81 (fases 3-1 a 3-21), na ordem de episodes.lua.

- Rei porco (KingPiglette, raio 2,825, resistência 100) na fase final 3-21, com quadros guardados em meia resolução e desenhados em escala 2.
- Novos objetos com sprites originais: escada, pneu médio, dado, ovo roubado, rosquinha e a Estrade03.
- Limite por fase passou de 128 para 160 objetos; a 3-21 tem 135.
- Atlas de 152 sprites em quatro páginas (512×512, 512×512, 512×256, 512×128), 2,75 MiB; sobram 160 KiB de VRAM após os frame buffers.
- Saves da 0.15.0 (42 fases) são aceitos: vencer a 2-21 libera a 3-1.

- Não foi encontrado roteiro vencedor para 3-2, 3-19, 3-20 e 3-21 com a física aproximada do port; elas podem bloquear o avanço sequencial.

Trechos das definições estão em `ps2/data/page3_definitions_excerpt.lua` e a ordem da página em `ps2/data/episode_page3.decompiled.lua`.

## Novidades da 0.17.0

Começa o episódio Mighty Hoax: fases 4-1 a 4-21 (LevelP2_103 a LevelP2_95), na ordem de episodes.lua.

- Blocos novos de madeira, vidro e pedra: quadrado 4×4, prancha 10×1 e dois triângulos com colisão poligonal recuperada (retângulo e isósceles).
- Extras novos: presunto, ovo dourado e bandeira.
- Bancos de textura: as texturas ficam em dois bancos na memória principal (Poached Eggs e Mighty Hoax) e são reenviadas à VRAM quando a fase muda de episódio. A VRAM usada continua 2,75 MiB.
- No banco de Mighty Hoax, o piscar dos porcos e os estados intermediários de dano das peças grandes reutilizam quadros vizinhos.
- Save versão 4 (544 bytes, até 128 fases). Saves v3 da 0.16.0 são lidos; vencer a 3-21 libera a 4-1.
- Regra do port, ausente no original: três derrotas seguidas numa fase liberam a próxima. Ela existe porque 3-2, 3-19, 3-20, 3-21 e 4-4 não têm vitória encontrada com a física aproximada.
