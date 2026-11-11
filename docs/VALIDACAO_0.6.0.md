# Validação 0.6.0

- Compilação PS2 GCC/G++ 15.2, PS2SDK, gsKit e libmc: ELF e ISO gerados.
- Testes nativos de Box2D: 175 trajetórias vencedoras e 215 com destruição na varredura existente; derrota/reinício aprovados.
- Campanha: três vitórias, bloqueios/desbloqueios, recordes e 50 transições aprovados.
- Atlas: 53 sprites, limites, hashes, transparência e canais aprovados.
- Ferramentas/perfis: testes anteriores preservados e aprovados.
- Persistência: CRC32 padrão; alteração individual dos 96 bytes rejeitada; truncamentos rejeitados; sequência circular aprovada.
- Gerenciador com RPC simulado: operações com espera não bloqueante, descritor zero, cartão ausente/não formatado/cheio/trocado, recuperação pela cópia anterior, escrita parcial, preservação de ambas inválidas e pedido pendente aprovados.
- PCSX2 com ISO final: cartão não formatado detectado sem formatação automática; jogo e menu funcionando.
- Cartão de teste inicialmente todo FF, copiado antes de ser formatado pelo BIOS fornecido. Nenhum cartão de usuário foi usado.
- Vitória em 1-1, ângulo 30 e força exibida 293: 29.000 pontos; mensagem de progresso salvo no MC1.
- Encerramento e nova execução da ISO: recorde 29.000 e 1-2 desbloqueada carregados do cartão.
- L1 no menu: gravação manual concluída e confirmada por releitura.
- Browser do BIOS: save reconhecido como PS2Birds Classic, ícone visível; 40 KiB ocupados após duas cópias (7998 para 7958 KiB livres no cartão de teste).
- Sem teste em PS2 físico. Falhas de cartão foram injetadas nos testes simulados, não provocadas por remoção física durante escrita.
- O PCSX2 usou saída de áudio nula; o jogo ainda não implementa áudio.

## Formato e operação

Pasta `/PS2BIRDS` no cartão do slot 1, porta 0. Arquivos `icon.sys`, `save.icn`, `progress_a.dat` e `progress_b.dat`.
Cada registro tem 96 bytes little endian: assinatura (8), versão (4), campanha (4), sequência (4), quantidade de fases (4), desbloqueios (4), 16 recordes (64), CRC32 (4). Não são gravados ponteiros nem estruturas de memória do motor.
A cópia válida mais recente é lida; a próxima gravação usa a oposta. CRC e releitura detectam inconsistências, mas não garantem recuperação se ambas as cópias ou o sistema de arquivos do cartão forem danificados.
O ícone é regravado junto ao progresso. Nenhuma operação de formatação ou exclusão de diretório é executada pelo jogo.

- ELF extraído da ISO comparado byte a byte com o ELF distribuído: idêntico.
- Compilação limpa a partir do ZIP extraído, sem APK, downloads ou arquivos do diretório de desenvolvimento: aprovada.
