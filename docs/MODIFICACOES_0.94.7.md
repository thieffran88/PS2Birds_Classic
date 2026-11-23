# Modificações — 0.94.7

Versão da **conferência dos gatilhos das cenas**.

## Adicionado
- **Teste de campanha completa dos gatilhos** (`ps2/tests/scenes_test.cpp`, no `make test`): as 47 cenas tocam exatamente uma vez, nos lugares certos (14 aberturas e 33 finais em 745 fases), e todos os nomes existem no `CUTSCENE.BIN`.
- `ps2/src/scenes.c` e `scenes.h`: as tabelas e as decisões de qual cena toca (antes dentro do `main.c`), separadas do desenho para poderem ser testadas.
- `docs/MODIFICACOES_0.94.7.md`, `docs/VALIDACAO_0.94.7.md` e `docs/rio/MODIFICACOES_RIO_0.46.0.md`.

## Editado
- **Defeito encontrado e corrigido:** ao avançar com "Próxima" da última fase de um mundo ou episódio, a abertura da fase seguinte nunca tocava (só tocava ao entrar pelo menu). Agora a abertura toca depois do final do mundo (se houver) também por "Próxima".
- `ps2/src/main.c` (usa `scenes.c`), `ps2/Makefile`, `docs/CUTSCENES.md`, README e versão 0.94.7.

## Excluído
- Da pasta do Drive e do repositório: documentos, código-fonte, ISO, ELF e kits da 0.94.6 (substituídos) e a documentação 0.45.0 do Rio.

## Observações
- Conferido no emulador (ELF de teste, fora do repositório): terminar POACHED EGGS 3-21 e ir para a próxima toca o final do mundo 3 (rei porco machucado), depois a abertura de MIGHTY HOAX e então a fase 4-1.
- O que o teste não cobre: a tela de vitória ao vivo e o salvamento no Memory Card.
