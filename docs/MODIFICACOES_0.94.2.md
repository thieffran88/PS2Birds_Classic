# Modificações — 0.94.2

Versão do **gerador**: agora gera também a ISO do **Angry Birds Rio** (experimental). O jogo Classic em si não mudou em relação à 0.94.1.

## Adicionado
- **Gerador do Rio de ponta a ponta** (`tools/gerar_iso.py --jogo rio`): analisa o APK do Rio (reaproveita a análise se já foi feita), converte as fases, os sprites, os temas e os sons e grava a ISO `SLUS_000.11.PS2Birds_Rio.iso`, mais a pasta `apps/PS2BirdsRio` (ELF + dados + `title.cfg`). Com o APK do Rio leva cerca de 30 segundos. Etapas, tempo, registro e painel iguais aos do Classic.
- Kits do Windows e do Android com o Rio: ferramentas do Rio, tabelas de sprites (para conferir contra o ELF) e o `PS2BIRDS_RIO.ELF`.
- ELF do Rio (`PS2Birds_Rio.elf`, de `make GAME=rio`) e ISO do Rio, gerados nesta versão.
- `docs/MODIFICACOES_0.94.2.md`, `docs/VALIDACAO_0.94.2.md` e `docs/rio/MODIFICACOES_RIO_0.41.0.md`.

## Editado
- `tools/gerar_iso.py` (rota do Rio e aviso se as tabelas de sprites geradas diferem das compiladas no ELF), `tools/rio/build_rio_data.py` (etapas, tabelas em outra pasta e lista padrão de pacotes), `tools/make_windows_kit.py` e `tools/make_android_kit.py` (Rio no kit), `GERAR_ISO.bat` (o Rio grava em `apps\PS2BirdsRio`), `GERAR_ISO_ANDROID.sh`, os LEIA-ME, `docs/GERADOR_ANDROID.md`, README e versão 0.94.2.

## Excluído
- Da pasta do Drive e do repositório: documentos, código-fonte, ISO, ELF e kits da 0.94.1 (substituídos) e a documentação 0.40.0 do Rio.

## Observações
- A ISO do Rio é experimental: primeiro episódio (30 fases) e o primeiro mundo da selva (mais 15, 45 no total), sem interface, fontes e menus do Rio (o jogo usa os provisórios), sem música de fase, com gravidade provisória.
- Testado: geração a partir do kit do Android sem o repositório (cold), ISO do Rio no emulador (abre a seleção de fases com o fundo do galpão), `make test`.
