# Modificações — 0.93.1

Versão do **gerador de ISO** (acompanhamento e velocidade). O jogo em si não mudou em relação à 0.93.0.

## Adicionado
- **Painel de acompanhamento** do gerador (opção `--painel`, já ligada no `GERAR_ISO.bat`): uma página no navegador, só neste computador, com a barra de etapas, o tempo total, o tempo da etapa atual, o "último sinal de vida" e o registro detalhado, atualizada a cada segundo. Fica amarelo se passar de 90 s sem sinais e vermelho em caso de erro.
- **Sinal de vida** no console e no registro: a cada 10 s, "ainda trabalhando: X nesta etapa, Y no total".
- **Arquivo de registro** `GERAR_ISO_LOG.txt`, com a hora de cada linha.
- `tools/progresso.py` (etapas, relógio, painel e registro; só biblioteca padrão).

## Editado
- `tools/gerar_iso.py`: nove etapas numeradas, cada uma com o próprio "ok" e o tempo (antes, interface, letras, menu, fundos e quadrinhos ficavam escondidos sob "3/3 Sons", e o "ok" só aparecia no fim de todos eles); opções `--painel` e `--log`.
- `tools/extract_ui.py`: o redimensionamento de imagens calcula os pesos de cada linha e coluna uma única vez (resultado idêntico byte a byte, cerca de 2 vezes mais rápido).
- `tools/extract_themes.py` e `tools/extract_cutscene.py`: os temas e as cenas são feitos em paralelo (um processo por núcleo, no máximo 6), com uma linha de registro por tema e por cena; se o paralelo não estiver disponível, segue em um processo.
- `tools/make_windows_kit.py` e `GERAR_ISO.bat`: incluem `progresso.py` e chamam o gerador com `--painel`.
- Versão 0.93.1 (README e tela do jogo).

## Excluído
- `docs/MODIFICACOES_0.93.0.md` e `docs/VALIDACAO_0.93.0.md` (substituídos pelos desta versão), ISO, ELF, kit e código-fonte da 0.93.0 (substituídos).

## Medido (4 núcleos, mesmo computador)
- Fundos: 388 s para 94 s; arquivo idêntico ao anterior.
- Quadrinhos: 290 s para 91 s; arquivo idêntico ao anterior.
- Interface, letras e menu: ligeiramente mais rápidos com o novo redimensionamento (resultado idêntico).
- Os tempos absolutos em outro computador variam com o número de núcleos.
