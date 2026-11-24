# Modificações — 0.94.24 (Classic) e 0.49.14 (Rio)

## Investigado (Rio: gravidade)
- Nenhum script do Rio define a gravidade: as fases não têm `worldGravity` (só `physicsToWorld 20`, nas 459) e o `gamelogic.lua` do Rio não chama `setWorldGravity` (o do Classic chama, com o `worldGravity` da fase, em `AdventureMode`). Sobra o valor padrão do código nativo.
- **Lido no código nativo** (`lib/x86/libAngryBirdsRio.so`): os dois lugares que criam o mundo do Box2D (`operator new(0x19258)`, o tamanho do `b2World`, em `0x7586c` e `0x886f5`) passam ao construtor o vetor de gravidade **(0, +20)** (`mov [esp+..],0x0` e `0x41a00000`) e `doSleep = 1`. É a gravidade que o port já usa (+20, a mesma do Classic): **confirmada**.

## Adicionado
- `docs/MODIFICACOES_0.94.24.md` e `docs/rio/MODIFICACOES_RIO_0.49.14.md`.

## Editado
- `docs/LACUNAS_PORT.md`. O jogo não mudou.

## Excluído
- Notas da versão anterior.
