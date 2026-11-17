# PS2Birds Rio — modificações desta etapa (0.23.0, investigação)

Nenhum ELF, ISO ou kit do Classic foi publicado nesta etapa; o Classic 0.93.0 publicado não mudou. O gerador do Rio continua recusando ISO. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- Decodificador ETC1 em `tools/rio/pvr.py` (Python puro).
- `docs/rio/MODIFICACOES_RIO_0.23.0.md`.

## Editado
- `tools/rio/pvr.py`: `read_pvr` agora lê ETC1 (formato 6).
- `docs/rio/ORDEM_DAS_FASES.md`: seção do fundo do tema `theme2` e do decodificador ETC1.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.22.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- As 139 folhas PVR do APK do Rio são decodificadas (antes, 118; as 21 restantes eram ETC1).
- Ordem dos blocos ETC1 confirmada numericamente (continuidade 9,05 contra 7,49 na leitura em fileiras; 24,81 em ordem de Morton).
- O fundo da primeira fase (`theme2`) usa folhas PVR, uma delas ETC1; agora todas são legíveis.

## O que falta
1. `THEMES.BIN` do Rio (camadas, rolagem e cores do `theme2`) e o `background.c` do Rio.
2. Decodificador de WebP (caixas de metal e outros).
3. Som, música e interface do Rio no gerador; rodar no PCSX2.
4. Macacos, Luiz e chefes.
