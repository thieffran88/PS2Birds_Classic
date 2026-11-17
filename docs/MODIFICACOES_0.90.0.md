# Modificações completas — PS2Birds Classic 0.90.0

## `velocityMultiplier` ligado: a física dos pássaros fica mais próxima do original
- **O que é:** o `blocks.lua` define, para cada pássaro, quanto da velocidade ele conserva ao quebrar uma peça. Agora o jogo aplica isso:
  - azul: ×1,9 no vidro;
  - Terence (pássaro grande): ×1,8;
  - bumerangue: ×1,3 na madeira e no vidro;
  - baiacu: ×4 (×2 quando inflado).
- **Regra usada:** quando o pássaro quebra a peça, ele segue com a velocidade de antes da batida × min(1, sobra do dano × multiplicador). Vem do `AdventureModeCollisions.lua`. A fórmula exata do modo Classic é nativa e não foi recuperada, então é uma aproximação.
- **Efeito:** esses pássaros atravessam melhor as peças que quebram, como no original.
- Para desligar, compile com `-DPS2BIRDS_NO_VELOCITY_MULTIPLIER`.

## Roteiros de vitória recalculados
- Ligar o efeito fez 86 fases deixarem de ser vencidas pelos roteiros antigos, que tinham sido calibrados na física anterior.
- A busca automática refez os roteiros:
  - **72 fases:** um tiro repetido para todos os pássaros;
  - **7 fases:** sequências de pássaros diferentes (busca em feixe);
  - **total de 79 fases** com roteiro novo verificado na campanha inteira.
- **7 fases ficaram sem roteiro verificado:** Bad Piggies 2-9, Bird Island 1-5, Piggy Farm 2-5, Piggy Farm 2-14, Dino 1-6, Dino 1-7 e Dino 1-13.
  - A busca não achou vitória para elas; duas delas (Dino 1-6 e 1-7) nem terminaram no tempo disponível.
  - Isso não quer dizer que sejam impossíveis, só que nenhum roteiro foi verificado. O total de fases sem roteiro passou de 71 para 78.
- **Correção de um número que eu tinha passado:** durante a busca, eu disse que ela tinha resolvido 9 das 14 fases difíceis, contando linhas de um arquivo, inclusive as que terminaram vazias. O número real foi 7.

## Conferido
- 82 verificações no PC aprovadas, com o efeito ligado. A do teste da campanha cobre as 745 fases.
- No PCSX2, a 1-1 roda com a cena de abertura, o lançamento e a destruição com partículas (captura em `docs/pcsx2_0.90.0_jogo.png`).

## Documentação
- `LACUNAS_PORT.md`: o `velocityMultiplier` sai das pendências, as linhas antigas foram corrigidas e há uma seção nova "Achados da 0.90.0".

## Limpeza
- Drive: os documentos e zips da 0.89.0 foram substituídos pelos da 0.90.0.
- Repositório: removidos os documentos e a captura da 0.89.0.
- Área temporária: removidos os resultados da busca de roteiros, a pasta e os zips da 0.89.0 e os arquivos de teste.

## Versão
- Tela de pausa, "Sobre" e README: 0.90.0.
