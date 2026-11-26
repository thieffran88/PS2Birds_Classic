# Modificações — Classic 1.7.0 (fim dos sons do Classic)

O Rio continua na 0.56.0.

## Adicionado
- **Canhão:** cada tiro do `BP_CANNON` toca `cannon_shot_01/02`.
- **Choque elétrico:** um porco eletrocutado (onda do pássaro de choque) toca `pigs_electrocuted_01..03`.
- **Bolos e cupcakes** (`*Cake*`, Birdday): ao quebrar tocam `birthday_cake1/2`.
- **Folhas de palmeira** (`Palm_leaf_*`): ao quebrar tocam `palm_leaves_1..3`.
- **Tubos de metal** (o cano do fogão da Farm, `*Pipe*` sem ser de vidro): ao quebrar tocam `metal_pipe_destroyed_01..03`.
- **Batida dos pássaros rosa e globo:** `pink_collision_01..03` e `Globe_Bird_Hit_1..3` (o rosa e o globo usavam o som do vermelho, ou nenhum).
- `SOUNDS.BIN` do Classic: 105 efeitos (59 + 46 variantes).
- A família do som vem do nome da definição (`sound_family`): no Classic 4 bolo, 5 palmeira, 6 tubo; eventos `GAME_EVENT_CANNON/ELECTRO/CAKE/PALM/PIPE`. No Rio as famílias 4 e 5 continuam sendo macaco e metal.

## Editado
- `tools/extract_audio.py`, `ps2/src/audio_ids.h`, `game.h`, `game.cpp`, `main.c`, testes de áudio, de campanha e de TNT. Versão do Classic: 1.7.0.

## Deixado de fora (decisão)
- **Fole** (`bellows_*`): o fole sopra por uma força contínua, sem um instante de "sopro" no jogo.
- **Joias, passos e ambiente do Dino, `pig_singing`, `pig_snare`/`hi-hat`, trovões (`Lightning`), `bird misc`, `bird shot`:** sem evento correspondente no port (as cenas dos porcos bateristas e das joias são de cutscene ou de efeitos que o port não tem).
- **Batida dos tubos de metal:** o jogo não tem mais bits livres para eventos de 32 bits; só o som de quebrar entrou.
- Não conferi no ouvido.
