#!/bin/sh
set -eu
: "${PS2DEV:?Defina PS2DEV para a pasta ps2dev extraida do pacote de ferramentas}"
export PS2SDK="${PS2SDK:-$PS2DEV/ps2sdk}"
export GSKIT="${GSKIT:-$PS2DEV/gsKit}"
export PATH="$PS2DEV/ee/bin:$PS2DEV/iop/bin:$PS2DEV/dvp/bin:$PS2SDK/bin:$PATH"
cd "$(dirname "$0")"
exec make "$@"
