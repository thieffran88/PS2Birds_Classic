# PS2Birds Rio — modificações desta etapa (0.2.0, investigação)

Nenhum ELF, ISO ou kit novo foi gerado nesta etapa: o gerador do Rio continua recusando ISO enquanto o runtime não estiver integrado, e o runtime do Classic (0.93.0) não mudou. Por isso não havia arquivos anteriores de ISO/ELF para excluir.

## Adicionado
- Base unificada no repositório, sobre a 0.93.0: `tools/rio/` (`investigate_rio`, `inspect_levels`, `inspect_dat`, `inspect_comp`, `build_catalogue`, `audit_gaps`), `profiles/projects.json` e `docs/rio/`.
- `docs/UNIFICACAO_PS2BIRDS.md`: registro da reorganização.
- `tools/rio/level_order.py`: recupera a ordem das fases de cada pacote a partir das constantes de `gamelogic.lua` e confere contra as pastas.
- `docs/rio/ORDEM_DAS_FASES.md`: ordem do episódio 1 (Smugglers' Den), primeira fase-alvo (`warehouse/Level190`) e as texturas que ela exige.
- `docs/rio/MODIFICACOES_RIO_0.2.0.md`: este registro.

## Editado
- `tools/gerar_iso.py`: opções `--jogo auto/classic/rio` e `--investigar`, detecção do jogo pelo SHA-256, recusa de ISO Rio.
- `tools/requirements-assets.txt`: comentário cobre PNG/WebP.
- `.gitignore`: `input/`, `work/`, `dist/`, `*.apk`, `*.iso`, `*.elf`; mantido `docs/*.png`.

## Excluído
- Nada foi excluído do repositório. Conferi a lista de arquivos versionados e não há arquivos sem relevância (sem capturas, APKs, ISOs, ELFs, zips ou temporários). Os dados gerados ficam em `work/`, fora do Git.

## Descobertas
- Ordem das fases: 43 de 44 pacotes conferem; exceção `plane2` sem `Level201` (fase especial).
- `Level190`: 4 pássaros vermelhos, 30 objetos, 5 gaiolas e nenhum porco (vitória por gaiolas).
- Texturas: as folhas `_sd` do Rio são PVR RGBA4444 (sem compressão, legíveis em Python puro); a caixa de metal só existe em WebP.
- `B_CAGE_*` são composições, não sprites de folha.

## O que falta para a primeira fase jogável
1. Decidir o decodificador de WebP (só a caixa de metal precisa dele na primeira fase).
2. Ler PVR RGBA4444 e montar o `SPRITES.BIN` do Rio.
3. Montar as composições das gaiolas.
4. Origem do lançamento e gravidade (não estão nos arquivos de fase).
5. Generalizar o objetivo de vitória no runtime e criar o save `/PS2BRIO`.
