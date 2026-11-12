# Validação 0.19.0

- PCSX2 v2.9.148 (pasta Ferramentas, SHA-256 conferido) com BIOS scph39001, sob Xvfb: a ISO 0.19.0 inicia, mostra a versão 0.19.0 e entra na fase 1-1 a 60 FPS. Captura em `docs/pcsx2_0.19.0_fase_1-1.png`.
- ELF de teste aberto direto na 4-1 com a física corrigida: castelo de pé, triângulos de pedra nos telhados e porcos no lugar. Captura em `docs/pcsx2_0.19.0_fase_4-1.png`. Na 0.18.0 a mesma cena aparecia desmoronada.
- ELF de teste da 3-2 (0.18.0): estilingue no alto da plataforma, como no original.
- Áudio sem saída no emulador porque o ambiente não tem placa de som (CUBEB); o jogo segue com saída nula.
- Suíte completa aprovada: 26 verificações, incluindo tamanho físico com fator 0,92 e materiais conferidos em todas as fases.
- Campanha: 83 vitórias com roteiros determinísticos; 3-20 sem roteiro, carregada, contada e confirmada sem vitória automática.
- Acomodação: fases com deslocamento inicial acima de 3 unidades caíram de 13 para 8.
- ELF e ISO compilados a partir de `make clean`; ELF dentro da ISO idêntico ao distribuído; ZIP de fontes recompilado e testado em pasta nova.

Partidas completas não foram jogadas no emulador; a física é a mesma testada no computador. Sem teste em PS2 físico.
