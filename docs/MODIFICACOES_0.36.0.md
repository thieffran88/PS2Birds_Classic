# Modificações 0.36.0

- **Episódio Bad Piggies (páginas 32 a 34, 45 fases)**
  - Ordem do `episodes.lua` (Level743 a Level1247) e limites do `starLimits.lua`; bancos de texturas 28 a 30.
  - A campanha passa de 435 para 480 fases; 41 das 45 novas têm roteiro vencedor (faltam 2-7, 2-15, 3-5 e 3-12).
  - Ao carregar, nenhuma peça cai sozinha nas 45 fases.
- **6 peças de veículo pela `tools/add_definitions.py --material-kinds`**
  - Saco, óculos, galão de óleo, capacete de piloto, pneu de madeira (tipo madeira) e chave inglesa dourada.
  - As juntas do episódio (446 soldas, 191 dobradiças, uma com motor, uma prismática com motor, 10 "destruir anexo") já eram suportadas.
- **Testes**
  - `profile_test`: Bad Piggies (ordem das 45 fases, bancos 28-30, 6 peças contra o `blocks.lua`).
  - `sprite_test`: 31 bancos, 419 quadros.
  - Suíte com 58 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.36.0; README com 480 fases; captura do Bad Piggies 1-1; documentos e captura da 0.35.0 substituídos.
  - Drive: zips da 0.34.0 e documentos da 0.35.0 removidos.
