# Validação 0.4.0 — 11/11/2026

- Chaves dos seis perfis comparadas com o README fornecido e com a configuração derivada de birdout.py.
- Testes sintéticos AES/LZMA verificam seleção das seis chaves, entradas Lua diretas, rejeição de perfil/chave incorretos e validação da detecção de JSON.
- Recuperação individual testada por CLI com conteúdo sintético, sem alterar a entrada; tentativa de entrada/saída no mesmo caminho rejeitada.
- Adaptadores testados com gravidade, estilingue e enquadramento alternativos, duas aves e alias de bloco; esses dados são fixtures, não outro jogo real.
- Definição desconhecida rejeitada antes de substituir cabeçalho válido.
- Recuperação real Classic: 2.058 chunks recuperados, zero falhas.
- Diagnóstico Classic: 12 amostras recuperadas, cabeçalho Lua 5.1; compatibilidade de runtime deliberadamente não inferida pelo scanner.
- Testes gráficos e de Box2D preservados: 23 sprites, 175 combinações vencedoras e 215 com destruição entre 567 tiros.
- Compilação PS2 com GCC/G++ 15.2.0 e ferramentas fornecidas; avisos já conhecidos em cabeçalhos de terceiros.
- Boot da ISO final confirmado no PCSX2; uma partida terminou com 28.000 pontos.
- Nenhum APK/pacote de Rio, Seasons, Space ou Star Wars foi validado. Não houve teste em PS2 físico.

O engine ainda não executa Lua do jogo diretamente: usa dados convertidos, física
aproximada, sprites e regras do subconjunto Classic implementado. Os outros jogos
exigem análise de versões, APIs nativas, recursos e comportamentos próprios.
