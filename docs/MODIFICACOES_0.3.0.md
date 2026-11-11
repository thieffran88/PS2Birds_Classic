# Modificações completas — 0.3.0 em relação à 0.2.0

- Adicionado extrator de sprites originais do APK, sem executar código ou scripts do jogo.
- Decodificados os metadados gráficos AES/LZMA e os streams de pixels 7z.
- Interpretados cabeçalhos de 40 bytes e pixels RGBA4444 little-endian.
- Validados formato, dimensões, orientação, comprimento e limites dos registros antes da conversão.
- Removida a borda de extrusão de um pixel, preservando recorte e pivô de cada sprite.
- Convertidas cores para RGBA32 e transparência para a faixa 0–128 do GS.
- Recuperados 23 sprites originais: RedBird; porco normal, piscando e ferido; frente e fundo do estilingue; plataforma; quatro estados para cada um dos três tamanhos de madeira e do bloco de vidro.
- Substituídos os desenhos procedurais desses objetos pelos recursos originais. O fundo e a interface continuam procedurais.
- Implementado empacotamento determinístico por skyline com margens transparentes entre sprites.
- Criado atlas único 512×256, com uso de 512 KiB de VRAM.
- Incluídos atlas binário, cabeçalho de regiões e manifesto com origem, pivôs, dimensões e hashes no código-fonte.
- Incorporado o atlas ao ELF com alinhamento de 128 bytes, dispensando arquivos gráficos externos.
- Adicionado módulo de renderização de sprites via gsKit e envio único da textura à VRAM na inicialização.
- Implementada verificação de falha de alocação da textura na VRAM.
- Ativada composição por transparência no GS e filtragem nearest para evitar mistura entre regiões.
- Implementados quads texturizados por listas de vértices, com rotação acompanhando os corpos do Box2D.
- Preservados pivôs e escala visual original de 20 pixels por unidade física.
- Selecionados os tamanhos e materiais de sprites correspondentes a cada bloco da fase.
- Ligados os quatro estados visuais dos blocos à resistência restante, com faixas de 75%, 50% e 25%.
- Adicionados piscar periódico e sprite ferido do porco; a animação de piscar para ao pausar.
- Implementada ordem de desenho da frente/fundo do estilingue em relação ao pássaro.
- Mantidos dados da fase, física, pontuação e controles da versão 0.2.0.
- Atualizado o Makefile para compilar o renderizador e incorporar o atlas, com dependências dos arquivos gerados.
- Adicionados testes de ordem de canais/endian, transparência GS, recorte, entrada truncada, limites, não sobreposição e hashes de cada sprite.
- Confirmados novamente os testes de física: 175 tiros vencedores e 215 tiros com destruição no conjunto de 567 combinações.
- Verificados boot do ELF/ISO e funcionamento da cena texturizada no PCSX2.
- Atualizados identificação para 0.3.0, instruções de reprodução da extração, documentação de validação e pacote de fonte.
- Gerados novos ELF, ISO e código-fonte; substituição dos três artefatos 0.2.0 após verificação dos uploads.

Ainda pendentes: fundo original, áudio, outras fases, fidelidade do dano e das formas
de colisão, execução integral de Lua, menus, salvamento e teste no PS2 físico.
