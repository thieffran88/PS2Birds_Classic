# Modificações completas — PS2Birds Classic 0.88.0

## Nova cutscene: fim do mundo 1 (`world1_complete`)
- É a cena de meio de Poached Eggs (`STORY_1_MIDDLE_1`): um porco atordoado com estrelinhas, o castelo ao fundo e o Rei Porco com os ovos.
- Toca ao sair da tela de vitória que completa o mundo 1, com a música `birds_boss`.
- Rolagem de 2,0 s e depois a imagem parada até 7,5 s. O mesmo padrão da abertura: rolagem curta e uma pausa de cerca de 6 s no fim.
- **Aproximação:** os dois tempos (2,0 e 7,5) vêm de constantes novas do `cutscenes.lua`, e o modo como o jogo as usa foi deduzido do padrão da abertura. O tamanho de referência do script é 1438, mas o desenho ocupa 1368 px, e usei o tamanho real para não sobrar uma faixa preta.

## Caminho pronto para as outras cutscenes
- O jogo procura as cenas **pelo nome** do `cutscenes.lua`. Uma cena que não está no `CUTSCENE.BIN` é pulada, sem erro.
- **Gatilhos ligados para todos os episódios:**
  - `pack1_intro` a `pack8_intro`: ao entrar pela primeira vez no episódio;
  - `worldN_complete`: ao sair da vitória que completa o mundo N;
  - `worldS1_complete` a `worldS3_complete`: Surf and Turf.
- Acrescentar uma cena agora é só uma linha em `SCENES` de `tools/extract_cutscene.py` e gerar os dados de novo. O passo a passo está no novo `docs/CUTSCENES.md`.
- **Música por cena:** cada cena guarda a sua faixa; a faixa `birds_boss` entrou no `MUSIC.BIN` (agora 15 faixas).
- **Formato do `CUTSCENE.BIN` (versão 2):** nome de cada cena, tempos e faixa de música. O jogo ainda lê o formato antigo.

## Correção: tela congelada antes da cena
- Ao começar uma cena, o jogo ficava quase 4 s mostrando o último quadro da fase, porque lia a música do disco antes de desenhar a cena.
- Agora o primeiro quadro da cena aparece primeiro, e só depois a música é carregada.
- **Conferência:** nas versões anteriores eu só tinha visto a abertura de Poached Eggs pelo log do console. Nesta rodada vi as duas cenas na tela do PCSX2: a abertura (pássaros com interrogações em volta do ninho, depois os porcos com a frigideira) e a cena nova.

## `velocityMultiplier` (preparação, desligado)
- O código que aplica a velocidade conservada ao quebrar uma peça está no jogo, atrás da chave `PS2BIRDS_VELOCITY_MULTIPLIER`, **desligada**. O jogo se comporta como na 0.87.0.
- Ligá-la quebra 86 roteiros de vitória. A busca automática de roteiros novos resolveu 72 dessas fases com um tiro repetido; as 14 restantes estão na busca em feixe, ainda em andamento.

## Documentação
- `LACUNAS_PORT.md`: nova seção **"Resumo do que falta (0.88.0)"** no topo, e cinco linhas antigas corrigidas, que davam como pendente o que já está feito (Big Setup, controladores 72 de 72, sprites e tela de fim de fase do menu, música do fim de fase, pontos flutuantes).
- `CUTSCENES.md` (novo): quando cada cena toca e como acrescentar uma.

## Conferido
- 82 verificações no PC aprovadas.
- No PCSX2, as duas cenas aparecem na tela com a rolagem certa, sem emendas nem faixas pretas (captura da cena nova em `docs/pcsx2_0.88.0_cena.png`).

## Limpeza
- Drive: os documentos e zips da 0.87.0 foram substituídos pelos da 0.88.0.
- Repositório: removidos os documentos da 0.87.0.
- Área temporária: removidas as pastas e os zips da 0.87.0, as prévias, os logs e as cópias de depuração.

## Versão
- Tela de pausa, "Sobre" e README: 0.88.0.
