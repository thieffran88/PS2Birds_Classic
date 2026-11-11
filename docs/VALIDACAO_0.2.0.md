# Validação 0.2.0 — 11/11/2026

- Compilação PS2 concluída com GCC/G++ 15.2.0, PS2SDK e gsKit fornecidos.
- Núcleo novo e testes nativos compilados com -Wall -Wextra -Werror.
- Box2D 2.4.1 original incluído: sua compilação PS2 apresenta três avisos conhecidos de variáveis não usadas/memset em código de terceiros; nenhum erro de link.
- Testes confirmaram 19 objetos, 14 blocos, 1 porco e 3 pássaros, posições importadas e rejeição de índices inválidos.
- Verificados assentamento sem pontuação automática, limites de mira, trajetória em voo livre, bloqueio de lançamento duplicado, vitória, derrota após três tentativas e reinício/liberação de recursos.
- Em 567 combinações testadas, 175 venceram e 215 destruíram blocos.
- Boot do ELF e da ISO confirmado no PCSX2 v2.9.148 com renderizador Software.
- Teste interativo pela ISO: ângulo 30, força 291; estrutura atingida, blocos destruídos, porco eliminado, vitória com 28.000 pontos.
- Capturas de tela da cena inicial e do resultado incluídas no pacote de fonte.

Limites: os tamanhos dos blocos usam extensões gráficas dos atlas e não as formas
nativas exatas; dano e contato são aproximações novas. O chão é uma caixa ampla
com topo na altura 5 da fase. O raio físico do pássaro é provisório. Arte procedural,
sem áudio e sem salvamento. Sem teste em console físico. O ambiente do emulador
não tem dispositivo de áudio funcional e utiliza saída nula após o aviso.
