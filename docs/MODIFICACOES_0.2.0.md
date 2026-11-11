# Modificações completas — 0.2.0 em relação à 0.1.1

- Substituído o cenário de alvo único pelos dados recuperados de Poached Eggs / Level1.lua.
- Importados 19 objetos de cenário: 1 porco, 14 blocos e 4 plataformas estáticas.
- Importada a fila de três RedBirds, ordenada por startNumber.
- Preservadas posições, rotações, densidade, resistência e defesa dos objetos.
- Recuperados gravidade 20 e ponto do estilingue a partir dos dados da fase.
- Consultados os atlas do APK para dimensionar madeira, vidro e plataformas.
- Incluídas métricas dos sprites e identificação das folhas de origem em JSON.
- Usadas caixas de tamanho sprite/20 como aproximação das colisões; raio do porco 1,075 recuperado de blocks.lua.
- Adicionado importador sem execução de Lua, com rejeição de definições desconhecidas.
- Incluídos Lua de origem, hash, JSON da fase e cabeçalho C gerado para reprodução da importação.
- Migrado o núcleo da simulação de C para C++11, preservando interface C para vídeo/controle.
- Integrado Box2D 2.4.1, com código-fonte e licença MIT no pacote.
- Criados corpos dinâmicos para blocos e porco e corpos estáticos para plataformas/chão.
- Adicionados gravidade, rotação, atrito, restituição, repouso e colisões entre corpos.
- Ativada detecção contínua de colisão no pássaro lançado para reduzir atravessamento de blocos.
- Substituída a remoção simples por sobreposição por dano baseado no impulso de colisão, massa e defesa.
- Implementada remoção de corpos destruídos fora dos callbacks do solver.
- Adicionado assentamento da estrutura durante a mira; dano inicial suprimido até o lançamento e fim do período de proteção.
- Introduzidos passo fixo de 120 Hz e acumulador de tempo para a física.
- Atualizada a previsão de trajetória para o integrador discreto usado pelo Box2D em voo livre.
- Adicionados limites de voo, detecção de repouso e espera antes de encerrar a tentativa.
- Mantidos corpos da estrutura entre tentativas; reinício recria a fase completa.
- Adicionada liberação explícita do mundo físico no reinício e encerramento.
- Atualizada pontuação: 500 por bloco destruído, 5.000 por porco e 10.000 por pássaro não usado na vitória.
- Adicionada transformação de coordenadas da fase para a tela PS2.
- Implementado desenho de blocos rotacionados por faixas horizontais, com recorte na área de jogo.
- Diferenciados visualmente madeira, vidro e plataformas e adicionada variação de cor por dano.
- Ajustados o tamanho desenhado do pássaro e do porco à escala da cena.
- Reposicionado o estilingue conforme os dados importados.
- Adicionados contadores de porcos/blocos, título Poached Eggs 1-1 e mensagem de fase concluída.
- Corrigida pausa indevida durante a negociação temporária do modo DualShock; pausa por desconexão/erro real mantida.
- Mantidos analógico, zona morta, direcional, ajuste fino, pausa e reinício da versão anterior.
- Atualizado o Makefile para compilar/linkar C++ e Box2D, com dependências dos dados gerados.
- Separada a compilação dos testes próprios, com avisos tratados como erro, da biblioteca de terceiros.
- Adicionados testes de importação, assentamento, trajetória, destruição, vitória, derrota, reinício e liberação do mundo.
- Verificadas 567 combinações de tiro: 175 venceram e 215 destruíram pelo menos um bloco na simulação nativa.
- Verificados boot do ELF e da ISO no PCSX2 e vitória com 28.000 pontos por controle emulado.
- Atualizadas identificação para 0.2.0, instruções, dependências, documentação e capturas.
- Gerados novos ELF, ISO, código-fonte e hashes SHA-256; substituição dos três artefatos 0.1.1 após verificação dos uploads.

Ainda faltam sprites originais, áudio, outras fases, dano/formas de colisão fiéis,
menus, salvamento e testes em PS2 físico. Esta versão executa dados convertidos
da fase com física nova; não executa o motor ou a lógica Lua completa do Android.
