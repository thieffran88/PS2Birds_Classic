# Modificações — 0.94.4

Versão de **correções do Rio, primeira fase**: tamanho dos sprites e posição do chão. O jogo Classic não mudou.

## Adicionado
- `docs/MODIFICACOES_0.94.4.md`, `docs/VALIDACAO_0.94.4.md` e `docs/rio/MODIFICACOES_RIO_0.43.0.md`.

## Editado
- **Chão do Rio no lugar certo** (`tools/rio/rio_import_level.py`): o chão de cada fase ficava 5 unidades abaixo de onde as peças foram colocadas (usava o centro do objeto `Ground` como se fosse o topo; o Classic já subtraía a metade da altura, 5). As peças da primeira fase caíam 5 unidades ao começar, e a torre desmoronava sozinha. Conferido nas 45 fases: a peça mais baixa de cada uma termina em y = 0, que agora é o topo do chão.
- **Sprites no tamanho certo** (`ps2/src/main.c`, ramo do Rio): o banco usa a versão SD das imagens (metade dos pixels que `physicsToWorld` conta), então tudo era desenhado com a metade do tamanho do corpo físico (caixas pequenas dentro de corpos grandes, parecendo flutuar). Agora cada pixel SD vale o dobro.
- **Juntas** (da 0.94.3, já incluídas) e **peças fixas** continuam como antes.
- README e versão 0.94.4.

## Excluído
- Da pasta do Drive e do repositório: documentos, código-fonte, ISO, ELF e kits da 0.94.3 (substituídos) e a documentação 0.42.0 do Rio.

## Observações
- Conferido no emulador: a primeira fase mostra a torre de caixas com as gaiolas no topo, parada; os tiros derrubam as caixas e a fase termina. Ainda pendentes: mira, som, gravidade, acentos nas telas e a interface do Rio.
