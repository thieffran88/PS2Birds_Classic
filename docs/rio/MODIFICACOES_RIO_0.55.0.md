# Modificações — Rio 0.55.0 (sons dos macacos e do metal) e Classic 1.6.1

## Adicionado (Rio)
- **Macacos-alvo:** ao ser destruído toca `monkey dies a1`/`a2` no lugar do som da gaiola; enquanto a fase espera o próximo tiro e ainda há macaco-alvo vivo, um macaco "fala" de vez em quando (`monkey idle a1..a3`, a cada 6 a 12 s, sorteado). O jogo agora conta quantos alvos são macacos (`Game.monkey_goals`) para não tocar isso em fases só de gaiolas.
- **Caixas de metal:** quebrar toca `metal_destroyed_01..03` e uma pancada forte toca `metal_collision_01..03`, sorteados (antes: `scaffolding_break` e nada).
- A família do som vem do nome da definição (`sound_family`, só com o Rio: `monkey` e `metal`); os eventos novos são `GAME_EVENT_MONKEY`, `GAME_EVENT_METAL` e `GAME_EVENT_HIT_METAL`.
- `SOUNDS.BIN` do Rio: 73 efeitos (48 + 25 variantes).

## Editado
- `tools/rio/extract_audio_rio.py` (`RIO_ONLY`, `RIO_VARIANT_GROUPS`), `ps2/src/audio_ids.h`, `game.h`, `game.cpp`, `main.c`, testes de campanha e TNT (máscara de peça quebrada com `GAME_EVENT_METAL`). Versões: Rio 0.55.0, Classic 1.6.1 (só o campo novo do `Game`; o Classic não muda de comportamento).

## Ainda de fora
- Água (entrar e sair), cestas e correntes, Nigel e o chefe dos macacos (`monkeyboss`), fogos, cutscenes. Não conferi no ouvido.
