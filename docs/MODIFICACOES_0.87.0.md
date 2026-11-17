# Modificações completas — PS2Birds Classic 0.87.0

## Mira do pássaro vermelho no Egg Defender: conferida e testada
- **Levantamento:** a mira do vermelho no Egg Defender (`redbird_special.lua`) **já estava no port desde a 0.62.0**, mas a documentação e três comentários ainda diziam que faltava. Ela funciona assim:
  - enquanto o vermelho voa, o porco vivo mais próximo vira o alvo;
  - o veículo que carrega esse porco é avisado (`pigTargeted`) e reage. Por exemplo, na 2-5 o balão puxa e a armação se solta;
  - ao disparar a habilidade, o veículo recebe o segundo aviso (`redBirdTriggered`).
- **Teste novo:** `ps2/tests/red_target_test.cpp`, que passa por todas as fases:
  - em cada fase do Egg Defender que começa com o vermelho, espera os veículos entrarem e lança o pássaro;
  - confere que o aviso de alvo chega antes da habilidade e o de disparo chega uma vez depois dela;
  - numa fase fora do modo, confere que nenhum aviso é enviado.
  - Resultado: os avisos chegam aos veículos em 24 fases do Egg Defender.
- **Para o teste:** a nova função somente leitura `game_red_notifications()` (em `game.h`) informa quantos avisos de cada tipo foram enviados na fase. O comportamento do jogo não mudou.

## Documentação corrigida
- `LACUNAS_PORT.md` e `EGG_DEFENDER.md`: a mira do vermelho saiu da lista de pendências do Egg Defender. Continuam pendentes o `setTargetY`, os tiros dos canhões e a calibragem dos voadores.
- `egg_vehicles.cpp`: três comentários desatualizados ("not in the port") foram corrigidos.

## Cutscenes
- A abertura de Poached Eggs (0.86.0) continua como está. Não acrescentei outras cenas; a lista do que falta segue em `LACUNAS_PORT.md`.

## Conferido
- 82 verificações no PC aprovadas (81 anteriores e o teste novo).
- Os arquivos de dados são os mesmos da 0.86.0; só o ELF mudou.

## Limpeza
- Drive: os documentos e zips da 0.86.0 foram substituídos pelos da 0.87.0.
- Repositório: removidos os documentos da 0.86.0.
- Área temporária: removidas a pasta e os zips da 0.86.0 e os logs.

## Versão
- Tela de pausa, "Sobre" e README: 0.87.0.
