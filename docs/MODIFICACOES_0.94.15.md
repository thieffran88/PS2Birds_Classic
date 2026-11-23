# Modificações — 0.94.15 (Classic) e 0.49.5 (Rio)

## Editado
- **Fases sem roteiro de vitória: de 62 para 43.** Uma busca nova por sequências de pássaros (`ps2/tests/solver4.cpp`) achou roteiro de vitória para 19 fases: Birdday, Short Fuse, Flock Favorites, Piggy Farm, Bad Piggies, Dino, Danger Above, Bird Island, Surf and Turf e o Tutorial 9. A tabela de `ps2/tests/campaign_test.cpp` foi atualizada (o teste confere que cada roteiro vence).
- `ps2/tests/AUXILIARES.md` documenta o `solver4`.
- Só o programa de testes mudou; o jogo é o mesmo da 0.94.14.

## Adicionado
- `ps2/tests/solver4.cpp`, `docs/MODIFICACOES_0.94.15.md` e `docs/rio/MODIFICACOES_RIO_0.49.5.md`.

## Excluído
- Notas da versão anterior.

## Sem roteiro (43)
- Golden Eggs 11 (fases de enigma: 1 pássaro, TNT em cadeia, 72 porcos), Short Fuse 5, Flock Favorites 4, Bad Piggies 3, Piggy Farm 3, Dino 3, Tutorial 3 (Bird Quake, Shockwave e Pucombo: pedem usar o power-up no roteiro, que a tabela ainda não registra), Mine and Dine 2, Birdday 2, Surf and Turf 2, Egg Defender 2, Danger Above 1, Bird Island 1, Mighty Feathers 1. A busca simples não achou vitória; isso não prova que sejam impossíveis.
