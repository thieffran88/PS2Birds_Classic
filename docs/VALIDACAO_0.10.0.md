# Validação 0.10.0

- Base: código-fonte 0.9.0 fornecido no Drive, preservado e recompilado com o SDK disponível nesta sessão.
- Ordem oficial das 21 fases conferida por nova recuperação/decompilação de episodes.lua, sem execução de Lua; coincide com o perfil da 0.9.0.
- Compilação EE/IOP: GCC/G++ 15.2, PS2SDK, gsKit, libpad, libmc, libpatches e audsrv compilado das fontes incluídas.
- Campanha: 21 vitórias, contagens, bloqueios, recordes, reinício, regressão de tiros que erram o cenário e 50 transições aprovados.
- Resultados das 21 fases: 27000/41500/30500/7000/56000/15000/44500/22000/23500/16000/26000/45500/49500/65500/33500/51000/32500/29000/14000/38500/19000, idênticos à base 0.9.0.
- Azul e amarelo: testes anteriores de habilidade, fila, colisão, direção/velocidade e 30 transições aprovados.
- Gráficos: 109 sprites e três páginas RGBA32, 2359296 bytes dentro do orçamento 2621440; hashes, canais, alfa, limites e sobreposição aprovados.
- Áudio: 16 efeitos, 330688 bytes, mono 22050 Hz; manifesto, hashes, alinhamento, cabeçalhos APCM, ausência de loop e decodificação independente com sinal não silencioso aprovados.
- Backend de áudio simulado: falhas de módulo/RPC não impedem a partida; carga única, índices inválidos, limites de frequência, R3 e canal ocupado aprovados.
- Eventos de destruição conferidos contra as mudanças reais dos contadores em todos os tiros do teste de campanha; eventos não se repetem após vitória.
- Saves: layouts v1/v2 e migrações anteriores preservados. Novas regressões protegem um formato futuro em qualquer slot quando o outro contém save antigo válido, e um v1 com quantidade de fases sem mapeamento conhecido.
- PCSX2: ISO final inicia, aloca as três páginas com framebuffer de 16 bits e dithering, carrega LIBSD/audsrv e mostra SOM LIGADO.
- PCSX2: navegação, confirmação, início e lançamento capturados com faixa AAC derivada do áudio interno do SPU2. A saída física do ambiente não estava disponível e foi substituída pelo modo nulo do emulador; a medição foi feita na captura interna, não no dispositivo de saída.
- PCSX2: R3 muda o indicador para SOM DESLIGADO; a mesma sequência capturada nesse modo fica silenciosa.
- PCSX2: leitura de save legado e gravação manual v2 com mensagem de progresso salvo aprovadas.

- PCSX2 após gravar e reiniciar: save v2 carregado, recordes remapeados visíveis em 1-4/1-5/1-6 e som novamente ligado por padrão da sessão.

- Helper Python de arquivos executado com sucesso sem permissão de execução e com espaços no caminho; mantido suporte ao 7z nativo.

Sem teste em PS2 físico. Música de fundo, mixagem refinada, demais páginas/episódios, pássaros adicionais e maior fidelidade do motor continuam pendentes. As capturas são apenas evidência local e não fazem parte dos artefatos do Drive.

## Medições de áudio

- Som ligado: 2162688 amostras a 48000 Hz, pico 16619, RMS 689.11, 476381 amostras não nulas.
- Som desligado: 3491840 amostras a 48000 Hz, pico 0, RMS 0.0.

- Compilação final no PCSX2: 1-1 vencida com 27000 pontos (10°/351,5), gravação automática concluída e R3 silenciando o áudio.
- Captura final: pico 15524, RMS 517,55, últimos 5 s com pico zero após R3; canais separados para início/vitória/derrota verificados.
- Teste adicional rejeita módulo IOP que não fica residente antes de tentar conectar ao RPC.

- Compilação limpa sequencial a partir do ZIP extraído: EE, módulo IOP e todos os testes aprovados; sem APK, downloads ou arquivos de build do diretório original.
- ELF extraído da ISO idêntico byte a byte ao ELF distribuído; todas as entradas do ZIP passaram na verificação de integridade.
