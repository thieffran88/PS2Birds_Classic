> Documento histórico da investigação isolada Rio. As ferramentas agora estão em tools/rio e compartilham tools/ na base PS2Birds; consulte docs/UNIFICACAO_PS2BIRDS.md.

# PS2Birds Rio — investigação 0.1.0

## Resultado desta etapa

- Lidos como dados 44 scripts auxiliares; os 146 demais scripts fora das fases foram recusados pelo leitor restrito, pois contêm código/estruturas não suportadas. Nenhum Lua foi executado.
- Extraídos 87 materiais e 1022 definições, sem definições conflitantes no cruzamento.
- Localizadas as definições de 904 dos 905 identificadores usados nas fases. Ground permanece como caso especial do motor.
- Encontradas 65 definições de alvos usadas nas fases, marcadas por levelGoal: gaiolas, macacos, redes, chefes e outros alvos.
- Lidos estruturalmente os 24 arquivos COMP, com 1380 composições, nas versões 1/2 e contêineres RVIO/KA3D. Campos brutos extras foram preservados; sua semântica ainda requer validação de renderização.
- Nos grupos warehouse/warehouse2: 30 arquivos, 81 definições distintas e 17 classes de objetivo. Todas as referências dos sprites principais foram localizadas, inclusive sub-sprites das gaiolas; isso não equivale a bancos gráficos renderizados/validados no PS2.
- Cruzados 420 aliases do perfil sonoro com os arquivos do APK; nenhum arquivo referenciado ausente. Há parâmetros de volume, repetição e streaming a transportar para os bancos PS2.
- Encontrados limites de estrelas para 458 dos 459 arquivos de fase. Exceção: plane2/Level201.lua, com Nigel_3 e BLOCK_DUMMY_LEVEL_GOAL, precisa de tratamento específico; não foram inventados valores de estrelas.

## O que falta acrescentar ou adaptar

| Área | Evidência encontrada | Trabalho pendente |
|---|---|---|
| Ordem das fases | levelQueryUtils consulta gamelua.levelOrder | Recuperar a ordem canônica e cruzá-la com os arquivos; nomes alfabéticos não são numeração de menu |
| Origem do lançamento/gravidade | Fases não têm os antigos globais birdZeroPos/worldGravity | Confirmar a configuração/origem usada pelo Rio, preservando fila e escala |
| Materiais e colisões | 87 materiais, tipos box/circle/polygon, escalas e vértices | Adaptador Rio, validação de dimensões e formas no motor; resolver o material red referenciado por oito definições |
| Resgate e vitória | 65 definições usadas têm levelGoal | Objetivos por instância, efeitos de fuga/libertação, sons, pontuação e conclusão próprios |
| Gaiolas | 16 tipos iniciais + B_CAGE_BLU_JEWEL; sprites compostos | Montar sprites/animações e tratar a gaiola especial; a regra Dino do Classic adiciona pássaros à fila e não é a regra de resgate Rio |
| Macacos | Em pé, agarrados/pendurados e variantes | Estados, ancoragem, queda, reação ao impacto e objetivos |
| Blu sozinho | Raio 1,4; especialidade BLU; duração 0,9, intervalo 0,2, impulso 3 | Adaptar o algoritmo recuperado, contatos e apresentação; parâmetros encontrados não bastam para declarar a habilidade pronta |
| Blu/Jewel | Especialidade BLU&JEWEL, corpo e parâmetros próprios | Adaptar comportamento conjunto e efeitos, aproveitando apenas as partes compatíveis do bumerangue |
| RocketBird | Especialidade ROCKET | Movimento, impulso, explosões/efeitos e testes próprios |
| Nigel | Variantes normal/_2/_3/_BOAT e vida declarada | Estados de chefe, dano/vulnerabilidade, movimento e conclusão; conferir cada variante |
| Luiz e resgates especiais | Luiz aparece em um arquivo de fase; redes de golfinhos também são alvos | Comportamentos de companheiro e libertação próprios, além das gaiolas comuns |
| Coletáveis e bônus | SPECIAL_FRUIT, peixe dourado, moedas e dados de penas/estrelas | Registrar coleta/desbloqueios por fase e avaliar ampliação do estado persistente Rio |
| Interações | BOMB, FIREWORK, forças, floatability, fusíveis e objetos móveis | Reutilizar TNT/forças/juntas onde compatíveis; acrescentar fogos, propagação, flutuação e interações específicas |
| Power-ups | Dados incluem SambaBurst, SUPERPOWERUP e consumíveis | Definir suporte offline/controles e adaptar efeitos; loja/anúncios/rede não são requisitos do ciclo básico de resgate |
| Sprites | DAT/SPRT e COMP lidos estruturalmente | Montagem, pivôs, escalas, transformações, espelhamento, animações e seleção de bancos por fase |
| Texturas | PNG/WebP e PVR RGBA4444/RGB565/alpha8/ETC1 | Conversão offline, incluindo ETC1 quando necessário; conferir ordem de canais e VRAM |
| Som/música | 420 aliases com arquivos disponíveis; MP3 | Selecionar/converter efeitos e músicas, transportar volumes/loops e mapear canais ao runtime |
| Apresentação | Temas, camadas de fundo, interface, tutoriais e localização | Adaptar THEMES/UI/FONT/MENU e cutscenes próprias do Rio |
| Estrelas | 458 tabelas encontradas | Mapear silver/gold/eagle e resolver a exceção Level201 sem valores inventados |
| Progresso | Codec e slots alternados existentes no Classic | Identidade e diretório exclusivos Rio; conferir objetivos/desbloqueios e evitar mistura com o Classic |
| Limites | Até 868 objetos brutos; três arquivos acima de 640 | Separar decoração/dicas de corpos físicos e medir antes de ampliar estruturas; juntas encontradas chegam a 396 |
| Validação | Dados recuperados, catálogo cruzado e ferramentas reproduzíveis | Primeira fase executável, campanha, testes no PCSX2 e console físico, desempenho e fidelidade |

## Alterações completas em tópicos

### Adicionado

- tools/investigate_rio.py: comando único com validação do SHA-256/CRC, recuperação sem execução, leitura restrita de tabelas e geração dos inventários.
- tools/inspect_levels.py: versão portátil da inspeção de 459 fases e comparação de nomes com o perfil Classic.
- tools/inspect_dat.py: versão portátil da leitura das 228 folhas SPRT e conferência dos retângulos PNG/WebP.
- tools/inspect_comp.py: leitor estrutural COMP v1/v2, incluindo registros extras brutos, posições assinadas e verificação de consumo completo dos arquivos.
- tools/build_catalogue.py: catálogo com procedência, material+definição, contagem de uso, conflitos e referências ainda não resolvidas.
- tools/audit_gaps.py: cruzamento de objetivos, áudio, estrelas, dependências dos 30 primeiros arquivos e verificações de consistência.
- vendor/classic092/tools: cinco leitores/decodificadores reutilizados, sem alteração, da base Classic fornecida.
- vendor/classic092/profiles: perfis de jogo e catálogo Classic para comparação; não são novos adaptadores Rio executáveis.
- README.md, requirements.txt e docs/PROCEDENCIA.md: execução, dependências e identificação da base reutilizada.
- analysis/data_tables: 44 tabelas auxiliares recuperadas, disponíveis localmente e regeneráveis a partir do APK.
- analysis/data_tables_inventory.json: índice novo das tabelas e scripts recusados pelo leitor de dados.
- analysis/definition_catalogue.json: 1022 definições e referências de material.
- analysis/comp_inventory.json: 24 arquivos e 1380 composições, sem falha estrutural.
- analysis/first_episode_dependencies.json: matriz das 81 definições dos grupos warehouse/warehouse2 e seus sprites/alvos.
- analysis/feature_matrix.json: lacunas, cobertura de áudio/estrelas, materiais e objetivos.
- docs/INVESTIGACAO_0.1.0.md: este levantamento e registro de alterações.
- ZIP de fontes das ferramentas de investigação, com relatórios/evidências resumidas. APK, chunks, texturas, fontes completas Classic, SDK e BIOS não integram o pacote.

### Editado

- ANALISE_PORT_RIO.md: acrescentados os resultados desta investigação, mantendo a distinção entre leitura de dados e execução PS2.
- Inventários recovery/levels/DAT: regenerados pelas ferramentas portáteis para validar reprodução; os conteúdos do APK continuam apenas como entrada de leitura.
- Leitores exploratórios de fases/DAT foram adaptados para localizar arquivos pela raiz do projeto, em vez de caminhos absolutos da sessão.

### Excluído

- Cópias locais exploratórias analysis/inspect_levels.py, analysis/inspect_dat.py, analysis/inspect_comp.py e analysis/build_catalogue.py, substituídas pelas ferramentas portáteis de mesmo nome em tools/.
- Cópias locais temporárias analysis/variantconfig.json e analysis/ingamenative.sheet.json: usadas para conferir se continham ordem de fases; são configurações promocionais/variantes, sem essa ordem. O conteúdo original permanece no APK.
- Nenhum arquivo anterior do Drive foi excluído nesta etapa: a pasta Rio só continha o APK, necessário à geração futura. Não havia ELF, ISO ou fonte Rio anterior a substituir, nem outro arquivo irrelevante identificado.

## Testes e escopo

O ZIP foi extraído em uma pasta separada e a auditoria completa passou novamente com os mesmos resultados. A rejeição de entrada com SHA-256 diferente também foi verificada. A geração dos inventários foi executada novamente com os leitores portáteis, sem usar o diretório de fontes Classic original. Verificações: 649 chunks, 459 fases, 44 tabelas auxiliares, catálogo sem conflitos, 904 identificadores usados resolvidos, 228 SPRT, 24 COMP, 420 aliases sem arquivo ausente e cobertura de estrelas 458/459.

Esta versão é de investigação e preparação. Nenhuma ISO/ELF Rio foi gerada: o runtime ainda precisa das adaptações listadas. O próximo marco é uma fase de Smugglers' Den com lançamento, quebra de gaiola, objetivo de resgate, vitória e save exclusivo Rio.
