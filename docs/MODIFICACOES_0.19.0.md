# Modificações completas — PS2Birds Classic 0.19.0

- Baixado o emulador da pasta Ferramentas pelo link fornecido; SHA-256 igual ao registrado em DEPENDENCIAS.md. PCSX2 v2.9.148 configurado em modo portátil com a BIOS scph39001, rodando sob Xvfb.
- ELF 0.18.0 conferido no PCSX2: inicia a 60 FPS, mostra as 84 fases e carrega as cenas sem erro de VRAM.
- Criados ELFs de teste (não distribuídos) que abrem direto numa fase, para conferir cenas específicas no emulador.
- Confirmado no emulador que o estilingue da 3-2 fica no alto da plataforma, como no original.
- O emulador mostrou a 4-1 desmoronando ao carregar. Uma sonda de acomodação no computador mediu o deslocamento de cada peça antes de qualquer tiro e confirmou desmoronamentos de 10 a 19 unidades em seis fases de Mighty Hoax.
- Corrigido o tamanho físico das peças conforme `ObjectCreation.lua`: caixas, círculos e polígonos dinâmicos usam 92% do sprite (`sizeFactorX/Y`, padrão 0,92); objetos estáticos (densidade 0) usam o sprite inteiro. O port usava 100%, e as peças nasciam sobrepostas.
- Corrigidos atrito e restituição conforme a tabela `materials` de blocks.lua: madeira 4,0/0,2, vidro 0,7/0,2, pedra 4,0/0,1, estático 0,8/0, porco 0,7/0,05 (0,3 nos porcos que declaram atrito próprio) e extras 0,7/0,4. O port usava 0,6/0,03 em tudo.
- O chão passou a usar o material staticGround (atrito 0,8, restituição 0); antes ficava com o padrão do Box2D (0,2).
- Atrito e restituição passaram a fazer parte dos dados de cada objeto da campanha, gerados pelo importador.
- Resultado da acomodação: fases com deslocamento acima de 3 unidades caíram de 13 para 8; a 4-1 aparece de pé no emulador, como no original.
- Conferido nos scripts que o original não coloca corpos para dormir ao carregar a fase; se o motor nativo o faz, não há como saber pelos scripts Lua.
- Refeita a busca de roteiros nas 84 fases: 76 vitórias com tiro único e 7 com busca em feixe.
- Resultado: 83 de 84 fases com vitória verificada, contra 82 na 0.18.0. A 3-9 e a 3-19, sem vitória até a 0.18.0, agora vencem.
- Continua sem roteiro a 3-20 (Level89): cinco pássaros explosivos (três pretos e dois brancos) contra nove porcos, dois deles com resistência 30. A vitória depende do modelo de explosão do port; raio, força e dano vêm dos dados, mas a queda com a distância e a conversão em impulso são aproximações.
- O auxiliar de busca em feixe passou a avaliar as sequências em paralelo e a não reprocessar sequências que já esgotaram os pássaros.
- Adicionadas as sondas `ps2/tests/settle_probe.cpp` e `settle_detail.cpp`, que medem a acomodação inicial de cada fase; não entram no ELF.
- Teste novo: tamanho físico com fator 0,92 nos objetos dinâmicos e tamanho cheio nos estáticos, e atrito e restituição de cada objeto iguais à tabela de materiais.
- Cenários de teste isolados atualizados com os materiais originais.
- Todas as pontuações de referência do teste de campanha mudaram, porque a física mudou.
- Atualizados README, histórico, validação e versão exibida 0.19.0; gerados ELF, ISO e ZIP de fontes, e o ELF final conferido no PCSX2.
- Substituídos no Drive os artefatos e relatórios da 0.18.0; mantidos APK, decompilação original e estudo de compatibilidade.

Faltam a 3-20, a segunda página de Mighty Hoax, os demais episódios, o pássaro grande, música e teste em PS2 físico.
