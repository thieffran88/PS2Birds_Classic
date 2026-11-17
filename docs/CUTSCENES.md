# Cutscenes — como acrescentar cenas

O port toca as cutscenes do `cutscenes.lua` pelo **nome**. O jogo já sabe quando cada uma deve
aparecer; uma cena que ainda não está no `CUTSCENE.BIN` é simplesmente pulada. Para acrescentar
uma cena basta registrá-la no gerador — nenhuma mudança no código do jogo é necessária.

## Quando cada cena toca (já ligado no jogo, `ps2/src/main.c`)

| Momento | Nome procurado |
| --- | --- |
| Entrar na primeira fase de um episódio que ainda não tem nenhuma pontuação | `pack1_intro` … `pack8_intro` (Poached Eggs, Mighty Hoax, Danger Above, Big Setup, Ham 'Em High, Mine and Dine, Birdday, Surf and Turf) |
| Sair da tela de vitória quando essa vitória completou um mundo (todas as fases com título `"<EPISÓDIO> N-"` pontuadas) | `worldN_complete` (N = número do mundo no título; Poached Eggs 1–3, Mighty Hoax 4–5, Danger Above 6–8, …) |
| O mesmo no Surf and Turf | `worldS1_complete` … `worldS3_complete` |

X ou Start pulam a cena. A música é a de cada cena (coluna "Música" da tabela no fim).

## Como acrescentar uma cena

1. **Leia o bloco da cena no `cutscenes.lua`** (o arquivo é bytecode Lua; os valores aparecem
   como constantes perto do nome da cena — o `extract_cutscene.py` mostra como a primeira foi lida):
   - `image` — o nome da composição no `CUTSCENES.compo.json` (por exemplo `STORY_1_MIDDLE_1`);
   - `setreferencesize` — largura e altura de referência (números `float` de 4 bytes);
   - `playsound` → `sound` — o clipe de música (por exemplo `birds_boss`); ele precisa estar na
     lista `MUSIC` de `tools/extract_audio.py` (acrescente uma linha se não estiver);
   - `scroll` → `duration` — o tempo da rolagem em segundos;
   - o último `time` do bloco — quando a cena termina.
   Dica: as constantes de um bloco só aparecem na primeira vez em que são usadas no arquivo; o que
   se repete (como a altura 659) vem do bloco anterior.
2. **Acrescente uma linha em `SCENES`** em `tools/extract_cutscene.py`:
   ```python
   SCENES=[('pack1_intro','STORY_1_BEGINNING',2048,659,9.6,15.6,'birds_intro'),
           ('world1_complete','STORY_1_MIDDLE_1',1368,659,2.0,7.5,'birds_boss'),
           ('world2_complete','STORY_1_MIDDLE_2',LARGURA,ALTURA,ROLAGEM,FIM,'CLIPE')]
   ```
   O primeiro campo é o nome que o jogo procura (tabela acima). Use como largura a que o desenho
   realmente ocupa (a caixa das peças da composição), que pode ser um pouco menor que a de referência.
3. **Gere os dados de novo** (`GERAR_ISO.bat`, ou `python3 tools/gerar_iso.py APK --dev` no
   desenvolvimento). Nada do jogo fica no repositório: a cena é montada a partir do seu APK.

## Limites conhecidos

- Cada cena ocupa cerca de 1,25 MB no disco e na RAM enquanto toca (1392×448 em 16 bits; o
  tamanho varia com a largura de referência). Só uma cena fica na memória por vez.
- O reprodutor faz a rolagem simples da esquerda para a direita (`cubic_in_out`). Cenas com
  vários quadros e efeitos (`createsprite`/`deletesprite`, `fittogrid`, olhos animados do Rei
  Porco, moldura de filme `CUTSCENE_FILMSTRIP_*`) precisariam de suporte novo no `cutscene.c`.
- Formato do `CUTSCENE.BIN`: "PBCS", versão 2, número de cenas, depois por cena o nome (24 bytes),
  largura, altura, rolagem (ms), fim (ms), posição dos dados e faixa de música; o jogo ainda lê a
  versão 1 antiga (uma cena só, a abertura).

## Cenas feitas

| Cena | Imagem | Música |
| --- | --- | --- |
| `pack1_intro` | `STORY_1_BEGINNING` | `birds_intro` |
| `world1_complete` | `STORY_1_MIDDLE_1` | `birds_boss` |
