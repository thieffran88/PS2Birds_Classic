# Modificações — 0.94.21 (Classic) e 0.49.11 (Rio)

## Investigado
- **Roteiros de pan (`castleCameraData` e `birdCameraData`):** cada uma das 745 fases traz as duas janelas de câmera (px, py, sx, sy para uma tela 1024 × 768). A abertura do original (`DefaultLevelStartCameraSequence`) mostra o "castelo" por 2 s e desliza até o "pássaro" (estilingue) em 1 s; depois `FollowShotBirdCameraSequence` segue o pássaro. O zoom das duas janelas é o mesmo (mediana 1,00; 0,96 a 1,20); o deslocamento entre elas vai de 0 a 1,35 telas (mediana 0,63; só 15 fases sem deslocamento).
- **Por que o port não tem pan:** a fase inteira é desenhada numa tela de 640 × 448, com cerca de 0,59 do zoom original (mediana; de 0,18 a 1,33). Não há o que deslizar.
- **Decisão (sua):** manter a vista fixa. As alternativas eram o zoom original com rolagem (mecanismo do Rio) ou só um destaque no castelo na abertura.

## Adicionado
- `tools/audit_camera.py` (mede a câmera original contra a vista do port), `docs/MODIFICACOES_0.94.21.md` e `docs/rio/MODIFICACOES_RIO_0.49.11.md`.

## Editado
- `docs/LACUNAS_PORT.md`. O jogo não mudou.

## Excluído
- Notas da versão anterior.
