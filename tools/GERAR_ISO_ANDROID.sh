#!/data/data/com.termux/files/usr/bin/bash
# PS2Birds - Gerador de ISO para Android (roda dentro do Termux). Veja o LEIA-ME_ANDROID.txt.
QUIET=0
DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$DIR" || exit 1
echo
echo "============================================"
echo "  PS2Birds - Gerador de ISO (Android)"
echo "============================================"
echo

# ---- 1. Preparo (só na primeira vez; precisa de internet)
if command -v pkg >/dev/null 2>&1; then
  if ! command -v python >/dev/null 2>&1 || ! command -v ffmpeg >/dev/null 2>&1; then
    echo "Instalando Python e ffmpeg (só na primeira vez, pode levar alguns minutos)..."
    pkg update -y && pkg install -y python ffmpeg || { echo "Não consegui instalar. Verifique a internet e tente de novo."; exit 1; }
  fi
  if ! python -c "import pycdlib" 2>/dev/null; then
    echo "Instalando o gerador de ISO (pycdlib)..."
    pip install pycdlib || { echo "Não consegui instalar o pycdlib. Verifique a internet e tente de novo."; exit 1; }
  fi
  if [ ! -d "$HOME/storage/shared" ]; then
    echo
    echo "O Android vai pedir permissão para o Termux acessar seus arquivos. Toque em PERMITIR."
    termux-setup-storage
    for i in $(seq 1 30); do [ -d "$HOME/storage/shared" ] && break; sleep 1; done
  fi
fi
command -v python >/dev/null 2>&1 || { echo "Python não encontrado."; exit 1; }

# ---- 2. Qual jogo?
echo
echo "Qual jogo você quer gerar?"
echo "  [1] Angry Birds Classic"
echo "  [2] Angry Birds Rio  (em desenvolvimento: ainda não gera ISO)"
read -r -p "Digite 1 ou 2: " OP
if [ "$OP" = "2" ]; then JOGO=rio; else JOGO=classic; fi
if [ "$JOGO" = "rio" ]; then echo; echo "O Angry Birds Rio ainda está em desenvolvimento e não gera ISO. Escolha o Classic."; exit 1; fi

# ---- 3. Onde está o APK?
APK="$1"
if [ -z "$APK" ]; then
  for d in "$PWD" "$HOME/storage/downloads" "$HOME/storage/shared/Download" "$HOME/storage/shared"; do
    [ -f "$d/com.rovio.angrybirds.apk" ] && APK="$d/com.rovio.angrybirds.apk" && break
  done
fi
if [ -z "$APK" ]; then
  APK="$(find "$HOME/storage/downloads" "$HOME/storage/shared/Download" "$PWD" -maxdepth 2 -iname '*angrybirds*.apk' 2>/dev/null | grep -vi rio | head -1)"
fi
if [ -z "$APK" ] || [ ! -f "$APK" ]; then
  echo
  echo "Não achei o APK do Angry Birds Classic (com.rovio.angrybirds.apk)."
  echo "Copie-o para a pasta Download do celular e rode este script de novo."
  exit 1
fi
echo
echo "APK: $APK"

# ---- 4. Onde gravar o resultado?
if [ -d "$HOME/storage/shared" ]; then OUT="$HOME/storage/shared/PS2Birds"; else OUT="$DIR/saida"; fi
mkdir -p "$OUT" || exit 1
FREE_KB=$(df -k "$OUT" 2>/dev/null | awk 'NR==2{print $4}')
if [ -n "$FREE_KB" ] && [ "$FREE_KB" -lt 1500000 ]; then
  echo "Pouco espaço livre no celular (precisa de uns 1,5 GB livres). Libere espaço e rode de novo."
  exit 1
fi
echo "O resultado vai para: $OUT"

# ---- 5. Gerar
export PS2BIRDS_PROCESSOS="${PS2BIRDS_PROCESSOS:-2}"
command -v termux-wake-lock >/dev/null 2>&1 && termux-wake-lock   # impede o celular de dormir durante a geração
echo
echo "Gerando. Uma página de acompanhamento vai abrir no navegador do celular."
echo "Pode levar de 15 a 40 minutos no celular. Não feche o Termux."
echo
cd "$OUT" || exit 1
python "$DIR/tools/gerar_iso.py" "$APK" --jogo "$JOGO" --painel --pasta apps/PS2Birds
STATUS=$?
command -v termux-wake-unlock >/dev/null 2>&1 && termux-wake-unlock
echo
if [ $STATUS -eq 0 ]; then
  echo "Pronto! Arquivos em: $OUT"
  echo "  - a ISO (nome começa com SLUS_000.10)"
  echo "  - a pasta apps/PS2Birds (ELF + dados)"
  echo "Abra o app de Arquivos do Android, pasta PS2Birds, para copiar para o pendrive ou o cartão."
else
  echo "A geração terminou com erro. Veja o arquivo GERAR_ISO_LOG.txt em $OUT e me envie."
fi
exit $STATUS
