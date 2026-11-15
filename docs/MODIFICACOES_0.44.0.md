# Modificações 0.44.0

- **Frascos do Short Fuse (novo no jogo, `VialEffects.lua`)**
  - Ao quebrar, o frasco age sobre os porcos dentro do raio do `vialEffect` (`blocks.lua`), 1,3 s depois.
  - Laranja (raio 6): o porco incha como o GlobeBird e estoura 3 s depois (`puffTime` 1,5 + `deathTime` 1,5).
  - Rosa (raio 7): o porco fica numa bolha que flutua como as da Stella e ignora dano enquanto dura (`transformIntoBubble`, `ignoreAllDamage`).
  - Azul: registrado no `CAMPAIGN.BIN`, mas ainda sem efeito (ele clona os porcos próximos; ver lacunas).
  - 44 fases têm frascos; todos os roteiros continuam vencendo.
  - `CAMPAIGN.BIN` versão 5: a tabela por definição ganhou o tipo e o raio do frasco (40 bytes por registro).
- **Roteiro novo**
  - Dino 3-15 (`Dino3_tntbomb`) passa a ter roteiro vencedor.
- **Testes**
  - `globe_test`: quebrar um frasco laranja estoura o porco ao lado; quebrar um rosa faz o porco subir.
  - `profile_test`: tipo e raio dos três frascos no `CAMPAIGN.BIN` conferidos com o `blocks.lua`.
  - Fixtures de teste no formato novo.
  - Suíte com 70 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.44.0; README descreve os frascos; captura do Short Fuse 1-4; documentos e captura da 0.43.0 substituídos; `LACUNAS_PORT.md` com os achados da 0.44.0.
  - Entregues ISO, ELF (com os .BIN), Gerador para Windows e Código-Fonte; os da 0.43.0 removidos da pasta de trabalho.
