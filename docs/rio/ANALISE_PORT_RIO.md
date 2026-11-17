> Documento histórico da investigação isolada Rio. As ferramentas agora estão em tools/rio e compartilham tools/ na base PS2Birds; consulte docs/UNIFICACAO_PS2BIRDS.md.

# PS2Birds Rio — análise de reaproveitamento

Data: 17/11/2026. Base consultada: fontes PS2Birds Classic 0.92.0, obtidas da pasta atual do Classic no Drive. A cópia antiga local 0.14.0 não foi usada como referência principal.

## Conclusão

Há reaproveitamento substancial da infraestrutura PS2 e do motor do Classic. A recuperação de scripts e a leitura segura de tabelas de fase já funcionaram neste APK Rio. O principal trabalho será criar adaptadores de recursos/definições e implementar os objetivos e comportamentos específicos do Rio.

Não foi atribuído um percentual de reaproveitamento: contagem de arquivos ou semelhança de nomes não mede esforço de implementação. Este levantamento valida dados e formatos, ainda não execução do Rio em PS2.

## Arquivos e evidência

- APK: `com.rovio.angrybirdsrio.apk`, Android, versão **2.6.13**, versionCode **2676**, pacote `com.rovio.angrybirdsrio`.
- Tamanho: **49.823.743 bytes**; CRC do arquivo ZIP verificado sem falhas.
- SHA-256: `64638e3207f0d2913b6882d1f1edc32e2abb736d938341d45525b0172ae2beb1`.
- Fonte Classic: `PS2Birds_Classic_0.92.0_Codigo_Fonte.zip`, 826.323 bytes. Seu README declara 745 fases e o código contém os dez tipos de pássaro de fila do Classic.
- APK Rio: https://drive.google.com/file/d/1c1VtXmXHTyV0x9MlPkbdvEsohMmma6px/view
- Fonte Classic consultada: https://drive.google.com/file/d/1Td1jekOkdwemPHINukB_D9Qtho1W_VSE/view
- Arquivos do Classic e APK Rio preservados. Foram criados apenas arquivos locais de análise e cópias de referência; nenhum ELF/ISO Rio foi publicado.

## O que foi verificado na prática

1. Recuperados **649/649 arquivos Lua**, com zero falhas, usando o perfil/chave Rio já existente nas ferramentas do Classic.
2. Todos os chunks recuperados têm cabeçalho Lua 5.1 little-endian/float32 compatível com o leitor de dados atual.
3. Lidos **459/459 arquivos de fase** com `lua51_data.py`, sem executar Lua. O total inclui tutoriais, recompensas e bônus; não é uma confirmação de 459 fases principais na ordem do menu.
4. As fases estão distribuídas por **44 pastas** e contêm, em conjunto, **905 identificadores de definição**. Apenas oito nomes coincidem diretamente com o perfil Classic, principalmente pássaros e Ground. Isso mede diferença de nomenclatura/catálogo, não 897 mecânicas novas.
5. Lidos **228 arquivos gráficos DAT/SPRT**, com **6.182 registros de sprite**, sem falhas de estrutura no leitor exploratório. A estrutura é KA3D/SPRT, com nomes de textura, retângulos e pivôs. Os 24 DAT/COMP foram depois lidos estruturalmente na investigação 0.1.0 (1380 composições); montagem e semântica de transformações permanecem pendentes no runtime.
6. Conferidos limites dos sprites contra as imagens PNG/WebP disponíveis em **120 folhas** (3.789 registros): 20 PNG e 100 WebP. As demais 108 folhas SPRT têm somente PVR entre as alternativas examinadas.
7. Identificados **139 arquivos PVR v3**: 109 RGBA4444, sete RGB565, dois alpha8 e 21 ETC1. A conversão deve respeitar a ordem de canais de cada formato. Os formatos não comprimidos são candidatos diretos a conversão offline; ETC1 exige decodificação própria/ferramenta apropriada.
8. Identificados **422 MP3**: 408 efeitos e 14 músicas. O conversor precisa usar as fontes/aliases Rio e a extensão MP3, mantendo a saída de áudio compatível com o runtime PS2.
9. Recuperados e decompilados, para inspeção estática, scripts de pássaros, objetivos, materiais e lógica. Foram confirmados `levelGoal`, gaiolas, macacos, Blu/Jewel, Rocket e variantes de Nigel. Os scripts não foram executados.
10. Confirmado que o gerador atual Classic rejeita a identidade deste APK, como esperado. O caminho correto é criar um perfil Rio com hash próprio e adaptar suas entradas.

## O que podemos compartilhar

| Camada | Base concreta na 0.92.0 | Trabalho Rio |
|---|---|---|
| Plataforma PS2 | PS2SDK/gsKit, build ELF/ISO, acesso a dados em disco/USB, DualShock, pausa | Nome/identidade/configuração do jogo e recursos próprios |
| Renderização | Sprites rotacionados, fontes/UI, fundos e bancos de texturas carregados conforme a fase | Converter DAT/SPRT/COMP e PNG/WebP/PVR; construir bancos Rio dentro do orçamento de VRAM |
| Áudio | Efeitos ADPCM, canais, música e controle de volume | Mapear aliases/MP3 e gerar SOUNDS.BIN/MUSIC.BIN Rio |
| Física | Box2D, corpos, materiais, juntas, destruição, explosões e forças | Importar dimensões, propriedades, escalas e parâmetros reais do Rio |
| Pássaros compartilhados | Vermelho, azul, amarelo, preto, branco, bumerangue, Terence, inflável e Stella têm base no Classic | Mapear nomes como WhiteBird/R2_WhiteBird e variantes R2; conferir parâmetros e diferenças de comportamento |
| Ferramentas | Decodificação, leitor Lua de dados, formatos intermediários e geração dos bancos | Adaptador de layout de fases e registro de definições Rio |
| Progresso | Codec com CRC, cópias alternadas, recordes e desbloqueios | Diretório/identificador exclusivos do Rio e ordem de campanha própria |
| Testes | Regressões de física, habilidades, saves, sprites e ferramentas | Acrescentar casos de resgate, macacos, personagens Rio e chefes |

O runtime atual já consome CAMPAIGN.BIN, SPRITES.BIN, SOUNDS.BIN, MUSIC.BIN, UI.BIN, FONT.BIN, MENU.BIN e THEMES.BIN. Essa separação é uma boa base para gerar os mesmos tipos de banco com conteúdo Rio, após confirmar compatibilidade dos registros e acrescentar os campos necessários.

## Diferenças que exigem implementação

### Metas de resgate

O Rio usa `levelGoal` nas definições. As gaiolas, os macacos e certos alvos especiais devem ter comportamento e condição de conclusão próprios. O contador de porcos do Classic precisa ser generalizado para objetivos da fase, com pontuação e efeitos por definição.

A 0.92.0 já possui liberação de pássaros de gaiolas Dino: eles entram na fila de ataque. Isso oferece ganchos úteis de destruição/spawn, mas o resgate do Rio precisa de regra própria. Libertar um prisioneiro não deve automaticamente colocá-lo na fila de ataque.

### Personagens e chefes

- Blu sozinho e Blu/Jewel: parâmetros e habilidades próprios, incluindo a especialidade `BLU&JEWEL` encontrada nos scripts. Há algoritmos compartilháveis com a base de bumerangue, sujeitos a adaptação.
- RocketBird: habilidade específica a integrar e testar.
- Macacos: variantes em pé, agarrados à parede e pendurados; postura, ancoragem e reação precisam ser conferidas.
- Nigel e variantes: há definições `BIRD_RIO_NIGEL`, `_2`, `_3` e `_BOAT` nos arquivos de fase. Chefes entram em uma etapa posterior com estados e objetivos próprios.
- Fases posteriores incluem novos alvos, como redes de golfinhos, além de objetos e interações Rio 2.

### Layout e parâmetros

As 459 tabelas lidas não trazem os antigos globais `birdZeroPosX`, `birdZeroPosY` e `worldGravity` esperados pelo caminho Classic. Muitos objetos trazem posição/definição sem as propriedades de material embutidas. O adaptador deve obter esses dados das definições/configuração Rio e conferir como a origem do lançamento é determinada. Não se deve copiar valores padrão do Classic sem evidência.

`counts` pode estar desatualizado: em uma amostra de warehouse havia referências a WhiteBird em counts, enquanto a tabela world continha RedBird. O inventário/importador deve continuar tomando os objetos reais de world como fonte.

A ordem jogável de cada episódio ainda precisa ser recuperada e cruzada com `gamelua.levelOrder`/consultas de episódios. A ordenação alfabética de nomes como Level135 e Level190 não define a numeração do menu.

### Limites e desempenho

A base Classic suporta 640 objetos e 512 juntas por fase e 1024 posições de campanha/save. No Rio, o maior arquivo bruto tem 868 entradas de objeto e o maior número de juntas encontrado é 396. Três arquivos superam 640 objetos brutos. Antes de ampliar limites, separar decoração, dicas e objetos físicos; a contagem bruta não equivale a corpos ativos necessários.

Os primeiros grupos warehouse/warehouse2 têm 30 arquivos, até 149 objetos e 62 juntas por arquivo, abaixo dos limites atuais. São um começo mais controlado. Ainda é necessário medir desempenho/memória no emulador e no PS2 físico.

As bibliotecas do APK são ARMv7 e x86. O executável PS2 virá do runtime próprio C/C++ compilado para MIPS/EE, usando os dados convertidos e as mecânicas implementadas.

## Organização recomendada

- Preservar a 0.92.0 como referência estável do Classic.
- Compartilhar o núcleo PS2/física/áudio/formatos entre os projetos.
- Separar perfis por jogo: identidade, hash do APK, definições/materiais, pássaros, objetivos, recursos, campanha e interface.
- Usar arquivos de dados e diretório de save próprios do Rio, por exemplo `/PS2BRIO`, com identificador distinto no codec. Hoje o Classic usa `/PS2BIRDS` e identidade CLAS; isso precisa ser separado antes da primeira versão jogável Rio.
- Criar um gerador Rio que valide este APK por SHA-256 e produza os bancos do runtime. Manter nomes/extensões/layouts específicos nos adaptadores.
- Reutilizar os comportamentos comuns com parâmetros por jogo; manter gaiolas de resgate, Blu/Jewel e chefes em módulos próprios.
- Acrescentar regressões Classic e Rio para evitar que melhorias compartilhadas quebrem o jogo anterior.

## Sequência prática

1. **Primeira fase verificável de Smugglers' Den:** recuperar a ordem correta, importar uma fase com vermelho/gaiolas, converter seus assets mínimos, gerar os bancos e iniciar no PS2 emulado. Critérios: lançar, quebrar gaiola, contabilizar resgate, concluir/reiniciar e salvar em diretório Rio.
2. **Primeiro episódio:** ampliar para os 30 arquivos warehouse/warehouse2, com alvos especiais como B_CAGE_BLU_JEWEL, fundos, áudio, animações e progressão. Confirmar cada fase contra os dados e os objetivos.
3. **Jungle e mecânicas Rio:** acrescentar macacos e Blu/Jewel; depois Blu sozinho, RocketBird, variantes R2 e interações dos demais episódios.
4. **Chefes e acabamento:** Nigel/variantes, novos alvos, bônus, cutscenes e ajustes de física, memória e desempenho.

O primeiro marco deve ser uma fase Rio com ciclo completo de resgate e progresso no PS2 emulado. Isso valida o reaproveitamento do motor em condições reais antes da expansão de catálogo.

## Evidências locais

- `analysis/evidence_manifest.json`: versões, hashes e procedência.
- `analysis/recovery.json`: resultado da recuperação dos 649 chunks.
- `analysis/levels_inventory.json`: inventário das 459 tabelas e definições.
- `analysis/dat_inventory.json`: estrutura das 228 folhas e verificações de imagens.
- `analysis/pvr_formats.json`: formatos de textura encontrados.
- `tools/inspect_levels.py` e `tools/inspect_dat.py`: verificações exploratórias usadas neste levantamento.

Validação concluída: integridade do APK, recuperação de scripts, leitura de tabelas, metadados gráficos e comparação estática com as fontes atuais do Classic. Execução do Rio no PS2 ainda pendente.

## Atualização — investigação 0.1.0

Foram lidas 44 tabelas auxiliares, identificados 87 materiais e 1022 definições, com correspondência de 904 das 905 definições usadas nas fases (Ground é especial). Os 24 arquivos COMP foram lidos estruturalmente. Para os primeiros 30 arquivos foram cruzadas 81 definições, 17 classes de objetivo e todas as referências dos sprites principais. Há 420 aliases de áudio sem arquivo ausente e tabelas de estrelas para 458/459 fases; a exceção é plane2/Level201.lua.

O levantamento detalhado e as alterações adicionadas/editadas/excluídas estão em `docs/INVESTIGACAO_0.1.0.md`. As ferramentas portáteis de auditoria estão em `tools/`, sem execução de Lua ou bibliotecas Android. Execução PS2 continua pendente.

Também foram identificados Luiz em uma fase e coletáveis (frutas especiais, peixe dourado e outros itens). Seus comportamentos/desbloqueios e estado persistente entram na lista de adaptações.
