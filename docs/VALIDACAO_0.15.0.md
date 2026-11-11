# Validação 0.15.0

- Base 0.14.0 conferida antes das alterações: ZIP do Drive extraído em pasta nova, toda a suíte aprovada e ELF recompilado com o ps2dev fornecido.
- Campanha de 42 fases concluída sequencialmente nos testes nativos, com bloqueios, contagens, desbloqueios, recordes, reinício, filas mistas, tempo decorrido a 30/60/120 Hz e 50 trocas de mundo. As 35 pontuações anteriores são idênticas às da 0.14.0.
- Novas fases: 2-15 Level66 93/5/5 (38000), 2-16 Level85 83/3/3 (35000), 2-17 Level27 58/3/3 (34000), 2-18 Level32 66/5/4 (36500), 2-19 Level72 55/2/3 (27500), 2-20 Level90 70/4/3 (44000) e 2-21 Level96 86/5/6 (50500). Roteiros determinísticos, não metas de estrelas originais.
- Roteiros: 10°/290, 10°/290, 15°/330, 10°/270, 68°/275 seguido de 66°/255 (pretos em arco com pavio automático), 20°/320 com habilidade a 0,4 s e 45°/230.
- TNT isolada: repouso estável por 5 s, explosão por impacto, segunda caixa 12 passos depois, madeira e porco eliminados, bloco fora do raio intacto, plataforma estática imóvel, duas explosões e nenhuma repetida, pontuação 6500, reinício, queda sem explosão e 20 trocas de mundo.
- Quatro caixas de TNT, um porco de bigode, três madeiras redondas e dezenove pilares/diamantes/cálices/bananas presentes nas fases novas com os tipos esperados.
- Atlas: 144 sprites, páginas 512×512, 512×512 e 512×256, 2.621.440 bytes; hashes, limites e ausência de sobreposição. Máscaras de terreno em meia resolução: 97×98, 321×17, 49×49, RGB branco e alfa até 128.
- Procedência: 21 fases da segunda página iguais ao trecho recuperado de episodes.lua; campos de explosão da TNT iguais à definição recuperada.
- Save: v3 preservado; expansão 35→42 com desbloqueio só após vitória na 2-14, recorde final gravado e recarregado; migrações v1/v2 e proteção contra saves futuros/inválidos mantidas.
- Áudio, preto, branco, azul, amarelo, Box2D e perfis: testes anteriores aprovados sem alteração.
- ELF e ISO compilados a partir de `make clean`; ELF dentro da ISO idêntico byte a byte ao distribuído; ZIP de fontes conferido por CRC, recompilado e testado em pasta nova.

Não houve execução no PCSX2 nesta rodada, pois o emulador e a BIOS da pasta Ferramentas não estavam disponíveis no ambiente de compilação. Ficam sem conferência visual: TNT, objetos novos e as máscaras de terreno em meia resolução. O uso de VRAM é o mesmo da 0.14.0, já validada no emulador. Sem teste em PS2 físico.
