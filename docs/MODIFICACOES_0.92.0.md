# Modificações completas — PS2Birds Classic 0.92.0

## Adicionado
- **Cutscene da abertura de Danger Above** (`pack3_intro`), a 29ª cena:
  - a imagem é ajustada à largura da tela e rola na **vertical**: começa embaixo, nos pássaros, sobe passando pelo pote de ovos pendurado e termina na cesta de balões dos porcos;
  - a rolagem começa aos 4,0 s e dura 9,9 s, e a cena termina aos 16 s, como no script;
  - toca ao entrar na primeira fase de Danger Above sem nenhuma pontuação.
- **Rolagem vertical no reprodutor de cutscenes:** o `CUTSCENE.BIN` ganhou um campo de opções (rolagem vertical e começar pelo fim); o formato passou a ser a versão 4.
- **Cor de fundo das cenas** (`set_bg_colour` do script): agora é lida e aplicada atrás das peças. A abertura de Danger Above usa o azul-céu.
- **`ps2/tests/AUXILIARES.md`:** documenta, em português, as ferramentas de busca de roteiros (`solver`, `solver3` e `beam.py`, `ed_solver`, `settle_probe` e `make_campaign_table.py`), com a receita usada na 0.90.0. Eram arquivos sem nenhuma explicação.
- Documentos novos da versão: `MODIFICACOES_0.92.0.md`, `VALIDACAO_0.92.0.md` e a captura `docs/pcsx2_0.92.0_cena.png`.

## Editado
- **`extract_cutscene.py`:** lê a direção da rolagem (`fitwidth` ou `fitheight` e `set_scroll_position`) e a cor de fundo do script, e monta cenas verticais (640 de largura).
- **`cutscene.c`:** lê o formato versão 4 e desenha a rolagem vertical, subindo ou descendo.
- **`main.c`:** a abertura de Danger Above entrou na tabela de gatilhos dos episódios.
- **`CUTSCENE.BIN`:** passou de 27,5 MB para 28,6 MB, com a cena nova.
- **`CUTSCENES.md`:** 29 cenas, a abertura de Danger Above saiu da lista do que ficou de fora, e há uma seção sobre a rolagem vertical e o novo formato.
- **`LACUNAS_PORT.md`:**
  - o resumo do topo e a seção de cutscenes foram atualizados;
  - **Short Fuse:** o levantamento antigo dava como pendentes os frascos, o foguete, o ímã e o balão explosivo. Os frascos, o balão explosivo e as peças de foguete já estavam no jogo. O ímã não tem nenhuma referência nos scripts do APK, então não há o que portar.
- Tela de pausa, "Sobre" e README: 0.92.0. `HISTORICO_MODIFICACOES.md` atualizado.

## Excluído
- **No repositório:** os documentos e a captura da 0.91.0, substituídos pelos da 0.92.0.
- **No Drive:** os documentos da 0.91.0, e os zips da 0.91.0 se você os tiver enviado.
- **Na área temporária:** a pasta e os zips da 0.91.0, as capturas, as prévias e os logs de teste.
- **Auditoria de arquivos sem relevância:** conferi todas as ferramentas, os testes e os arquivos de apoio. Nenhuma ferramenta está sem uso, e os arquivos de apoio dos testes (`*_fixture.h`) são usados pelo `game.cpp`. Os únicos itens sem referência eram os auxiliares de busca de roteiros, que mantive e documentei, em vez de apagar. Os exemplos da biblioteca de som já tinham sido removidos na 0.91.0.

## Conferido
- 82 verificações no PC aprovadas.
- No PCSX2, a abertura de Danger Above em três momentos (embaixo, subindo, no topo), com a rolagem vertical e as cores do original.

## O que continua de fora das cutscenes
- As cenas do Egg Defender (painéis animados).
- Short Fuse, Piggy Farm e Dino (imagens em WebP, que o gerador não decodifica).
- Birdday 18/19 e o episódio de `pack9`: sem correspondência conhecida com o port.
- As animações dentro das cenas (olhos do Rei Porco, "oink").

## Versão
- Tela de pausa, "Sobre" e README: 0.92.0.
