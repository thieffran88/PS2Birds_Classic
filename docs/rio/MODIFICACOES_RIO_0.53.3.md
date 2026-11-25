# Modificações — Rio 0.53.3 (camadas de tema em PNG)

## O que era
Os cenários dos temas do Rio eram montados só com camadas em PVR e WebP; as camadas em PNG (pasta `themes_android`) eram ignoradas ("PNG sheets: not used by the themes"). Faltavam planos inteiros, como o fundo do Market Mayhem (o chão aparecia liso) e a água e o coral do Hidden Harbor.

## Editado
- `tools/rio/extract_themes_rio.py`: `read_sheet` lê PNG (`png_decode`) e `layer` deixa de pular as folhas PNG.
- Resultado: de 35 camadas ilegíveis nos temas passam a sobrar 8: `FOREST_SKY_CLOUDS`, `FOREST_WALL_COMPO` e as cachoeiras (`AMAZON_WATERFALL_*`, uma animação por tabela) nos temas 22 e 23 do Timber Tumble e do Treasure Hunt.
- Versão do Rio 0.53.3 (só dados e a versão: o Classic continua na 1.4.2).

## Conferido
- Emulador: Market Mayhem 1 (rua de mercado com folhagem e prédios ao fundo) e Hidden Harbor (água e coral); o resto do cenário continua como antes.
