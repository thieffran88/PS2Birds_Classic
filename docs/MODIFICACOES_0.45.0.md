# Modificações 0.45.0

- **Frasco azul do Short Fuse (novo no jogo, `VialEffects.lua`)**
  - Ao quebrar, cada porco vivo dentro do raio 7 ganha um clone em escala 0,8 (`blackBirdBlueVialRemoved`, `cloneScale`), que passa a contar como alvo.
  - O motor não cria objetos durante a fase: o importador guarda um clone escondido para cada porco próximo de um frasco azul (29 na campanha); o frasco libera o clone ao lado do porco se ele ainda estiver vivo e no raio.
  - Clones escondidos não entram na contagem inicial de porcos.
  - `CAMPAIGN.BIN`: o porco de origem do clone vai nos bits 16-31 das marcas do objeto (sem mudança de formato).
  - O giro dos clones em volta do frasco (`blueVialSpin*`) não foi portado.
  - Com os três frascos, os efeitos do Short Fuse estão completos.
- **Roteiros refeitos**
  - Short Fuse 1-14 (`BlackBird_14`) e 3-13 (`BlackBird_43`) ganharam porcos clonados e roteiros novos.
- **Testes**
  - `globe_test`: quebrar um frasco azul aumenta o número de porcos.
  - `profile_test`: clones escondidos no `CAMPAIGN.BIN`; o teste dos bolos do Birdday ignora os clones.
  - `make_campaign_table.py`: os porcos da tabela não contam clones escondidos.
  - Suíte com 70 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.45.0; README descreve os três frascos; captura do Short Fuse 1-14; documentos e captura da 0.44.0 substituídos; `LACUNAS_PORT.md` atualizado.
  - Entregues ISO, ELF (com os .BIN), Gerador para Windows e Código-Fonte; os da 0.44.0 removidos da pasta de trabalho.
