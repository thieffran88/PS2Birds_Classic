# Modificações completas — PS2Birds Classic 0.84.0

## Pássaros da fila pulando (`gamelogic.lua`, `jumpTimer`)
- No original, os pássaros que esperam ao lado do estilingue dão pulinhos de vez em quando.
- No port, cada pássaro da fila espera um tempo aleatório de 1 a 4 s e então pula 10 px, numa curva de 0,35 s.
- É uma aproximação: os tempos exatos ficam no código compilado do `gamelogic.lua`, não em tabelas de dados.
- A física e a pontuação não mudam: o pulo é só no desenho.

## Limpeza
- No Drive:
  - foram para a lixeira os zips da 0.81.0 e da 0.82.0, que você mesmo tinha enviado;
  - os zips e documentos da 0.83.0 foram substituídos pelos da 0.84.0.
- No repositório:
  - removidas as capturas antigas do PCSX2 (0.73 a 0.83); fica só a da versão atual;
  - removidos os documentos da 0.83.0.
- Na área temporária: removidas a pasta e os zips da 0.83.0.

## Conferido
- 81 verificações no PC aprovadas.
- No PCSX2, a fase 1-1 roda com a fila animada (captura em `docs/pcsx2_0.84.0_fila.png`).

## Versão
- Tela de pausa, "Sobre" e README: 0.84.0.
