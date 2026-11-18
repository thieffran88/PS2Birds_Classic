# PS2Birds Rio — modificações desta etapa (0.41.0)

## Adicionado
- **Gerador do Rio de ponta a ponta:** `gerar_iso.py --jogo rio` gera a ISO `SLUS_000.11.PS2Birds_Rio.iso` e a pasta `apps/PS2BirdsRio` a partir do APK do jogador (cerca de 30 s). Detalhes em `docs/MODIFICACOES_0.94.2.md`.
- Kits do Windows e do Android levam o Rio; ELF do Rio e ISO entregues nesta versão.

## Editado
- `tools/rio/build_rio_data.py`: etapas, pasta das tabelas e lista padrão de pacotes (`warehouse`, `warehouse2`, `jungle1`: as que o ELF foi compilado para).

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.40.0.md` (e a cópia do Drive), substituído por este.

## O que falta
1. Habilidades vistas em ação, os 49 níveis sem tipo de pássaro, macacos, folhas e frutas, paralaxe por camada, câmera vertical.
2. Interface, fonte e menu do Rio, música da fase, gravidade real, vitória por gaiolas confirmada.
3. Os outros 43 pacotes de fases (o ELF é compilado com as tabelas de sprites dos pacotes atuais).
