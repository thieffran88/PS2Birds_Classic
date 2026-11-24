# Modificações — 0.94.20 (Classic) e 0.49.10 (Rio)

## Investigado
- **"547 definições sem adaptador de importação":** o número era de uma versão muito anterior. `tools/audit_definitions.py` lê as 1.082 fases do APK e confere cada definição com `profiles/classic_level.json`: as 745 fases do port usam 604 definições e **todas** têm adaptador (nenhuma falta). Das 748 definições do `blocks.lua`, 134 não têm adaptador; 103 delas nenhuma fase usa, e as demais só aparecem em fases fora do port (Mighty League, siege, eagleDemo: 46 definições próprias).

## Adicionado
- `tools/audit_definitions.py` (relatório da conferência; sai com erro se uma fase importada precisar de uma definição sem adaptador), `docs/MODIFICACOES_0.94.20.md` e `docs/rio/MODIFICACOES_RIO_0.49.10.md`.

## Editado
- `docs/LACUNAS_PORT.md` (seção "O que falta na decompilação/importação"). O jogo não mudou.

## Excluído
- Notas da versão anterior.

## Fora do port
- Mighty League (273 fases), siege (9) e eagleDemo (3): 337 fases de modos que o port não tem.
