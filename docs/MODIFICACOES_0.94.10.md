# Modificações — 0.94.10 (Classic) e 0.49.0 (Rio)

## Adicionado
- **Tela de seleção de power-ups antes da fase:** Dobro de pontos, Pássaro extra (vermelho, amarelo ou azul), Chapéu teleguiado (R2 em voo). Sem moedas: tudo grátis e ilimitado. Teste: `ps2/tests/shop_test.cpp`.
- `docs/MODIFICACOES_0.94.10.md` e `docs/rio/MODIFICACOES_RIO_0.49.0.md`.

## Editado
- **Início mais curto:** memory card, efeitos e música carregam depois do primeiro quadro (logo da Rovio).
- `tools/rio/inspect_dat.py` lê o cabeçalho das imagens sem Pillow (corrige o kit do Windows).
- `README.md` e `docs/` sem as referências ao Android; `tools/paralelo.py` e `tools/progresso.py` sem o tratamento do Termux.
- `docs/LACUNAS_PORT.md`: power-ups da loja.

## Excluído
- **Gerador para Android:** `docs/GERADOR_ANDROID.md`, `tools/GERAR_ISO_ANDROID.sh`, `tools/LEIA-ME_ANDROID.txt`, `tools/make_android_kit.py` (a pedido).
- Notas da versão anterior (`MODIFICACOES_0.94.9.md`, `VALIDACAO_0.94.9.md`, `MODIFICACOES_RIO_0.48.0.md`) e os arquivos da release 0.94.9.

## Não validado
- Nada disto foi visto no console. O Homing Hat é aproximado; Boom Box, Chrono Bird e Red's Mighty Feathers ainda faltam.
