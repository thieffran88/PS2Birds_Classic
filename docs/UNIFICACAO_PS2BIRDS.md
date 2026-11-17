# Unificação PS2Birds — 17/11/2026

## Organização adotada

- Pasta principal do Drive: PS2Birds, preservando o ID/link da antiga PS2Birds Classic.
- Classic/: APK Classic, ELF/ISO 0.92.0, kit Windows lançado e relatórios dessa versão.
- Rio/: APK Rio e relatórios da investigação.
- Raiz: pacote de fontes unificado, este registro e referências compartilhadas.
- Repositório informado: https://github.com/thieffran88/PS2Birds_Classic. A base local foi clonada do estado atual do repositório, incluindo a licença e as alterações de documentação existentes. O nome remoto permanece o escolhido pelo usuário.

## Uma base de fontes

- ps2/: runtime Classic 0.92.0 existente, que será a base compartilhada de execução.
- tools/: gerador existente e leitores/decodificadores comuns.
- tools/rio/: adaptadores e auditoria Rio, consumindo os leitores comuns em tools/; sem outra cópia vendor/classic092.
- profiles/projects.json: identidade e capacidade por jogo, com os hashes dos APKs suportados.
- docs/rio/: relatórios da investigação anterior, preservados como histórico.
- work/rio/analysis/: dados locais regenerados; fora do Git e do ZIP de fontes.

O ponto de entrada continua tools/gerar_iso.py. Classic mantém o fluxo de geração existente. Rio é reconhecido pelo hash e pode ser investigado com --jogo rio --investigar. A geração de ISO Rio permanece bloqueada até integrar suas metas/objetos/personagens ao runtime. O kit Windows Classic 0.92.0 já lançado não foi recompilado nesta reorganização; a integração atual está nas fontes do gerador.

## Alterações em tópicos

### Adicionado

- Subpastas Classic e Rio dentro de PS2Birds no Drive.
- profiles/projects.json com perfis Classic/Rio, hashes e capacidade atual de cada pipeline.
- tools/rio/__init__.py e seis ferramentas Rio: investigate_rio, inspect_levels, inspect_dat, inspect_comp, build_catalogue e audit_gaps.
- docs/rio/ANALISE_PORT_RIO.md e docs/rio/INVESTIGACAO_0.1.0.md, reunindo a documentação Rio no repositório comum.
- docs/UNIFICACAO_PS2BIRDS.md e pacote PS2Birds_Codigo_Fonte_Unificado_2026-11-17.zip.

### Editado

- Pasta PS2Birds Classic renomeada para PS2Birds, mantendo seu ID/link.
- Pasta PS2Birds Rio movida para dentro de PS2Birds e renomeada para Rio.
- Arquivos de distribuição Classic, APK Classic e relatórios da 0.92.0 movidos para Classic/, preservando seus IDs.
- tools/gerar_iso.py: opções --jogo auto/classic/rio e --investigar; detecção de APK por SHA; rota Rio para auditoria; recusa explícita de gerar ISO Rio enquanto o runtime não está integrado. Funções de geração Classic preservadas.
- Ferramentas Rio: caminhos relativos à raiz unificada, saída em work/rio/analysis, uso dos leitores do projeto principal e hash obtido do perfil comum.
- README.md: apresentação PS2Birds, comandos por perfil, estado de suporte e localização dos dados locais.
- .gitignore: exclusão de input/, work/, dist/, APKs e executáveis/imagens gerados.
- Comentário do requisito Pillow para cobrir PNG/WebP.

### Excluído/substituído na consolidação

- ZIPs separados de fontes Classic 0.92.0 e investigação Rio 0.1.0 substituídos pelo pacote unificado, depois da validação e conferência do novo arquivo no Drive.
- Segunda cópia redundante do APK Rio na antiga raiz Classic removida após conferência de SHA-256. Preservado o APK Rio original na subpasta Rio.
- Duplicação dos cinco leitores e dos perfis em vendor/classic092 eliminada na base unificada; as ferramentas Rio passam a usar os arquivos comuns existentes.
- APKs, fases/texturas/sons extraídos, SDK, BIOS, saídas de auditoria e diretório .git ficam fora do pacote de fontes. Os APKs de entrada continuam nas pastas de jogo no Drive.

## Verificação

- Clone do repositório acessível e base 0.92.0 conferida.
- Código C/C++ do runtime e os cinco leitores comuns não sofreram alteração nesta reorganização.
- Validação do hash Classic passou; sua função de geração permaneceu preservada.
- Auditoria Rio passou pelo mesmo ponto de entrada do gerador: 649 chunks, 459 fases, 44 tabelas auxiliares e os cruzamentos previamente documentados.
- Testada a rota automática com APK Rio sem --investigar: recusa antes de criar ISO, evitando apresentar uma ISO Classic como Rio.
- Pacote de fontes sem APK, ELF, ISO, saídas work/ ou histórico .git; integridade ZIP e compilação sintática Python conferidas.
- Não foram reconstruídos ELF/ISO ou o kit Windows; não houve novo teste de runtime no PS2 nesta reorganização.
- Preparação local e pacote no Drive; nenhum commit/push ou alteração remota foi realizado no GitHub.
