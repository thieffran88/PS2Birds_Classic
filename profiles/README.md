# Perfis e adaptadores

- games.json: chaves declaradas no projeto de referência e limites da evidência.
- REFERENCE_DECOMPILATION_README.md: referência fornecida que declara os seis jogos.
- classic_level.json: adaptador real da fase Classic atualmente integrada.

Um perfil criptográfico não é um port. Um adaptador de fase também não fornece
habilidades ou APIs nativas. Não criamos adaptadores fictícios para os outros jogos.
Leia docs/COMPATIBILIDADE_ANGRY_BIRDS.md antes de registrar um novo adaptador.

Para objetos equivalentes já suportados, um adaptador JSON pode mapear nomes para
kinds 1 (madeira), 2 (vidro), 3 (plataforma) 4 (porco circular) e 5 (pedra). Nomes de pássaros
são aliases do comportamento RedBird atual; não implementam habilidades novas.
metrics aponta para dimensões verificadas dos sprites. O desenho ainda usa IDs do
Classic, portanto novos materiais precisam também de um adaptador gráfico.
