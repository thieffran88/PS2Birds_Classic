# Modificações completas — PS2Birds Classic 0.93.0

## Adicionado
- **Ícones dos botões no lugar dos nomes.** Onde o jogo escrevia "X:", "O:", "TRIANGULO:", "L2/R2:", "DIRECIONAL:" etc., agora aparecem os ícones:
  - os quatro botões de ação, com o anel e o símbolo coloridos como na sua imagem (triângulo verde-água, círculo vermelho, X azul-arroxeado, quadrado rosa);
  - o direcional, em azul-acinzentado, com os braços que a dica representa em destaque (cima e baixo, esquerda e direita, ou todos);
  - L1, L2, R1, R2 e R3, além de START e SELECT, em pílulas com o nome.
  - Aparecem nos rodapés dos menus (tela inicial, episódios, fases), na pausa, na dica do power-up (L2) e na mensagem do Memory Card ("L1 tentar").
- **Navegação nos pop-ups de vitória e de derrota**, igual à dos menus:
  - o direcional escolhe entre Menu, Reiniciar e Próxima (só Menu e Reiniciar na derrota);
  - X confirma, e O volta ao menu;
  - o botão escolhido fica um pouco maior, com um anel fino atrás e o nome embaixo;
  - o cursor começa em "Próxima" na vitória e em "Reiniciar" na derrota.
- **Teste novo do som** (`music_test.cpp`): confere que silenciar para e esvazia o fluxo de música, que silenciar de novo não repete a parada, e que religar devolve o volume e retoma a música.
- **Licença MIT** (`LICENSE`, em nome de ThiefFran), **aviso legal e licenças de terceiros** no README, e `.gitignore` para capturas de tela (feitos depois da 0.92.0 e entregues nesta versão).
- Documentos novos da versão: `MODIFICACOES_0.93.0.md` e `VALIDACAO_0.93.0.md`.

## Editado
- **Círculo amarelo da pausa:** era um halo de cerca de 94 px que cobria o rótulo "CONTINUAR". Agora é um anel fino, só um pouco maior que o botão (o desenho do botão tem uma margem transparente, que eu descontei). O botão escolhido cresce 10% em vez de 18%.
- **Pausa:** os botões subiram um pouco para as dicas de controle caberem embaixo, sem bater no rótulo "SOM" nem sair da tela.
- **Pop-ups de vitória e de derrota:** ficaram mais altos para caber o nome do botão e a dica de controles. Os rótulos fixos "O", "TRI" e "X" sob os botões saíram.
- **Atalho do Triângulo:** deixa de reiniciar enquanto o pop-up de vitória ou de derrota está na tela (durante a partida continua reiniciando).
- **Chiado ao silenciar:** ao desligar o som, a música só deixava de ser alimentada, e o que já estava no buffer tocava até acabar e depois repetia lixo (o chiado). Agora o fluxo de música é parado e esvaziado, e o volume zera; ao religar, o volume volta e a música é realimentada. As mensagens de áudio e o botão de som da pausa e do menu continuam iguais.
- `ui.c` e `ui.h`: nova função para obter a largura dos botões da interface.
- **README e documentos:** versão 0.93.0, aviso legal, licença, e as referências às capturas de tela passaram a dizer que elas não estão no repositório.

## Excluído
- **No repositório (já feito após a 0.92.0):**
  - as 67 capturas de tela de todo o histórico, que mostravam a arte da Rovio;
  - o seu e-mail pessoal nos 19 commits antigos, trocado pelo e-mail anônimo do GitHub. O histórico foi reescrito e substituído no GitHub (133 commits).
  - os documentos da 0.92.0, substituídos pelos da 0.93.0.
- **Nos pop-ups:** os rótulos "O", "TRI" e "X" e os textos de ajuda com o nome dos botões.
- **No Drive:** os documentos da 0.92.0, e os zips da 0.92.0 se você os tiver enviado.
- **Na área temporária:** a pasta e os zips da 0.92.0, as capturas, os logs e os arquivos de teste.

## Observações
- **Os ícones são desenhados pelo próprio jogo**, com formas simples, no mesmo estilo e nas mesmas cores da primeira linha da sua imagem. Não usei a sua imagem em si: ela não faz parte do APK nem do repositório, e eu não sei de que pacote de ícones ela vem nem qual é a licença. Se você quiser os ícones idênticos aos dela, me diga a origem e a licença, e eu ajusto.
- **Dificuldade encontrada:** numa das versões de teste, o jogo ficava com a tela preta já no menu. Era um defeito que dependia de como o compilador organizava o código: ligado às funções novas de desenho dos ícones quando embutidas na função de desenho principal, que já é enorme. Não achei a causa exata. As funções novas ficam agora fora de linha, o que resolveu, e acrescentar dados e código sem relação não trouxe a tela preta de volta. Está registrado num comentário do código. Se o jogo ficar preto no seu console, me avise.
- O chiado só pôde ser corrigido pelo que o código fazia (e por teste); não dá para ouvi-lo no emulador que uso aqui, que roda sem som.

## Conferido
- 82 verificações no PC aprovadas.
- No PCSX2: rodapés dos menus, pausa (nos quatro botões) e pop-up de vitória e de derrota com a seleção pelo direcional, todos com os ícones.

## Versão
- Tela de pausa, "Sobre" e README: 0.93.0.
