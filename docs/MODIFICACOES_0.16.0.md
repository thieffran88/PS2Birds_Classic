# Modificações completas — PS2Birds Classic 0.16.0

- Conferida a pasta do Drive antes de começar: nenhuma alteração desde a 0.15.0.
- Expandida a campanha de 42 para 63 fases, completando o episódio Poached Eggs com a terceira página (3-1 a 3-21).
- Ordem oficial recuperada de episodes.lua: Level43, Level77, Level28, Level29, Level87, Level18, Level91, Level49, Level45, Level75, Level51, Level30, Level79, Level40, Level59, Level58, Level95, Level82, Level22, Level89 e Level81; trecho incluído em `ps2/data/episode_page3.decompiled.lua`.
- Recuperados e decompilados os 21 scripts com o unluac de referência, sem executar Lua; incluídas as fontes, JSON de fase, enquadramentos e descritores C++.
- Adicionado o rei porco (KingPiglette): raio 2,825, densidade 1, resistência 100 e defesa 0 recuperados; aparece na fase final 3-21.
- Aceita defesa 0 no importador, como na definição original do rei; o motor já aplica defesa mínima 1 no cálculo de dano.
- Adicionados com sprites originais: escada (ExtraBlockStairs), pneu médio (ExtraTire02), dado (ExtraBlockDice), ovo roubado (ExtraStolenEgg, defesa 20000), rosquinha (ExtraDonut01) e a plataforma Estrade03.
- Limite de objetos por fase elevado de 128 para 160 no importador e no núcleo; a 3-21 tem 135 objetos.
- Atlas ampliado de 144 para 152 sprites.
- Quadros do rei guardados em meia resolução, com filtro 2×2 ponderado pelo alfa, e desenhados em escala 2; ele aparece em uma única fase.
- Acrescentada uma quarta página de textura de 512×128: layout 512×512, 512×512, 512×256 e 512×128, total de 2,75 MiB.
- Orçamento de VRAM elevado de 2,5 para 2,75 MiB, mantendo 160 KiB livres dos 2,9 MiB disponíveis após os dois frame buffers de 16 bits.
- Renderizador atualizado para o rei, os novos objetos e a Estrade03.
- Corrigido defeito introduzido na 0.15.0: ao inserir o porco de bigode, o porco padrão da tabela de sprites passou a ser ele em vez do porco pequeno. O padrão agora é escolhido pelo nome.
- Adicionado `ps2/data/page3_definitions_excerpt.lua` com as definições recuperadas e parâmetros documentados em `classic_actor_parameters.json`.
- Adicionado `ps2/tests/beam.py`, busca em feixe por pássaro sobre o `solver3`, com janela de refinamento; não entra no ELF.
- Encontrados roteiros vencedores para 17 das 21 fases novas: 12 com tiro único e 5 com sequências de três ou quatro tiros.
- Não foi encontrado roteiro vencedor para 3-2 (Level77), 3-19 (Level22), 3-20 (Level89) e 3-21 (Level81), mesmo com feixe de largura 5. Elas carregam, contam objetos corretamente e não vencem sozinhas; o teste libera a fase seguinte diretamente para seguir o restante da campanha.
- Teste de campanha ampliado para 63 fases; as 42 pontuações anteriores ficaram idênticas.
- Teste de sprites ampliado: 152 quadros, layout de quatro páginas, redução do rei com valores conhecidos e margem de VRAM.
- Teste de procedência ampliado: ordem das 21 fases da terceira página e raio do rei conferidos com a fonte.
- Save v3 mantido. Saves da 0.15.0 (42 fases) são aceitos: vencer a 2-21 libera a 3-1; teste de expansão 42→63.
- Atualizados README, histórico, validação e versão exibida 0.16.0; gerados ELF, ISO e ZIP de fontes com conferência de integridade.
- Substituídos no Drive os artefatos e relatórios da 0.15.0; mantidos APK, decompilação original e estudo de compatibilidade.

O port continua parcial. Faltam os demais episódios (Mighty Hoax em diante), o pássaro grande, música, refinamento de física e cenários, validação no PCSX2 e no PS2 físico. As quatro fases sem roteiro vencedor podem ser impossíveis com a física aproximada do port e bloqueiam o avanço sequencial do jogador.
