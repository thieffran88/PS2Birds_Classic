# Relatório do projeto — versão para quem não é da área técnica

*Atualizado em 17/11/2026*

## O que é este projeto

O objetivo é fazer um jogo de estilingue de passarinhos, que existe para celular, rodar num PlayStation 2. O projeto tem duas frentes:

- **Versão "Clássica":** já funciona, é o trabalho principal feito até agora e tem versões prontas para uso (a última é a 0.93).
- **Versão "Rio":** um segundo jogo da mesma série, que está sendo adaptado reaproveitando a maior parte do trabalho da primeira.

## Como o projeto respeita os direitos do jogo original

O projeto **não distribui nenhum conteúdo do jogo original** (imagens, sons, fases). Quem quiser jogar usa o próprio arquivo do jogo, que obtém por conta própria, e uma ferramenta converte esse arquivo no formato do PlayStation 2 no computador da própria pessoa. O que é publicado são só o programa adaptado e o código-fonte, com licença livre.

## O que já está pronto (Clássica)

- Centenas de fases jogáveis, com sons, música, menus e histórias em quadrinhos entre os capítulos.
- Salvamento de progresso no cartão de memória.
- Botões na tela com os símbolos do controle, menus que se navegam pelo direcional e uma correção de um chiado que aparecia ao silenciar o jogo.
- Testes automáticos extensos: o programa é conferido por centenas de verificações a cada mudança importante.

## Como está a versão Rio

Nesta etapa do trabalho a versão Rio saiu do zero até um ponto em que **os dados do jogo são lidos e convertidos**, mas **ainda não existe um jogo para rodar**. Em termos simples:

**Já feito**
- O arquivo do jogo foi analisado por inteiro: as fases foram lidas, a ordem delas foi descoberta e a primeira fase (do primeiro capítulo) foi escolhida como alvo.
- As **30 fases do primeiro capítulo** já são convertidas e carregam no motor do jogo. Em cada uma, as peças assentam sem desabar e um lançamento de teste funciona sem erros.
- O objetivo do jogo (libertar as aves presas em gaiolas, em vez de derrotar porcos) já é reconhecido pelo motor, sem precisar alterá-lo.
- As imagens, o fundo, os efeitos sonoros e a música da primeira fase são convertidos para o formato do PlayStation 2. Isso inclui dois formatos de imagem que o projeto ainda não sabia ler.
- O salvamento do Rio fica separado do salvamento da versão Clássica: um não estraga o progresso do outro.
- Uma versão de teste do programa do Rio já é gerada (ainda sem imagens de menu e letras).

**Ainda falta**
- Menus, letras e telas do Rio.
- Conferir tudo rodando de verdade no emulador de PlayStation 2 (até agora, os testes foram feitos no computador, sem ver o jogo na tela).
- Algumas peças que usam um formato de imagem que o projeto ainda não consegue ler (por exemplo, certas caixas de metal).
- As habilidades dos pássaros do Rio (o azul do Rio, por exemplo, tem uma habilidade diferente da do jogo clássico) e os personagens novos: macacos, a ave Luiz e os chefes.
- Ajustar a "sensação" da física (a gravidade usada ainda é um valor provisório, porque não havia como medi-la no jogo original).

## O que as medições mostraram

- Das fases do primeiro capítulo, só três tipos de pássaro aparecem (vermelho em 42% dos lançamentos, amarelo em 30% e azul em 28%). Isso ajuda: dá para chegar a algo jogável sem implementar todos os pássaros de uma vez.
- Três dos quatro pássaros que já existem no motor têm o mesmo "peso e tamanho" nos dois jogos. Só o vermelho do Rio é um pouco maior e mais saltitante, e isso já foi ajustado.

## Erros encontrados e corrigidos no caminho

- Uma versão de teste do Rio estava, sem querer, reaproveitando peças da versão Clássica e usando o local de salvamento errado. Foi detectado ao conferir o resultado e corrigido, e a separação entre os dois jogos agora é verificada.

## Limites e honestidade

- Nada da versão Rio foi visto rodando num PlayStation 2 nem num emulador. Os resultados vêm de testes no computador e de leitura dos dados.
- As escolhas feitas "por nome" (por exemplo, qual música toca no título) são aproximações e não foram ouvidas.
- Não há data prevista para uma versão Rio jogável.

## Próximos passos

1. Criar menus e letras do Rio.
2. Rodar no emulador e ajustar o que aparecer.
3. Implementar as habilidades dos pássaros e depois os personagens novos.
