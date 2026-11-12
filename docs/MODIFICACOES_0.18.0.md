# Modificações completas — PS2Birds Classic 0.18.0

- Removida a regra das três derrotas criada na 0.17.0, a pedido do usuário. O avanço volta a ser o do jogo original: só a vitória libera a fase seguinte.
- Investigadas as fases sem vitória (3-2, 3-19, 3-20, 3-21 e 4-4) comparando o importador com os scripts originais recuperados. Foram encontrados três erros presentes desde as primeiras versões do port.
- Corrigida a altura do chão. O objeto Ground de blocks.lua é uma caixa sem tamanho próprio, com centro em y=5; em todas as 84 fases, os objetos apoiados no chão têm a base em y≈0. A superfície agora fica em Ground.y − 5. Antes, o chão estava 5 unidades abaixo e todo o cenário caía e se desarranjava ao iniciar.
- Corrigida a posição do estilingue. `SlingshotSystem.setPositionByStartObject` (Slingshot.lua) coloca o estilingue sobre o primeiro pássaro da fila: x − 0,1 e y − (199 − 30)/20 + raio + 0,1, com a altura e o pivô do sprite SLING_SHOT_01_BACK. O port usava `birdZeroPos`, que só posiciona a fila de espera. Na 3-2, por exemplo, o estilingue fica no alto de uma plataforma (30,0; −30,6), e o port lançava do chão (9,7; 0,1).
- Corrigida a velocidade de lançamento. `gamelogic.lua` define `defaultForce = -925` em `updateValues` e `physicsScale = 1/20`; `launchBird` aplica impulso de −defaultForce × physicsScale × massa × esticamento, ou seja, 46,25 unidades/s com o estilingue esticado ao máximo, igual para todos os pássaros e independente da câmera. O port dividia a força pela escala da tela, de 23 a 152 unidades/s conforme a fase.
- A força passou a ser a porcentagem de esticamento (20 a 100%) e o ângulo foi ampliado de 10°–75° para −30°–85°, como o arrasto livre do estilingue original. A força e o ângulo também são limitados no momento do lançamento.
- A prévia da trajetória usa a nova velocidade física convertida para a tela.
- O enquadramento das fases foi recalculado com o chão e o estilingue corrigidos (exceto a 1-1, que mantém o enquadramento fixo).
- Conferido que as máscaras de plataforma estática são retangulares (93% a 100% de cobertura), confirmando a colisão em caixa.
- Conferido que o dano por impacto (redução de resistência) é calculado no código nativo do jogo, ausente dos scripts Lua; o cálculo de dano do port continua aproximado.
- Refeita a busca de roteiros nas 84 fases com a física corrigida: tiro único em grade de −10° a 80° e força de 30% a 100%, depois busca em feixe por pássaro (largura 3 e 5) para as que não venceram.
- Resultado: 82 das 84 fases com vitória verificada, contra 79 antes. As antes impossíveis 3-2, 3-20, 3-21 e 4-4 agora vencem.
- Continuam sem roteiro 3-9 (Level45, sete porcos e três pássaros vermelhos, vitória depende de derrubar uma torre) e 3-19 (Level22, porcos com capacete e um avô cercado por paredes). As duas dependem de dano por queda e esmagamento, a parte não recuperada do motor.
- Todas as pontuações de referência do teste de campanha mudaram, porque a física mudou para todas as fases.
- Adicionado `ps2/tests/make_campaign_table.py`, que monta a tabela do teste de campanha a partir dos resultados dos buscadores.
- Testes novos: velocidade de lançamento igual em fases com zoom diferente; chão em Ground.y − 5 e estilingue sobre o primeiro pássaro em todas as fases; ponto do estilingue e altura do chão conferidos pelo importador.
- Testes isolados (preto, branco, TNT, triângulo, amarelo) convertidos para a nova escala de força.
- Atualizados README, histórico, validação e versão exibida 0.18.0; gerados ELF, ISO e ZIP de fontes.
- Substituídos no Drive os artefatos e relatórios da 0.17.0; mantidos APK, decompilação original e estudo de compatibilidade.

Fases 3-9 e 3-19 continuam bloqueando o avanço sequencial até serem vencidas pelo jogador. Faltam a segunda página de Mighty Hoax, os demais episódios, o pássaro grande, música e testes no PCSX2 e no PS2 físico.
