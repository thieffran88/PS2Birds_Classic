.section .rodata
.balign 64
.global audsrv_irx
audsrv_irx:
.incbin "build/audsrv.irx"
audsrv_irx_end:
.balign 4
.global audsrv_irx_size
audsrv_irx_size:
.word audsrv_irx_end-audsrv_irx
.balign 64
.global audio_bank
audio_bank:
.incbin "assets/sounds.adp"
