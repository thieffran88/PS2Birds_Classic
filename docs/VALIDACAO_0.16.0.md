# Validação 0.16.0

- Campanha de 63 fases percorrida nos testes nativos com bloqueios, contagens, desbloqueios, recordes, reinício, filas mistas, tempo decorrido a 30/60/120 Hz e 50 trocas de mundo. As 42 pontuações anteriores são idênticas às da 0.15.0.
- Terceira página: 17 vitórias com 47500/26500/40000/56500/45500/50000/38500/46500/35500/56000/39000/22500/45000/48500/32000/58500/71500 pontos (3-1, 3-3 a 3-18).
- Sem roteiro vencedor: 3-2 Level77 (22 objetos, 6 porcos), 3-19 Level22 (91/5), 3-20 Level89 (107/9) e 3-21 Level81 (135/9). Buscas: tiro único em grade de 5°/10 de força com cinco tempos de habilidade, e feixe por pássaro de largura 3 e 5 com onze tempos de habilidade.
- Atlas: 152 sprites, quatro páginas, 2.883.584 bytes; hashes, limites, sobreposição e margem de 160 KiB de VRAM conferidos.
- Procedência: 21 fases da terceira página iguais ao trecho de episodes.lua; raio do rei igual à definição.
- Save: expansão 35→42 e 42→63, migrações anteriores e proteção contra saves futuros aprovadas.
- TNT, preto, branco, azul, amarelo, áudio, Box2D e perfis aprovados sem alteração.
- ELF e ISO compilados a partir de `make clean`; ELF dentro da ISO idêntico ao distribuído; ZIP de fontes recompilado e testado em pasta nova.

Sem execução no PCSX2 nesta rodada; o emulador da pasta Ferramentas não está acessível no ambiente. A quarta página de textura aumenta o uso de VRAM em 256 KiB em relação à versão já testada no emulador, e isso ainda não foi verificado em execução. Sem teste em PS2 físico.
