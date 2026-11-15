# Modificações 0.47.0

- **Poção (Power Potion, novo no jogo, `Powerup_SuperSeed.lua`)**
  - No Tutorial 10, L2 durante a mira aumenta o próximo pássaro, uma vez por fase; a tela mostra "L2: POCAO" enquanto ela estiver disponível.
  - O círculo do pássaro é refeito com o raio ×1,5 (Terence ×1,4) e a densidade multiplicada pelo fator do tipo: vermelho 2, azul 3, amarelo 1,5, preto 2, branco 2,5, verde 1,5, Terence 1,01, GlobeBird 2, pássaro de choque 1,45 (a Stella não tem entrada e fica igual).
  - O desenho acompanha o tamanho (campo `scale` do pássaro).
  - O `velocityMultiplierModifier` da poção não é aplicado: o port não tem a passagem do pássaro por material.
- **Investigação**
  - `Powerup_Shockwave.lua` (Tutorial 13) é quase todo animação de introdução; o efeito de jogo fica para a próxima rodada.
- **Testes**
  - `globe_test`: a Poção só pode ser usada uma vez e o pássaro lançado sai com o raio do fator certo.
  - Suíte com 71 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.47.0; README com o controle da Poção; captura do Tutorial 10; documentos e captura da 0.46.0 substituídos; `LACUNAS_PORT.md` atualizado.
  - Entregues ISO, ELF (com os .BIN), Gerador para Windows e Código-Fonte; os da 0.46.0 removidos da pasta de trabalho.
