# PS2Birds Classic — protótipo 0.8.0

Foco exclusivo no Classic. Inclui as oito primeiras cenas recuperadas de Poached
Eggs, com sprites originais, pedra, madeira, vidro, pedras circulares e baú, seleção de fases, desbloqueio
sequencial e recordes persistentes no Memory Card 1. Ainda não é o port completo do Classic.

## Jogar

Abra o ELF ou ISO no PCSX2 ou teste no carregador homebrew escolhido. O console
físico ainda não foi validado. Vídeo NTSC 640×448, sem áudio; salvamento no Memory Card 1.

- Menu: cima/baixo escolhem a fase; X inicia se estiver desbloqueada.
- Partida: direcional/analógico esquerdo ajustam ângulo e força; X lança.
- Pássaro azul: após lançar, pressione X novamente antes de colidir para dividir em três.
- Segure Quadrado para ajuste fino; Triângulo reinicia; Start pausa/continua.
- Select abre/fecha a seleção de fases. O jogo pausa enquanto o menu está aberto.
- Vitória: X avança para a próxima fase; após 1-8 retorna à seleção.
- L1 salva/tenta novamente no Memory Card 1. A vitória salva automaticamente.
- Use cartão PS2 previamente formatado pelo navegador do console; o jogo não formata.
- Aguarde o aviso de salvamento terminar antes de remover o cartão.
- Sem cartão, a partida continua com progresso somente da sessão.

## Compilar

```sh
export PS2DEV=/caminho/ps2dev
sh ps2/build.sh -j3
sh ps2/build.sh -j3 test
sh ps2/build.sh iso
```

Requisitos: pacote ps2dev fornecido (PS2SDK/gsKit), make, Python 3.9+, g++ e
OpenSSL para os testes, genisoimage/mkisofs para a ISO. Box2D e texturas convertidas
estão incluídos no código-fonte; o build não precisa do APK nem de downloads.

## Regenerar dados e gráficos

```sh
python tools/build_campaign.py
python tools/extract_sprites.py /caminho/com.rovio.angrybirds.apk ps2/assets --game classic --sevenzip /caminho/7z
python tools/build_save_icon.py
```

As oito fontes Lua decompiladas e métricas estão em ps2/data; o importador não
executa Lua. A extração de gráficos exige o APK original, OpenSSL e 7-Zip. As duas páginas de atlas
usam 1,5 MiB e ficam incorporadas no ELF. As plataformas estáticas novas usam máscaras
originais com cor provisória; o preenchimento original ainda está pendente.

As ferramentas/perfis de outras séries permanecem no pacote, sem novos ports.
Consulte docs/MODIFICACOES_0.8.0.md e docs/VALIDACAO_0.8.0.md. Documentos das versões
anteriores são históricos. Saves da 0.6.0 e da 0.7.0 são aproveitados. Se a antiga última fase foi vencida,
a primeira fase nova é desbloqueada; caso contrário, o bloqueio permanece. A seleção mostra três fases
por página e muda automaticamente conforme cima/baixo.

O azul usa raio 0,6, densidade 4,5 e fator de dano direto no vidro 2,9,
consultados nos dados recuperados. A abertura do leque (±10°), o cálculo de dano
e as colisões de cenário ainda são aproximações. Objetos só recebem dano de
impactos ligados ao lançamento; pressão de repouso não causa desgaste contínuo.

Faltam as demais fases e episódios, outros pássaros e habilidades, áudio,
fidelidade do motor e testes no PS2 físico.
