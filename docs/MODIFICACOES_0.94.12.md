# Modificações — 0.94.12 (Classic) e 0.49.2 (Rio)

## Editado
- **Pontuação por peça** (`getScoreToAddForBreaking`): cada peça agora vale o `destroyedScoreInc` da própria definição (`blocks.lua`), não mais 500 fixos. 7.926 das 114.580 peças das fases têm valor próprio (gemas e frutas 3000, flores, cordas 100, rodas 50, estática frágil 0...); as outras continuam em 500, e os porcos em 5000. O valor da instância na fase tem prioridade sobre o da definição, como no original.
- `CAMPAIGN.BIN` passa para a versão 10 (um campo `score` por peça): `tools/build_campaign.py`, `ps2/src/game.cpp`, `campaign_types.h`; os arquivos de dados precisam ser gerados de novo.
- `tools/import_level.py` e `profiles/classic_level.json` (239 definições com `score`).
- Teste: `ps2/tests/profile_test.py` confere os valores; a fase Golden Eggs 2-10 pontua 50 ao assentar (uma peça frágil estática estoura e agora vale o seu `destroyedScoreInc`).

## Adicionado
- `tools/add_scores.py` (lê o `blocks.lua` decompilado e grava o `score` no perfil), `docs/MODIFICACOES_0.94.12.md` e `docs/rio/MODIFICACOES_RIO_0.49.2.md`.

## Excluído
- Notas da versão anterior (`MODIFICACOES_0.94.11.md`, `MODIFICACOES_RIO_0.49.1.md`).

## Ainda aproximado
- **Pontos de dano por colisão:** no original, cada batida do pássaro ou de uma peça que causa dano soma `floor(dano) × 10` (`CollisionCallbacks.lua`) e aparece como número flutuante. O port não soma isso, porque depende da fórmula exata de dano de colisão (pendência 33b). As explosões não dão pontos de dano (`addExplosionScore` é vazia no original).
- O bônus de 10.000 por pássaro que sobra e o 5.000 por porco já eram os do original.
