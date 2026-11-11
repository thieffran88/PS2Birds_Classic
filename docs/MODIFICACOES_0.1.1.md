# Modificações 0.1.1

- Adicionado suporte ao analógico esquerdo para ângulo e força.
- Implementada zona morta de 24 unidades para reduzir deriva do analógico.
- Adicionada solicitação de modo DualShock, sem esperar em laços bloqueantes.
- Implementada nova solicitação de modo analógico após desconexão.
- Mantida compatibilidade com controle digital, sem ler eixos ausentes.
- Definida prioridade do direcional quando usado junto com o analógico.
- Adicionado modo de precisão ao segurar Quadrado: 25% da velocidade de ajuste.
- Ajustado o passo de simulação para 59,94 Hz do modo NTSC utilizado.
- Atualizadas instruções na tela para analógico e precisão.
- Atualizada identificação do executável para 0.1.1.
- Gerados novo ELF, ISO e pacote completo de código-fonte.

As fases originais continuam pendentes; esta entrega aprimora a base técnica.
Compilação e testes de simulação verificados. O novo suporte analógico ainda
precisa de teste com controle físico. Não houve teste em console nesta etapa.
