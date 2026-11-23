# Modificações — 0.94.11 (Classic) e 0.49.1 (Rio)

## Editado
- **Explosão com a fórmula exata** (`ps2/src/game.cpp`, `game.h`): o impulso é `força × 1/20` (era `força/120`), caindo linearmente até 0 no raio, como no `Explosions.lua`; o dano agora desconta a defesa da peça. Afeta TNT, pássaro preto, baiacu e toda peça explosiva.
- **Roteiros de vitória** (`ps2/tests/campaign_test.cpp`): refeitos para as fases afetadas. 153 fases ganharam roteiro novo; 3-20, 5-16 e 13-7, que nunca tinham vencido, agora vencem. Fases sem roteiro: de 78 para 62.
- `docs/LACUNAS_PORT.md` (seção "Fórmulas exatas (0.94.11)") e `README.md`.

## Adicionado
- `docs/MODIFICACOES_0.94.11.md` e `docs/rio/MODIFICACOES_RIO_0.49.1.md`.

## Excluído
- Notas da versão anterior (`MODIFICACOES_0.94.10.md`, `MODIFICACOES_RIO_0.49.0.md`) e os arquivos da release 0.94.10.

## Não validado
- Nada foi visto no console. O dano de colisão entre peças e com o chão continua aproximado (fórmula exata documentada, não aplicada). As 62 fases sem roteiro são fases em que a busca simples não achou vitória, não prova de que sejam impossíveis.
