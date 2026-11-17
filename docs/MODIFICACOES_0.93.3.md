# Modificações — 0.93.3

Versão do **gerador**: agora também para **Android** (Termux), para quem não tem computador. O jogo em si não mudou em relação à 0.93.2.

## Adicionado
- **Gerador para Android:** pacote `PS2Birds_Classic_0.93.3_Gerador_Android.zip` com o `GERAR_ISO_ANDROID.sh` e o `LEIA-ME_ANDROID.txt`. Roda no Termux (aplicativo gratuito de terminal): instala sozinho o Python e o ffmpeg na primeira vez, pergunta o jogo, procura o APK na pasta Download, avisa se faltar espaço e grava a ISO e a pasta `apps/PS2Birds` na pasta `PS2Birds` do armazenamento do celular. Mantém o celular acordado durante a geração e abre o painel de acompanhamento no navegador do celular. Não leva Python, ffmpeg nem nada do jogo original. `tools/make_android_kit.py` monta o pacote.
- `tools/paralelo.py`: número de processos paralelos (um por núcleo no computador, 2 no celular; a variável `PS2BIRDS_PROCESSOS` troca).

## Editado
- `tools/make_iso.py`: a ISO é montada lendo cada arquivo durante a gravação, sem carregá-los antes na memória (pico de cerca de 25 MB). A ISO é a mesma (as diferenças de bytes são só as datas de criação, nos primeiros 48 KB).
- `tools/extract_themes.py` e `tools/extract_cutscene.py`: usam `paralelo.py`.
- `tools/progresso.py`: no Termux, o painel abre o navegador do celular pelo `termux-open-url`.
- `tools/make_windows_kit.py`: inclui `paralelo.py`.
- Versão 0.93.3 (README e tela do jogo).

## Excluído
- Da pasta do Drive e do repositório: ISO, ELF, gerador, código-fonte e registros da 0.93.2 (substituídos).

## Observação
- O gerador para Android foi testado aqui no Linux, com o script rodando de ponta a ponta num ambiente que imita o Termux (HOME, pastas de armazenamento e comando `python`). **Não foi testado num celular real**, então a instalação pelo `pkg` e a permissão de armazenamento do Termux seguem a documentação do Termux, sem verificação minha.
