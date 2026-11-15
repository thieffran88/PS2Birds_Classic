# Modificações 0.63.0

- **Egg Defender — tiros dos canhões** (`EggDefenderGadget_Cannon.lua`, `EggDefenderCannonBallManager.lua`)
  - Ao lançar o pássaro, cada canhão prevê a trajetória dele; se ela passar a menos de 15 m e sobrar ao menos 0,4 s para a bala chegar, o pavio acende.
  - Durante o voo a previsão é refeita a cada 5 quadros; perto do pássaro o canhão atira logo.
  - A bala sai da boca com impulso de −1500 no eixo do canhão, e o canhão recua com 30 % disso. São no máximo 10 tiros por canhão.
  - Balas que saem da área de jogo são removidas. A bala não colide com o próprio canhão.
  - `CAMPAIGN.BIN`: 2 balas escondidas por canhão (marca 32, `OBJECT_CANNON_BALL`), reaproveitadas como as bolhas das armas; 22 balas nas fases Egg Defender. Não contam como blocos.
  - O desenho da bala (`BLOCK_APPLE`, como no jogo) entra no banco 64.
- **Testes**
  - `globe_test`: na Egg Defender 9, o pássaro lançado faz o canhão atirar em até 1 s.
  - `sprite_test` (banco 64 com 73 figuras), `profile_test` e a tabela da campanha consideram as balas.
  - Suíte com 73 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela, log e README em 0.63.0.
  - `docs/EGG_DEFENDER.md` e `docs/LACUNAS_PORT.md` atualizados.
  - Documentos da 0.62.0 substituídos.
  - Sondas temporárias apagadas da pasta de trabalho.
  - Entregues ISO, ELF (com os .BIN), Gerador para Windows e Código-Fonte; os da 0.62.0 removidos.
