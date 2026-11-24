# Boom Box (power-up da loja): investigação da pendência 32b

Conclusão: **o Boom Box não pode ser reproduzido a partir do APK do Classic.** O script existe, mas os dados que ele usa não estão no APK. Nenhum código foi alterado.

## O que o script faz (`powerups/Powerup_BoomBox.lua`, decompilado com o unluac)
- `activateBoomBox` escolhe 3 pontos livres dentro de `BoomBoxSpawnRect` (retângulo `left/right/top/bottom` da fase), sem sobrepor outras peças (área de teste de 3,22 × 3,174 m), e ordena-os por x.
- `updateBoomBox` solta uma caixa por vez (`makeTNTDelivery`): cria o objeto `MagicBoxProtected` com defesa 9.000.000, mais um `Parachute` preso por junta (resistência do ar 20), com fumaça (`smokeBuff`) e o som `magic_box_appear_0N`. Quando a caixa pousa, o paraquedas some, a defesa original volta depois de um tempo, e quando ela é atingida vira o objeto `MagicBox`, com o som `magic_box_reveal_0N` e depois `magic_box_explode_0N`. O power-up termina quando as 3 caixas acabam.
- A câmera faz um zoom (0,5) em direção ao centro do retângulo enquanto as caixas caem (`cameraBoomBoxMode`); um toque encerra o zoom.
- O `MuseumMode.lua` imprime "No TNT spawn rect specified for level": o Boom Box nasceu como uma entrega de TNT.

## O que falta no APK
- **As definições `MagicBox`, `MagicBoxProtected` e `Parachute` não existem em nenhum arquivo do APK.** Procurei em todos os scripts, nas categorias de `assets/data/blocks`, nos materiais e nas strings de `libAngryBirdsClassic.so`: o nome só aparece no próprio `Powerup_BoomBox.lua`. Sem as definições não há raio, força, dano, massa nem forma da explosão.
- **Não há arte:** nenhum quadro de folha chama-se `MAGICBOX`, `PARACHUTE` ou parecido (nem os ícones `INGAME_POWERUP_MAGICBOX`, `UI_POWERUPS_MAGICBOX`, `REWARDS2_MAGICBOX`, `SHOP_TUTORIALS_MAGICBOX` que o `PowerupDefines.lua` cita).
- **Não há sons** `magic_box_*` entre os arquivos de áudio (só `tnt box explodes.ogg`, do TNT comum).
- No jogo original, `createObject` dessas definições devolveria nada: o power-up só funcionaria com arquivos que este APK não traz.

## O que o APK tem
- `BoomBoxSpawnRect` está em **1.055 fases** (toda fase do Classic traz o campo), mas só **54** têm um retângulo preenchido; nas outras os quatro valores são 0, e o Boom Box não teria onde largar as caixas. O importador do port ainda não lê o campo.

## Decisão
Não inventei uma versão: sem a definição das caixas, qualquer explosão seria um número arbitrário, e sem arte o jogador veria caixas genéricas. Se algum dia as definições aparecerem (outro APK do Classic, por exemplo), a implementação é direta: ler `BoomBoxSpawnRect` no `import_level.py`, gravar no `CAMPAIGN.BIN`, e reaproveitar a explosão exata do TNT (`radial_blast`).
