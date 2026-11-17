# Modificações completas — PS2Birds Classic 0.89.0

## Nova cutscene: fim do mundo 2 (`world2_complete`)
- É a cena de meio de Poached Eggs 2 (`STORY_1_MIDDLE_2`): o porco de bigode atordoado com estrelinhas, o castelo ao fundo e o Rei Porco com os ovos.
- Toca ao sair da tela de vitória que completa o mundo 2, com a música `birds_boss`.
- **Aproximação:** o bloco da cena no `cutscenes.lua` não traz nenhum número novo, o que indica que ele reaproveita os valores da cena anterior. Usei os mesmos: rolagem de 2,0 s e fim aos 7,5 s. A composição tem o mesmo tamanho da do mundo 1 (1368×657).

## Correção: personagens das cutscenes saíam semitransparentes
- Os sprites trazem a transparência na escala do PS2 (0 a 128), e a composição das cenas a tratava como se fosse 0 a 255. O resultado era que tudo saía com a metade da opacidade: dava para ver o sol através do porco, e o ninho e as cores ficavam desbotados.
- Agora a conversão é feita corretamente. Os personagens ficam opacos e com as cores do original (o bigode laranja, os ovos brancos).
- A correção vale para as três cenas: a abertura, a do mundo 1 e a nova.
- O defeito existia desde a abertura da 0.86.0 e só ficou evidente com a cena nova.

## Conferido
- 82 verificações no PC aprovadas.
- No PCSX2, a cena nova na tela, com cores e opacidade certas (captura em `docs/pcsx2_0.89.0_cena.png`).

## Documentação
- `docs/CUTSCENES.md`: nova cena na tabela das feitas e dica sobre blocos sem números novos.
- `LACUNAS_PORT.md`: as cenas feitas passam a ser três; lista renumerada para a 0.89.0.

## Limpeza
- Drive: os documentos e zips da 0.88.0 foram substituídos pelos da 0.89.0.
- Repositório: removidos os documentos e a captura da 0.88.0.
- Área temporária: removidas a pasta e os zips da 0.88.0, a grade de screenshots e os arquivos de teste.

## Versão
- Tela de pausa, "Sobre" e README: 0.89.0.
