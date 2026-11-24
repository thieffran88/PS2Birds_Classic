# Modificações — Classic 1.0.0 e Rio 0.49.15

## Classic 1.0.0
- **O Classic passa a ser a versão 1.0.0**, a pedido do usuário: ele roda muito bem no PS2 físico e não será mexido por um tempo. O jogo é o mesmo da 0.94.24; mudam só o número da versão (título, créditos e tela de diagnóstico, agora vindos de `GAME_VERSION` em `ps2/src/main.c`) e a legenda "PROTÓTIPO" da tela inicial, que virou "VERSAO 1.0.0". O Rio tem o próprio número (`0.49.15`).
- Todo o código novo do som é só do Rio (`#ifdef PS2BIRDS_RIO`); o Classic fica com os mesmos 25 efeitos.

## Rio 0.49.15: som próprio
Fonte: `birds.lua` (`sounds` de cada pássaro), o perfil `audio/profiles/default.lua` e o `gamelogic.lua` do Rio, lidos como dados.
- **Lançamento por pássaro:** boomerang (`bird_06_flying`) e Terence (`big_brother_flying`) deixam de usar o som do vermelho. Vermelho, azul, amarelo, preto e branco já usavam os próprios arquivos do Rio.
- **Habilidade por pássaro:** vermelho (`red_special_1`), boomerang (`boomerang_activate`), Terence (`big_brother_yell`) e globo (`Globe_Bird_Special_Activation_1`) ganham som. O azul divide com `special_group` e o branco solta o ovo com `special_egg` (no banco do Rio o Classic usava os dois trocados), e a explosão do preto usa `special_explosion`.
- **Colisão do pássaro:** o primeiro toque do pássaro lançado toca o `collision` do próprio pássaro (`bird 01..05 collision a1`).
- **Material:** as peças de metal (as que o Rio manda como "extras") tocam `steel_destroyed` em vez do som da madeira. Madeira, vidro (`light_destroyed`) e pedra (`rock_destroyed`) já usavam os arquivos do Rio; a gaiola é `bird cage breaks`.
- **Banco de sons:** 37 efeitos, 667.760 bytes (limite de 1 MiB), `tools/rio/extract_audio_rio.py` (que passa a resolver os apelidos do perfil de som, ex. `special_group` → `special group`), `ps2/src/audio_ids.h` (ids 25 a 36 só do Rio).
- **Sem som de seleção:** `bird_0N_select` e `unselect` aparecem no `birds.lua`, mas não existem no perfil de som nem entre os arquivos do APK: o original não toca nada ali.
- Conferido no emulador: o Rio abre, lança, bate e usa a habilidade sem travar com o banco novo. O áudio em si não dá para ouvir no emulador deste ambiente.

## Excluído
- Notas da versão anterior.

## Falta (som do Rio)
- Sons de colisão e dano de cada material (`wood_collision`, `wood_damage` etc.), de rolagem e de macacos/gaiolas por peça: o port só emite um som por quebra de peça, por classe de material.
