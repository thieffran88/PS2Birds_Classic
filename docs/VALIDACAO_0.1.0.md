# Validação — PS2Birds Classic 0.1.0

Data: 11/11/2026.

## Escopo

Primeira base técnica nativa para PS2 com cenário próprio de teste e gráficos
procedurais. Verifica compilação, desenho no GS, controle e ciclo de lançamento.
Ainda não é a fase original Poached Eggs nem a execução do motor Android.

## Verificações concluídas

- Compilação sem avisos com GCC 15.2.0 do pacote ps2dev fornecido.
- ELF32 little-endian, máquina MIPS, flags R5900/5900, executável estático.
- Testes C com `-Wall -Wextra -Werror`: limites de mira/força, consistência da
  previsão de trajetória, bloqueio de lançamento repetido, 391 combinações
  vencedoras, derrota após três tentativas e reinício completo.
- ISO9660 com `/SYSTEM.CNF;1` e `/PS2BIRDS.ELF;1`; BOOT2 coerente com o ELF.
- Os bytes do ELF extraído da ISO conferem exatamente com o ELF entregue.
- Boot do ELF e da ISO no PCSX2 v2.9.148 com renderizador Software e BIOS fornecido.
- Controle emulado: X lançou; alvo atingido com 7.000 pontos. Triângulo reiniciou,
  direita alterou a força de 360 para 397 e Start exibiu a tela de pausa.
- Todos os 2.058 arquivos Lua do APK recuperados com AES + LZMA/7z, sem falhas;
  uma fase decompilada para leitura.

As capturas representam execução real no PCSX2. O ambiente não tem saída de áudio
funcional: o emulador informou falha ao abrir o dispositivo e continuou com saída
nula. O protótipo ainda não produz áudio.

## Pendente

- Teste em PS2 físico, incluindo carregadores e desconexão do controle.
- Carregamento e renderização de fases e atlas originais.
- Física de corpos rígidos, dano e destruição compatíveis com o original.
- Habilidades, câmera, menus, múltiplas fases, áudio e Memory Card.
- PAL/seleção de vídeo e medição de memória/desempenho no hardware real.

O cenário mantém o obstáculo estático até o impacto e remove o alvo por colisão
simples. A trajetória usa gravidade constante e três tentativas. Essas regras
são provisórias para validar o ciclo de jogo.
