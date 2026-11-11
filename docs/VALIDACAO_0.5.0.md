# Validação 0.5.0 — 11/11/2026

- Dados reais recuperados do APK Classic: Level1.lua, Level2.lua e Level3.lua.
- Contagens: 1-1 = 19 objetos/3 aves/1 porco; 1-2 = 24 objetos/4 aves/2 porcos; 1-3 = 20 objetos/4 aves/1 porco.
- Primeiro cenário preserva as regressões anteriores: 175 tiros vencedores e 215 com destruição entre 567 combinações.
- Teste de campanha completa as três fases (28.000/23.000/18.000 pontos), verifica desbloqueios, rejeita acesso a fases bloqueadas/índices inválidos e preserva recordes da sessão.
- Verificados bônus único, reinício e 50 transições com destruição/recriação dos mundos físicos.
- Atlas: 53 sprites, 512×512, RGBA32/alfa GS, 1.048.576 bytes. Hashes, limites, ausência de sobreposição e canais verificados.
- Testes próprios C++ compilados com avisos tratados como erro; cabeçalho gsInline.h de terceiros ainda emite aviso de parâmetro não usado na compilação PS2.
- PCSX2: menu de bloqueios inspecionado, fase 1-1 concluída e avanço para 1-2; fase 1-2 concluída com 23.000 pontos, liberando a cena 1-3.
- Gerador de campanha e atlas de 53 sprites reproduzem os arquivos byte a byte.
- Os testes das ferramentas existentes continuam passando; não houve trabalho de port de outros jogos.

Roteiros nativos reproduzíveis: iniciar e aguardar 2 s; 1-1 usa ângulo 30/força 291;
1-2 usa até quatro tiros de ângulo 20/força 360; 1-3 até quatro tiros de ângulo 15/
força 280. Os resultados dependem do instante dos impactos e do assentamento; os
valores são roteiros de teste, não garantia de pontuação idêntica em toda partida.

O progresso é volátil e não é salvo no Memory Card. Máscaras do terreno usam
coloração provisória. Colisões/dano não reproduzem integralmente o motor original.
Não houve teste em PS2 físico. O ambiente PCSX2 usa saída de áudio nula.
