# audsrv

EE/IOP audio implementation from https://github.com/ps2dev/ps2sdk,
retrieved 2026-11-11. Library sources retain their GNU Library GPL v2 notices;
shared import headers retain their Academic Free License v2 notices. Both license
texts are included. The optional EE IOP-reboot detection call is omitted for compatibility with
the supplied older SDK; this application initializes once and never resets IOP.
The IOP Makefile is adapted to the installed PS2SDK prefix.
The port builds its embedded IRX and EE RPC object from these included sources;
no prebuilt audsrv module is required. Rebuilding the main source recompiles and
relinks the library. A complete SDK is still needed for system headers/toolchain.
