# Chrono Bird (power-up da loja): investigação da pendência 32c

Conclusão: **o Chrono Bird não pode ser reproduzido fielmente a partir do APK do Classic.** A regra está nos scripts, mas a definição do pássaro e a arte não estão no APK. Nenhum código foi alterado.

## A regra (lida em `powerups/Powerup_ChronoBird.lua` e `subsystems/ChronoBirdAbility.lua`, decompilados com o unluac)
- O power-up troca o pássaro do estilingue por um `ChronoBird` (`replaceBird("ChronoBird")`, o mesmo esquema do Shockwave: o script foi copiado dele, até o ícone `UI_POWERUPS_SHOCKWAVE` e o campo `bonusShockwaveBird`), com fumaça (`bigSmokeBuff`) e o som `super_slingshot`.
- Durante o voo o pássaro grava a cada 1/30 s a posição e a velocidade, guardando só as últimas 60 amostras (2 s).
- Ao toque (`triggerChronoBirdSpecialty`): o corpo vira estático, o tempo físico cai para 0,25 (`physicsSpeedFactor`), o fim da fase fica bloqueado (`g_prevent_level_ending`), um filtro azul-esverdeado (0, 0,15, 0,2; alfa até 0,7 pelo seno) cobre o fundo, e o pássaro é levado de volta pelas amostras, da mais recente para a mais antiga, uma por 1/30 s. Na última amostra ele volta a ser dinâmico e segue com a velocidade gravada: é um "refazer" de até 2 s do voo.
- Um rastro de até 10 imagens `BIRD_CHRONO` com transparência crescente mostra o caminho gravado.

## O que falta no APK
- **A definição `ChronoBird` não existe** em `assets/data/blocks` (nem em nenhum outro script): sem ela não há raio, densidade nem sprites. `replaceBird("ChronoBird")` não teria o que criar.
- **Não há o sprite `BIRD_CHRONO`** em nenhuma folha, nem ícones próprios (o script reaproveita o do Shockwave).
- Os nomes `suctionAirFlowStarting` e `staticGround` que o script cita também não têm definição em dados.
- Só `PowerupManager.lua`, `GameHud.lua`, `level_scripts.lua` e os dois scripts do Chrono Bird mencionam o nome: é um protótipo que o jogo não chegou a completar.

## Decisão
Como na Boom Box (`docs/BOOM_BOX.md`), não inventei os dados que faltam. A parte de regra (gravar e voltar no tempo) é simples e poderia ser feita no runtime; o que seria inventado é o corpo do pássaro e a arte. Se você quiser mesmo assim, a proposta é usar o corpo e o sprite do pássaro vermelho (tingido) e deixar o resto exato; isso mudaria o Classic, então exigiria a versão 1.0.1.
