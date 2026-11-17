# PS2Birds Rio — modificações desta etapa (0.3.0, investigação)

Nenhum ELF, ISO ou kit novo foi gerado: o gerador do Rio continua recusando ISO até o runtime ser integrado, e o runtime do Classic (0.93.0) não mudou. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `tools/rio/pvr.py`: leitor de texturas PVR v3 em Python puro (RGBA4444, RGB565 e alpha8). Rodando `python3 tools/rio/pvr.py APK` ele decodifica 118 das 139 folhas PVR do APK.
- Ordem dos canais confirmada olhando a folha `INGAME_RIO1_BLOCKS_MISC_1`: vermelho nos 4 bits mais altos, depois verde, azul e, nos 4 bits mais baixos, a transparência.

## Editado
- `docs/rio/ORDEM_DAS_FASES.md`: tabela de texturas da Level190 (já da etapa anterior) e a lista do que falta.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.2.0.md`, substituído por este registro (e o equivalente no Drive).

## Resultado
- As 21 folhas restantes são ETC1 (comprimidas); nenhuma delas é necessária para a Level190.
- Só a caixa de metal da Level190 continua dependendo de WebP.

## O que falta
1. Recortar os sprites usando os retângulos dos arquivos `.dat` e montar o `SPRITES.BIN` do Rio.
2. Decidir sobre o WebP (caixa de metal).
3. Montar as composições das gaiolas.
4. Origem do lançamento e gravidade.
5. Objetivo de vitória por gaiolas no runtime e save `/PS2BRIO`.
