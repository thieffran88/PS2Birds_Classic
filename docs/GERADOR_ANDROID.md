# Gerar a ISO pelo celular (Android)

Para quem **não tem computador**. O gerador roda no próprio celular ou tablet Android, dentro do **Termux**, um aplicativo gratuito de terminal. Você não precisa saber programar: são poucos comandos, e eles estão prontos abaixo para copiar.

> **Aviso importante:** este pacote **não traz nada do jogo original**. Você precisa do arquivo do jogo (o APK) obtido por conta própria, de forma legítima. O gerador só converte o seu APK, no seu celular.
>
> **Estado do teste:** o script foi testado de ponta a ponta no Linux, num ambiente que imita o Termux, mas **ainda não foi testado num celular real**. Se algo falhar, veja "Se der erro" no fim e envie o arquivo de registro.

## O que você precisa

- Celular ou tablet com **Android 7 ou mais novo**.
- Uns **2 GB de memória livre** e **1,5 GB de espaço livre** no armazenamento.
- **Internet**, só na primeira vez (o Termux baixa o Python e o ffmpeg).
- O **APK original do Angry Birds Classic**, com o nome `com.rovio.angrybirds.apk`.
- O pacote do gerador para Android: `PS2Birds_Classic_<versão>_Gerador_Android.zip` (anexo das Releases).

O **Classic** gera a ISO completa. O **Angry Birds Rio** (opção 2) é experimental: gera uma ISO com o primeiro episódio e parte da selva, ainda sem menus próprios; use o APK `com.rovio.angrybirdsrio.apk`. O resultado do Rio vai para a pasta `apps/PS2BirdsRio`.

## Passo a passo

### 1. Instale o Termux

Instale pelo **F-Droid**: <https://f-droid.org/packages/com.termux/>

Não use a versão da Play Store: ela está desatualizada e não funciona para isto.

### 2. Coloque os arquivos na pasta Download

Copie para a pasta **Download** do celular:

- o APK `com.rovio.angrybirds.apk`;
- o ZIP `PS2Birds_Classic_<versão>_Gerador_Android.zip`.

### 3. Abra o Termux e digite os comandos

Digite **um comando por linha** e toque em Enter depois de cada um.

```sh
termux-setup-storage
```

O Android vai perguntar se o Termux pode acessar seus arquivos: toque em **Permitir**.

```sh
cd ~/storage/downloads
unzip PS2Birds_Classic_*_Gerador_Android.zip
bash PS2Birds_Gerador/GERAR_ISO_ANDROID.sh
```

### 4. Espere a primeira preparação

Na primeira vez, o script instala o Python e o ffmpeg. Isso leva alguns minutos e usa a internet. Nas próximas vezes ele pula esta parte.

Depois ele pergunta qual jogo você quer. **Digite `1`** (Angry Birds Classic) e toque em Enter.

### 5. Acompanhe a geração

Uma **página de acompanhamento** abre no navegador do celular. Ela mostra:

- a barra com as nove etapas e quais já terminaram;
- o tempo total e o tempo da etapa atual;
- o "último sinal de vida", para você saber se o programa está trabalhando ou parou;
- o registro detalhado, linha por linha.

Se a página não abrir sozinha, o endereço aparece no Termux (algo como `http://127.0.0.1:12345/`): abra-o no navegador.

A geração costuma levar de **15 a 40 minutos** no celular (esta é uma estimativa; depende do aparelho). **Mantenha o Termux aberto.** O script já pede ao Android para não dormir durante a geração.

### 6. Pegue o resultado

No fim, os arquivos ficam na pasta **PS2Birds** do armazenamento do celular. Abra o aplicativo **Arquivos** e procure por ela:

| O que | Para que serve |
| --- | --- |
| `SLUS_000.10.PS2Birds_Classic.iso` | A ISO, para emulador (PCSX2), OPL e outros. O começo do nome é o ID do disco: **não troque**. |
| `apps/PS2Birds/` | O programa (ELF) e os arquivos de dados, para pendrive ou HDD do PlayStation 2. Copie a pasta `apps` inteira para a raiz do pendrive. |

Para o **OPL**, copie a ISO para a pasta `DVD` do pendrive ou do HDD, mantendo o nome. A pasta `apps/PS2Birds` traz um `title.cfg`, usado pelo OPL para listá-la no menu de aplicativos.

> Não testamos no OPL de verdade. Se ele não listar o jogo, conte o que aparece.

## Se der erro

| Mensagem | O que fazer |
| --- | --- |
| "Não achei o APK" | Coloque o `com.rovio.angrybirds.apk` na pasta Download e rode o script de novo. |
| "Pouco espaço livre" | Libere uns 1,5 GB no celular. |
| "Não consegui instalar" | Confira a internet e rode de novo. |
| Qualquer outro erro | Envie o arquivo `GERAR_ISO_LOG.txt` (fica na pasta `PS2Birds`) numa Issue do repositório. |

Se o celular tiver pouca memória e a geração for encerrada sozinha, o script já usa só 2 processos. Num celular potente você pode tentar mais:

```sh
PS2BIRDS_PROCESSOS=4 bash PS2Birds_Gerador/GERAR_ISO_ANDROID.sh
```

## Perguntas rápidas

**Preciso de root?** Não. Nada é instalado no Android além do Termux.

**Posso apagar o Termux depois?** Sim. A ISO e a pasta ficam na memória do celular, fora do Termux.

**Funciona em tablet?** Sim, o procedimento é o mesmo.

**Por que não é um aplicativo normal?** O gerador é feito em Python, e o jeito mais simples e seguro de rodá-lo no Android é pelo Termux. Um aplicativo próprio exigiria ferramentas que ainda não foram testadas aqui.
