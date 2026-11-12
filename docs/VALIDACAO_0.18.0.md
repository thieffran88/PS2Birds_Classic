# Validação 0.18.0

- Suíte completa aprovada: 25 verificações, incluindo campanha de 84 fases, save, áudio, sprites, procedência e testes isolados de cada pássaro, TNT e triângulos.
- Campanha: 82 vitórias com roteiros determinísticos; 3-9 e 3-19 sem roteiro, carregadas, contadas e confirmadas sem vitória automática.
- Velocidade: em fases com zoom de 7 a 3, o pássaro sai com 46,25 unidades/s a 100% e 23,125 a 50%.
- Chão e estilingue: conferidos nas 84 fases contra Ground.y − 5 e o primeiro pássaro da fila.
- Fases antes impossíveis: 3-2 vence com quatro tiros (32000), 3-20 com cinco (59500), 3-21 com cinco (78500) e 4-4 com um tiro a −5° (35000).
- ELF e ISO compilados a partir de `make clean`; ELF dentro da ISO idêntico ao distribuído; ZIP de fontes recompilado e testado em pasta nova.

Sem execução no PCSX2 e sem teste em PS2 físico. A conferência visual do estilingue na nova altura, do chão e da troca de bancos de textura depende do emulador.
