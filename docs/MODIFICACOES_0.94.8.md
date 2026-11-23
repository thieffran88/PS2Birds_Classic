# Modificações — 0.94.8

Versão da **cena final do Dino**. O jogo em si não mudou; só o `CUTSCENE.BIN` gerado.

## Editado
- **`useReferenceSize` resolvido** (`tools/extract_cutscene.py`): o alvo da rolagem é a posição do sprite mais a fração do tamanho. Na abertura do Dino o sprite fica em (0, 0): o alvo 0,75 continua sendo 75% da imagem (sem mudança). No final do Dino o sprite fica centrado em (716, 384) de uma imagem de 1432 x 768 e o alvo é (0,5; −0,5): 716 + 0,5 × 1432 é a borda direita e 384 − 0,5 × 768 é o topo. A cena rola até o fim da imagem (antes ficava parada porque eu tratava 0,5 como metade da imagem).
- `docs/CUTSCENES.md`, README e versão 0.94.8.

## Adicionado
- `docs/MODIFICACOES_0.94.8.md`, `docs/VALIDACAO_0.94.8.md` e `docs/rio/MODIFICACOES_RIO_0.47.0.md`.

## Excluído
- Da pasta do Drive e do repositório: documentos, código-fonte, ISO, ELF e kits da 0.94.7 (substituídos) e a documentação 0.46.0 do Rio.

## Observações
- O raciocínio vem dos números do próprio roteiro (a posição do sprite do final bate exatamente com o centro da imagem de referência); não há vídeo do original para comparar.
