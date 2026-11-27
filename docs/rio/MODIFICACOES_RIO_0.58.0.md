# Modificações — Rio 0.58.0 (macacos soltam o que os segura)

O Classic continua na 1.7.1.

## O que é
As animações dos macacos (`animations` de cada definição do `blocks`) trazem estados por velocidade: `default` (0 a 3), `getting_up` (3 a 5), `slipping` (5 a 20), `slipping_fatal` (acima de 20), `falling` e `laughing`. Os de `getting_up`, `slipping` e `slipping_fatal` têm `destroyJoints`: ao passar de 3 (ou 5, ou 15, conforme a definição), com um tempo de limiar de 0,2 a 0,5 s, o macaco solta tudo que o segura. É o que faz o macaco de parede ou de teto cair quando é atingido.

## Adicionado
- **Soltar:** um macaco-alvo que se move a 3 ou mais por 0,3 s (depois do primeiro 1 s da fase) tem todas as suas juntas destruídas e o jogo toca `monkey near falling` (3 gravações sorteadas). O limiar é o mais comum do original (3,0); há definições com 5, 15 ou 0 e 0,2 a 0,5 s, que o port ainda trata igual.
- `SOUNDS.BIN` do Rio: 88 efeitos (53 + 35 variantes). O evento reaproveita o bit `GAME_EVENT_ELECTRO` (o Rio não tem eletrocussão): `GAME_EVENT_MONKEY_SLIP`.

## Editado
- `ps2/src/game.cpp` (`monkeys_check`), `game.h`, `audio_ids.h`, `main.c`, `tools/rio/extract_audio_rio.py`. Versão do Rio: 0.58.0.

## Ainda de fora (R5)
- As animações em si: os quadros `DEFAULT`, `IDLE`, `GETUP`, `SLIP`, `FALL`, `LAUGH` e o piscar (cada uma pediria compor mais sprites por macaco; os macacos continuam com um quadro só) e o riso quando a fase é perdida.
- Os limiares por definição e o som em laço de `monkey_near_falling`/`monkey_falling`.

## Conferido
- Simulação de 5 s em 6 fases com macacos: o resultado é o mesmo de antes (o soltar só acontece depois de uma pancada). Não vi um macaco de parede sendo derrubado por um tiro, nem ouvi o som.
