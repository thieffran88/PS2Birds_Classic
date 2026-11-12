# Modificações completas — PS2Birds Classic 0.17.0

- Conferida a pasta do Drive antes de começar: nenhuma alteração desde a 0.16.0.
- Iniciado o episódio Mighty Hoax: fases 4-1 a 4-21 (primeira página), campanha ampliada de 63 para 84 fases.
- Ordem oficial recuperada de episodes.lua: LevelP2_103, 91, 65, 96, 69, 88, 64, 80, 108, 85, 82, 66, 104, 210, 83, 79, 77, 114, 81, 68 e 95; trecho incluído em `ps2/data/episode_mighty_hoax_page1.decompiled.lua`.
- Recuperados e decompilados os 21 scripts da pasta `levels/2_MightyHoax` com o unluac de referência, sem executar Lua.
- O gerador de campanha passou a aceitar pasta de origem e banco de textura por página do episódio.
- Adicionados doze blocos novos (madeira, vidro e pedra): quadrado 4×4 (9), prancha 10×1 (10), triângulo retângulo (11) e triângulo isósceles (12), com resistências recuperadas.
- Implementada colisão poligonal para os triângulos com os vértices recuperados de blocks.lua: (0,0) (1,1) (0,1) e (0,5;0) (1,1) (0,1), normalizados sobre o tamanho do sprite e centrados na origem do corpo.
- O campo de forma dos objetos passou de "círculo sim/não" para um código: caixa, círculo, triângulo retângulo e triângulo isósceles.
- Adicionados extras com sprites originais: presunto, ovo dourado e bandeira.
- Criados bancos de textura: os sprites ficam em dois bancos na memória principal (Poached Eggs e Mighty Hoax), e o banco da fase ativa é reenviado à VRAM quando o episódio muda.
- As quatro posições de página na VRAM são reservadas uma vez pelo maior tamanho entre os bancos; o uso continua em 2,75 MiB, com 160 KiB livres.
- O banco 0 é idêntico ao atlas da 0.16.0. O banco 1 contém os sprites comuns (pássaros, estilingue, ovo e máscaras de terreno), os porcos e blocos usados nas fases de Mighty Hoax e os 51 sprites novos.
- No banco 1, o piscar dos porcos e os estados de dano 2 e 3 das peças 4×4 e triangulares reutilizam quadros vizinhos (23 apelidos), para caber na VRAM. Poached Eggs não muda.
- O extrator calcula automaticamente quais sprites cada banco precisa a partir das fases geradas e rejeita sprites inexistentes.
- O ELF passou a carregar 5,5 MiB de texturas na memória principal (antes 2,75 MiB); a VRAM usada não mudou.
- Renderizador atualizado para os blocos 9 a 12, os três extras e a troca de banco no início de cada quadro.
- O gerador do ícone de salvamento lê o novo manifesto por bancos; o ícone ficou idêntico byte a byte.
- Save versão 4: 544 bytes e até 128 fases, com mesma assinatura e CRC32. Lê v1, v2, v3 e v4 e grava v4.
- Saves v3 da 0.16.0 são aceitos: vencer a 3-21 libera a 4-1, e a primeira gravação v4 vai para o outro slot, preservando a cópia v3.
- Capacidade de campanha elevada de 64 para 128 fases.
- Nova regra do port, que não existe no jogo original: três derrotas seguidas numa fase liberam a próxima, e uma vitória zera a contagem. A tela de derrota avisa quando isso acontece.
- Motivo da regra: cinco fases não têm vitória encontrada com a física aproximada do port (3-2, 3-19, 3-20, 3-21 e 4-4). Sem ela, a 3-2 bloquearia o acesso a todo o Mighty Hoax.
- Encontrados roteiros vencedores para 20 das 21 fases de Mighty Hoax: 17 com tiro único e 3 com sequências de três ou quatro tiros (4-3, 4-10 e 4-14).
- Adicionado teste isolado de triângulos: os dois tipos ficam em pé ao lado de uma caixa, apoiados no chão, e são atingidos por tiros baixos.
- Teste de campanha ampliado para 84 fases; as 63 pontuações anteriores ficaram idênticas. Inclui teste da regra das três derrotas.
- Testes de sprites reescritos para bancos: 203 sprites, 302 posicionamentos, sem sobreposição, hashes por quadro, apelidos válidos e tamanho das posições de página.
- Teste de save ampliado: layout v3 codificado de forma independente, corrupção byte a byte, migração v3→v4 e limite de 128 fases.
- Teste de procedência ampliado: ordem das 21 fases de Mighty Hoax e formas dos blocos conferidas com a fonte recuperada.
- Adicionado `ps2/data/mighty_hoax_definitions_excerpt.lua` e parâmetros dos novos blocos em `classic_actor_parameters.json`.
- Atualizados README, histórico, validação e versão exibida 0.17.0; gerados ELF, ISO e ZIP de fontes com conferência de integridade.
- Substituídos no Drive os artefatos e relatórios da 0.16.0; mantidos APK, decompilação original e estudo de compatibilidade.

O port continua parcial. Faltam a segunda página de Mighty Hoax e os demais episódios, o pássaro grande, música, refinamento de física e cenários e testes no PCSX2 e no PS2 físico.
