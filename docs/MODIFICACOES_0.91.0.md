# Modificações completas — PS2Birds Classic 0.91.0

## Adicionado
- **25 cutscenes novas** (agora são 28), com as imagens montadas do seu APK:
  - aberturas de Mighty Hoax, Big Setup, Ham 'Em High, Mine and Dine, Birdday e Surf and Turf;
  - cenas de fim de mundo: Poached Eggs 3, Mighty Hoax 4 e 5, Danger Above 6, 7 e 8, Big Setup 9, 10 e 11, Ham 'Em High 12, 13 e 14, Mine and Dine 15, 16 e 17, Birdday 3 e Surf and Turf 1, 2 e 3.
  - As três cenas que já existiam (abertura de Poached Eggs e fim dos mundos 1 e 2) continuam.
- **Faixa de música `birds_outro`** (a trilha das cenas de encerramento) no `MUSIC.BIN`, que passou a ter 16 faixas.
- **Leitura da tabela do script de cutscenes:** o leitor de dados (`lua51_data.py`) passou a aceitar soma, subtração, multiplicação e divisão entre números. Continua recusando chamadas, funções e saltos, então nenhum código do jogo roda. Foi isso que permitiu ler o `cutscenes.lua` inteiro.
- **Tabelas de gatilhos** no `main.c` (`episode_scene` e `world_scene`): ligam cada cena ao episódio ou ao mundo certo, de forma explícita.
- **Aviso na geração** quando uma cena não pode ser montada (formato de imagem sem suporte), em vez de travar.
- Documentos novos da versão: `MODIFICACOES_0.91.0.md`, `VALIDACAO_0.91.0.md` e a captura `docs/pcsx2_0.91.0_cena.png`.

## Editado
- **Extrator das cutscenes (`extract_cutscene.py`):** agora é guiado pelo próprio script. Imagem, tamanho, música, início e duração da rolagem e fim de cada cena vêm do `cutscenes.lua`, e não de valores que eu copiava à mão.
- **Correção do tempo das cenas:** o script começa a rolagem alguns segundos depois do início (entre 2,5 s e 4,5 s, conforme a cena), com a imagem parada antes disso. Eu rolava desde o primeiro segundo, então a abertura e as cenas dos mundos 1 e 2 estavam com a temporização errada. Os valores que eu tinha deduzido para a duração e o fim estavam certos.
- **Correção dos gatilhos:** o jogo montava o nome da cena a partir do número no título da fase, então completar "BIRDDAY 2" dispararia a cena `world2_complete`, que é de Poached Eggs. Agora cada gatilho vem de uma tabela explícita.
- **Reprodutor (`cutscene.c`):** lê o novo formato do `CUTSCENE.BIN` (versão 3), respeita o início da rolagem e toca a música guardada em cada cena.
- **`CUTSCENE.BIN`:** passou de 2,9 MB para 27,5 MB, por causa das 28 cenas. Cada cena só ocupa a RAM enquanto toca.
- **`extract_audio.py`:** acrescenta a faixa `birds_outro`.
- **Documentação:** `CUTSCENES.md` reescrito (como funciona, as 28 cenas, o que ficou de fora e como acrescentar mais); `LACUNAS_PORT.md` com a seção de cutscenes atualizada; `README.md`, tela de pausa e "Sobre" em 0.91.0; `HISTORICO_MODIFICACOES.md` atualizado.

## Excluído
- **No repositório:**
  - os exemplos da biblioteca de som incluída (`ps2/vendor/audsrv/ee/samples`: 17 arquivos, 196 KB). Nenhuma parte do projeto os usava;
  - os documentos da 0.90.0 e a captura da 0.90.0, substituídos pelos da 0.91.0;
  - a lógica antiga dos gatilhos, que dependia do número no título.
- **No Drive:** os documentos da 0.90.0, e os zips da 0.90.0 se você os tiver enviado.
- **Na área temporária:** a pasta e os zips da 0.90.0, as pastas `__pycache__` da pasta de ferramentas, as capturas e os logs de teste.

## O que ficou de fora das cutscenes
- A abertura de Danger Above (rola na vertical) e as cenas do Egg Defender (painéis animados).
- Short Fuse, Piggy Farm e Dino: as imagens estão em WebP, que o gerador não decodifica.
- Birdday 18/19 e o episódio do `pack9`: não dá para saber a que mundo do port correspondem.
- As animações dentro das cenas (os olhos do Rei Porco, o "oink").
- Detalhes em `docs/CUTSCENES.md`.

## Conferido
- 82 verificações no PC aprovadas.
- No PCSX2:
  - ao entrar na 1-1 sem pontuação, a abertura toca com a rolagem certa e as cores do original;
  - a cena `world3_complete` aparece em três momentos (parada, rolando, final), com a música própria.
- Todas as 28 cenas foram geradas, e o log lista os tempos de cada uma.

## Versão
- Tela de pausa, "Sobre" e README: 0.91.0.
