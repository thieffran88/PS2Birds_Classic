# Reutilização da decompilação e do port — 11/11/2026

## Conclusão

A ferramenta fornecida declara chaves para Classic, Rio, Seasons, Space, Star Wars
e Star Wars II, tanto no README quanto em birdout.py. Isso é suporte declarado
para recuperação de scripts de versões compatíveis, não decompilação completa do
motor nem garantia para todas as versões/plataformas desses jogos.

Só há confirmação prática com o APK Classic fornecido: 2.058 chunks recuperados,
uma fase convertida e executada no PS2 emulado. Os demais jogos não foram testados
com arquivos reais. Testes sintéticos de AES/LZMA verificam nossa seleção de
chaves, mas NÃO comprovam compatibilidade de um APK Rio/Seasons/Space/Star Wars.
A referência original, preservada em profiles/REFERENCE_DECOMPILATION_README.md,
é a fonte das declarações. O projeto se descreve como ferramenta para arquivos
Lua de ports PC; nossa adaptação também foi verificada no APK Classic disponível.

| Jogo | Scripts na ferramenta fornecida | Reuso do port / trabalho adicional |
| --- | --- | --- |
| Classic | Declarado e confirmado no APK fornecido | Base atual; só o subconjunto da primeira fase/RedBird está integrado |
| Seasons | Declarado; nenhum pacote real testado | Candidato próximo: fases, temas, pássaros e objetos próprios precisam de adaptação |
| Rio | Declarado; nenhum pacote real testado | Base de lançamento reaproveitável; gaiolas, resgate, inimigos e objetivos precisam de código |
| Space | Declarado; nenhum pacote real testado | Vídeo/controle/ferramentas reaproveitáveis; gravidade planetária e mecânicas espaciais não implementadas |
| Star Wars | Declarado; nenhum pacote real testado | Infraestrutura reaproveitável; poderes de personagens e mecânicas de gravidade precisam de código |
| Star Wars II | Declarado; nenhum pacote real testado | Infraestrutura reaproveitável; seleção/troca e poderes dos personagens precisam de código |
| Outros títulos da série | Não contemplados por essa referência | Analisar formato, motor e mecânicas antes de estimar reaproveitamento |

Classic e Rio usam a mesma chave na referência. Isso não torna seus motores ou
arquivos automaticamente intercambiáveis. Recuperar Lua não fornece as APIs
nativas C/C++ chamadas pelos scripts, nem reconstrói renderização/áudio/física.
Não há evidência suficiente para prometer que os seis jogos já possam rodar no PS2.

## O que já pode ser compartilhado

- Build PS2SDK/gsKit, ELF/ISO e carregamento dos recursos incorporados.
- Vídeo, transparência, atlas, sprites rotacionados e orçamento de VRAM.
- DualShock, analógico, zona morta, precisão, pausa e desconexão.
- Integração Box2D e ciclo de lançamento, colisões, reinício e pontuação básica.
- Decodificação AES/LZMA/7z e ferramentas de inventário/diagnóstico.
- Formato intermediário de fase e testes de regressão.

## Facilitações implementadas em 0.4.0

- profiles/games.json separa chaves, títulos, origem da declaração e estado dos testes.
- --game seleciona o perfil no inventário, recuperação de scripts e extração gráfica.
- check_game.py diagnostica amostras de um APK sem executar scripts e sem anunciar compatibilidade de runtime.
- recover_lua_file.py aceita um arquivo individual de uma versão compatível, inclusive arquivos de ports PC.
- profiles/classic_level.json mapeia nomes de objetos/pássaros para o subconjunto suportado.
- O importador aceita --profile para um adaptador explicitamente verificado; nomes desconhecidos são rejeitados.
- Gravidade uniforme, origem do estilingue, chão, escala e quantidade de pássaros vêm dos dados gerados em level_config.h.
- Chaves e contêineres são tratados num módulo compartilhado, evitando implementações divergentes.

Ainda há um adaptador de sprites específico do Classic, IDs gráficos específicos
no renderizador e regras/formatos de Lua limitados. O parser de fase entende o
layout de atribuições produzido pelo unluac desta amostra; não é um interpretador
Lua genérico. Apenas registrar uma chave ou renomear objetos não adiciona poderes,
objetivos de resgate ou gravidade planetária. O adaptador atual só aceita gravidade
uniforme para baixo, caixas/plataformas, RedBird e o porco suportado.

## Como avaliar o próximo jogo

1. Usar uma cópia do jogo alvo e registrar plataforma, versão e hash.
2. Executar check_game.py com --game correspondente; examinar o resultado e as falhas.
3. Recuperar scripts e conferir versão/cabeçalho Lua e a relação com APIs nativas.
4. Criar adaptadores de fases, materiais e sprites reais, sem inferir compatibilidade apenas pela chave.
5. Implementar habilidades/objetivos específicos e validar uma fase pequena.
6. Medir memória/desempenho no PS2 físico antes de expandir conteúdo.

Recomendação: concluir a base do Classic e usar uma fase simples de Seasons como
primeiro candidato de reaproveitamento. Rio é outra possibilidade próxima, mas o
objetivo de resgate exige adaptação. Space deve vir depois do suporte a novos
campos de gravidade. Essa ordem é uma avaliação de engenharia, não um teste desses
jogos. Nenhum desses pacotes adicionais foi validado nesta entrega.
