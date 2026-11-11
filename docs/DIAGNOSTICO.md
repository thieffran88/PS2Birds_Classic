# PS2Birds Classic — o que falta para o port

**Atualização após examinar Ferramentas:** o SDK e o PCSX2 fornecidos funcionam
neste ambiente. Foi criado o protótipo técnico 0.1.0 com ELF e ISO; consulte
`VALIDACAO_0.1.0.md`. O diagnóstico abaixo descreve as lacunas do port completo.

Análise em 11/11/2026, limitada aos dois arquivos encontrados na pasta do projeto
no Google Drive. Não foi encontrado um port PS2 existente nesses arquivos.

## Resultado principal

Há recursos do jogo e uma ferramenta de recuperação de Lua. Falta a implementação
do motor para PS2. Não é possível produzir o port apenas recompilando esse ZIP ou
transformando o APK em ELF/ISO.

## Material verificado

| Arquivo | Evidência | Consequência |
| --- | --- | --- |
| `Angry_Birds_Decompilation-master.zip` | 13 entradas; README, `birdout.py`, `openssl.exe`, `unluac.jar`, exemplo `HANSOLO.json` | Ferramentas auxiliares, sem projeto C/C++ ou build PS2 |
| Submódulos `ABStudio` e `SpriteDATGenerator` | Declarados em `.gitmodules`, mas diretórios vazios no ZIP | Conteúdo dos submódulos não foi incluído |
| `com.rovio.angrybirds.apk` | 103.772.201 bytes; 4.389 arquivos; CRCs válidos | APK íntegro para esta análise |
| Motor Android | `lib/armeabi-v7a/libAngryBirdsClassic.so`: ELF ARM, máquina 40; `lib/x86/...`: ELF x86, máquina 3 | Nenhuma implementação nativa para MIPS R5900 do PS2 |
| Dados de fases | 1.082 arquivos sob `assets/data/levels`, incluindo 63 em `1_PoachedEggs` | Fases podem servir de dados de entrada para o novo motor |
| Lua | 2.058 arquivos `.lua`; todos recuperados em bytecode Lua 5.1 | Há lógica reaproveitável, dependente de APIs nativas a implementar |
| Áudio | 490 arquivos OGG | Necessita integração de reprodução/streaming ou conversão para formatos adequados ao PS2 |
| Gráficos | 1.015 PNG no APK inteiro, incluindo recursos Android; 172 PNG válidos sob `assets/data`; 57 PVR no APK | É necessário selecionar os recursos do jogo e preparar texturas para o GS |

SHA-256 do APK:
`8cdea1ef39a2a5c3bad5ac27050b491dbdf68050754241390cf9f0d3d24979b8`

## Recuperação dos dados

O caminho confirmado para 2.057 scripts é AES-256-CBC com a chave Classic do README,
seguido do cabeçalho Rovio `89 4c 5a 4d 41 0d 0a 1a 0a`, um fluxo LZMA Alone e
bytecode Lua. Isso difere do caminho que pressupõe sempre um arquivo 7z no
`birdout.py` fornecido.

Foi possível decompilar `assets/data/levels/1_PoachedEggs/Level1.lua` com o
`unluac.jar` do ZIP. A saída inclui `RedBird`, `SmallPiglette`, `WoodBlock6`,
posições, ângulos, densidade, resistência e dados de câmera. Isso comprova a
recuperação dos dados de uma fase, mas não a execução dela.

Exceção identificada e resolvida: `assets/data/scripts/options.lua` é AES seguido
de um arquivo 7z convencional. O extrator agora aceita 7-Zip e recupera esse chunk
também, totalizando 2.058 arquivos Lua sem falhas.

Todos os 2.058 chunks recuperados têm cabeçalho `1b4c75615100010404040400`:
Lua 5.1, little-endian, int/size_t/instrução de 4 bytes e `lua_Number` de 4 bytes.
Um Lua compilado com números de 8 bytes não deve carregar esses chunks diretamente.
É preciso compatibilizar o runtime ou decompilar e recompilar os scripts para ele.

## Componentes que faltam

| Prioridade | Componente | Trabalho necessário / critério de conclusão |
| --- | --- | --- |
| 1 | Build PS2 | Configurar PS2SDK/toolchain, Makefile e dependências; gerar um ELF mínimo que inicia |
| 1 | Vídeo e controle | Inicializar GS, desenhar sprites, ler DualShock 2 e tratar desconexão; mostrar tela com controle funcionando |
| 1 | Carregamento | Definir caminhos e acesso aos dados por USB e/ou disco; carregar uma fase sem depender do Android |
| 1 | Recursos de uma fase | Decodificar metadados de atlas, extrair recortes, converter texturas, preservar transparência/pivôs; desenhar o cenário da primeira fase |
| 1 | Física | Integrar uma biblioteca adequada, como Box2D de versão avaliada para PS2, e implementar corpos, colisões, dano e destruição; lançar um pássaro e atingir um porco |
| 1 | Regras | Estilingue, fila de pássaros, câmera, vitória/derrota, reinício e pontuação para uma fase |
| 2 | Lua e APIs do motor | Escolher entre converter dados e implementar regras em C/C++, ou integrar Lua 5.1 com as APIs usadas pelos scripts; listar e implementar dependências reais |
| 2 | Memória e desempenho | Usar carregamento por fase e orçamento de RAM/VRAM; medir estabilidade e tempo de quadro |
| 2 | Áudio | Implementar efeitos e música com buffers/streaming adequados; medir uso de CPU e memória |
| 2 | Interface e progresso | Menu, seleção de fases, pausa e salvamento no Memory Card |
| 3 | Conteúdo adicional | Habilidades dos pássaros, materiais, temas, partículas e demais episódios |
| 3 | ELF/ISO de entrega | Build reproduzível, nome de boot coerente em `SYSTEM.CNF`, imagem de disco e validação no carregador escolhido |
| 3 | Validação | Testar no PCSX2 e depois no PS2 físico, incluindo carregamento, controle, áudio e salvamento |

O orçamento importa desde o início: o PS2 tem 32 MiB de RAM principal e 4 MiB de
VRAM. O conteúdo total descompactado do APK soma 163.234.921 bytes, incluindo
recursos Android e bibliotecas de duas arquiteturas. Isso não representa o tamanho
final do port. Um único PNG encontrado, `THEME_23_1.png`, mede 2000×1991: em RGBA32
ocuparia 15.928.000 bytes, acima de toda a VRAM. Seleção, recorte, formatos
indexados/reduzidos e carregamento por fase precisam ser planejados.

Recursos de anúncios, compras e serviços Android aparecem no APK. O projeto
precisa definir os pontos de entrada locais e as dependências que serão removidas
ou substituídas para funcionar offline.

## Próximo marco recomendado

Criar um ELF de uma única fase de Poached Eggs: carregar os objetos recuperados,
mostrar o cenário, mirar pelo controle, lançar RedBird, aplicar física/dano e
permitir vitória/derrota e reinício. Esse marco testa os principais riscos do port
antes de expandir para menus, habilidades e outros episódios.

## Entregue nesta etapa e limites da verificação

- Inventário completo e resumo em JSON, com hash e cabeçalhos dos chunks Lua.
- Código `tools/inspect_apk.py` e instruções para repetir a recuperação localmente.
- Recuperação local de 2.058 chunks e decompilação de uma fase de exemplo.
- Nenhuma alteração nos dois arquivos originais do Drive.

Após a análise inicial, as ferramentas da pasta Ferramentas foram disponibilizadas
localmente e usadas para gerar ELF e ISO do protótipo 0.1.0, com teste no PCSX2.
Ainda não foi realizado teste em console, verificada a fidelidade de todas as
fases ou recuperado o código C/C++ do motor Android. O novo código PS2 é uma base
técnica e ainda não executa os dados originais.
