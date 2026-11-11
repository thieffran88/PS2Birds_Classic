# Revisão da versão 0.9.0 fornecida pelo usuário

- A base desta entrega é o ZIP 0.9.0 do Drive, não a antiga árvore 0.8.0. O ZIP original foi guardado localmente para comparação.
- A ordem das 21 fases foi conferida recuperando novamente `assets/data/scripts/episodes.lua` do mesmo APK e decompilando com o unluac fornecido, sem executar Lua. A lista coincide com `profiles/classic_episode.json`; o trecho de procedência foi incluído em `ps2/data/episode_page1.decompiled.lua`.
- A correção de numeração da 0.9.0 é necessária: LevelN não equivale à posição N no episódio. As posições e a migração corrigidas foram preservadas.
- As 21 fases passaram nos testes nativos, com os mesmos 21 resultados publicados na 0.9.0. Também passaram os testes do azul, amarelo, colisão inicial em rampa, contagens, desbloqueios, recordes, tiros que erram o cenário e transições de mundo.
- Mantidos os 109 sprites, as três páginas e o framebuffer de 16 bits com dithering. A inicialização e alocação das páginas foram verificadas no PCSX2.
- Os layouts v1/v2 e o remapeamento de saves antigos foram mantidos. A gravação v2 foi conferida no cartão de teste do emulador.
- Foi encontrado um caso incompleto na proteção contra saves futuros: se um slot contém formato desconhecido e o outro um save antigo válido, a seleção ignorava o desconhecido e a próxima gravação poderia sobrescrevê-lo. A 0.10.0 inspeciona os dois cabeçalhos antes de selecionar/gravar, preserva o arquivo futuro e informa a incompatibilidade.
- Cabeçalhos v1 com mais de oito fases também são preservados: as versões publicadas em v1 só tinham até oito, e não existe mapeamento confiável para supostas posições extras.
- Corrigida também a chamada de tools/py7z.py: após extrair o ZIP, o script pode não ter permissão de execução. Os extratores agora o invocam pelo Python corrente, inclusive com espaços no caminho; a dependência py7zr continua necessária para esse backend.
- Não foi removida nenhuma funcionalidade da 0.9.0. O avanço principal da 0.10.0 é o sistema de efeitos sonoros originais.

A revisão não equivale a validar todos os detalhes do motor original. A pontuação, os contatos de cenário e os fatores das habilidades continuam aproximados. Console físico ainda não foi testado.

ZIP original 0.9.0 analisado: SHA-256 `4e28fdffd3ad34492633f1eff920b3611317a776b7a127c47707c350525e85e3`.
