# PS2Birds Rio — modificações desta etapa (0.44.0)

Resposta ao vídeo do som e à instabilidade das peças.

## Som
- Do vídeo (`SLUS-00011`, 14,8 s): até 8 s há música e o som de início de fase; depois, um zumbido de timbre constante (harmônicos em torno de 1 kHz, pulsos de 0,107 s e repetição de 0,32 s) até o fim, com a fase parada. Não bate com nenhum efeito do Rio; é o anel de áudio repetindo dados antigos. A fase do Rio não tem música de fase, então o jogo parava a música do menu sem esvaziar o anel. Corrigido em `ps2/src/music.c`.

## Peças
- **Causa da instabilidade:** a simulação sem tiros das 45 fases mostrou peças se mexendo em todas (417 peças a mais de 10 px depois de 30 s com o chão e os tamanhos atuais). As fases são salvas em repouso; o jogo as criava acordadas e as folgas de 0,1 a 0,2 unidade entre as peças bastavam para a torre da primeira fase se desfazer (gaiolas do topo escorregando).
- **Correção:** as peças começam dormindo no ELF do Rio (o gancho `SCENE_STARTS_ASLEEP` do motor). Resultado da simulação: 0 peças se mexendo nas 45 fases; no emulador, a primeira fase fica paradas por 25 s com as 5 gaiolas no topo.
- O fator de tamanho continua 0,92 (constante do script de criação de objetos); testei 0,90 a 1,00 e ele não é a causa.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.43.0.md` (e a cópia do Drive), substituído por este.

## O que falta
1. Mira e gravidade (a gravidade não está em nenhum script), acentos nas telas e a interface do Rio.
2. Conferir o som depois da correção, com a ISO nova.
3. Outras fases: passar uma a uma (lâmpadas penduradas e fases com macacos ainda se movem quando tocadas).
