# Modificações completas — PS2Birds Classic 0.71.0

## Gráficos originais na tela de fim de fase
- Novo `tools/extract_ui.py`: tira do APK os 8 sprites que o `level_end.lua` usa e grava o novo arquivo `UI.BIN`:
  - `BIG_STAR_1/2/3` (estrelas grandes);
  - `NEW_HIGHSCORE_BADGE` (selo de novo recorde);
  - `BUTTON_MENU`, `BUTTON_RESTART` e `BUTTON_NEXTLEVEL` (botões);
  - `LEVEL_FAILED_PIG` (porco da derrota).
- **Formato:**
  - os originais foram desenhados para 1024×768 e são reduzidos a 40% com média de área ponderada pelo alfa;
  - ficam numa única página de 256×256 em 16 bits (RGBA5551), com 128 KB.
- **Leitura das folhas do APK:** três vêm de PNG e a dos botões (`BUTTONS_SHEET_1_1`) vem de PVR v3 em RGBA4444 com alfa pré-multiplicado. Há um leitor novo para esse formato.
- **Memória de vídeo:** a página fica nos cerca de 160 KB livres além das páginas de textura das fases. É enviada uma vez só e vale para todas as fases.
- Novo módulo `ps2/src/ui.c` / `ui.h`. Para os texels transparentes sumirem:
  - o registro TEXA (com AEM) torna transparente o preto sem alfa;
  - o teste de alfa do GS descarta esses texels, porque sem ele o pontilhado do buffer de 16 bits deixava uma moldura em volta de cada sprite.
- **Vitória:**
  - três estrelas grandes; as conquistadas aparecem acesas no ritmo da contagem e as outras escurecidas, como as vazias do original;
  - a do meio fica maior e mais alta, como no original;
  - a pontuação contando;
  - o selo de novo recorde;
  - os botões Menu (O), Reiniciar (TRIÂNGULO) e Próxima (X).
- **Derrota:** o porco da derrota e os botões Menu (O) e Reiniciar (TRIÂNGULO).
- Sem `UI.BIN`, as telas voltam ao texto da 0.70.0.
- Novo teste `ps2/tests/ui_assets_test.py`: cabeçalho, tamanho da página, posições sem sobreposição, área opaca de cada sprite e texels transparentes pretos.
- **Conferido no PCSX2** (captura em `docs/pcsx2_0.71.0_fim_de_fase.png`):
  - vitória na 1-1 (2 estrelas) e na 1-2 (1 estrela);
  - derrota na 1-1.

## Gerador e documentação
- `gerar_iso.py` grava `UI.BIN` na ISO e na pasta USB/HDD. Agora são seis arquivos; o README e o `LEIA-ME` do kit foram atualizados.
- O kit para Windows (`make_windows_kit.py`) passa a incluir o `extract_ui.py`. Sem ele, o `GERAR_ISO.bat` falhava; o problema foi visto na conferência sob Wine e corrigido antes da entrega.

## Limpeza
- Removidos os documentos da 0.70.0 e a captura da 0.68.0 (substituídos pelos da 0.71.0).
- Removidas da área temporária as pastas da 0.70.0, as prévias da página de interface, as capturas de teste e o log de testes.

## Versão
- Tela inicial e README: 0.71.0.
