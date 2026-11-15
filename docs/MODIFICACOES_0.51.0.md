# Modificações 0.51.0

- **Roteiros novos (fase por fase)**
  - O buscador de tiro único terminou a passada pelas fases novas com o motor atual (armas de bolhas incluídas): mais 24 fases ganharam roteiro vencedor no Flock Favorites, Golden Eggs, Tutorial e Mighty Feathers.
  - Fases sem roteiro: de 95 para 71.
- **Investigação da decompilação: sugador dos veículos (`EggDefenderGadget_Sucker.lua`, `RMFClassic_Sucker.lua`)**
  - Recuperados: alcance 35, cone de 55° para trás da peça, força 8 × (35 − distância) / 35 × massa (só acima de 10), estados `STARTING`/`SUCKING`; no modo clássico peças sugadas que encostam no sugador são destruídas e o pássaro sugado fica preso até usar a habilidade.
  - Não portado: o fator passado a `doSuction` no modo clássico se perdeu na decompilação; registrado em `LACUNAS_PORT.md`.
- **Testes**
  - `campaign_test`: tabela com os roteiros novos; suíte com 72 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.51.0; captura do menu (730 fases); documentos e captura da 0.50.0 substituídos.
  - Entregues ISO, ELF (com os .BIN), Gerador para Windows e Código-Fonte; os da 0.50.0 removidos da pasta de trabalho.
