# Modificações — 0.94.6

Versão de **correção da correção do som**: a 0.94.5 deu tela preta com som travado no console do jogador.

## Editado
- **Som sem chamar `audsrv_stop_audio`** (`ps2/src/music.c`): a 0.94.5 esvaziava o anel de áudio com essa chamada ao parar uma música; no console a chamada trava (tela preta e som preso, como o defeito anterior). Agora o anel é preenchido com silêncio (só `audsrv_play_audio`), de modo que o que ele repete ao esvaziar é silêncio. `audsrv_stop_audio` só continua sendo chamada ao desligar o som no menu, como antes. O teste da música cobre o preenchimento com silêncio, a música que termina sozinha e a ausência da chamada.
- Versão 0.94.6 e README.

## Excluído
- Da pasta do Drive e do repositório: documentos, código-fonte, ISO, ELF e kits da 0.94.5 (substituídos) e a documentação 0.44.0 do Rio.

## Observações
- A tela preta não apareceu nas duas ISOs da 0.94.5 no emulador (abrem no menu); a hipótese da trava em `audsrv_stop_audio` vem de a chamada ser a única mudança de áudio da versão e de os sintomas (tela preta, som constante) serem os de uma chamada que não volta. Sem o console aqui, não consegui provar.
- Se a tela preta continuar, preciso saber: qual ISO (Classic ou Rio), em que momento (ao ligar, ao abrir a fase) e se usa ISO ou a pasta `apps`.
