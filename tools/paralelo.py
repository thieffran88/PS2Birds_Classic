"""Quantos processos paralelos o gerador usa. Em computador: um por núcleo (no máximo 6).  A variável de ambiente PS2BIRDS_PROCESSOS escolhe outro número (1 = sem paralelo)."""
import os
def processos(tarefas):
    env=os.environ.get('PS2BIRDS_PROCESSOS')
    if env and env.isdigit() and int(env)>=1:n=int(env)
    else:n=min(6,os.cpu_count() or 1)
    return max(1,min(n,tarefas))
