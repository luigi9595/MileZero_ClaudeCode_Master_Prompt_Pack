# MileZero Master Prompt Pack

Questo pack è pensato per Claude Code, non per una chat generica.
La logica è semplice:
- `CLAUDE.md` governa il comportamento persistente del progetto
- i file in `docs/` tengono stato, roadmap, rischi e verifiche
- `BOOTSTRAP_PROMPT.txt` fa partire la sessione
- `RESUME_PROMPT.txt` la riapre senza perdere il filo
- `ANTI_STALL_PROMPT.txt` lo rimette in carreggiata se comincia a filosofeggiare

## Uso consigliato
1. Apri o crea il repository del gioco.
2. Copia `CLAUDE.md` nella root del repo.
3. Copia la cartella `docs/` nel repo e lascia che Claude la aggiorni man mano.
4. Apri Claude Code nella root del progetto.
5. Incolla `BOOTSTRAP_PROMPT.txt` come primo messaggio.
6. Se chiudi e riapri la sessione, usa `RESUME_PROMPT.txt`.
7. Se Claude deraglia in teoria o design-only output, usa `ANTI_STALL_PROMPT.txt`.
8. Solo dopo che M2 è VERIFIED valuta i subagent opzionali nella cartella `optional_after_M2_verified/`.

## Filosofia del pack
Questo pack non spinge Claude a “immaginare” il gioco. Lo spinge a:
- ispezionare il workspace
- costruire una base UE5 reale
- verificare
- aggiornare lo stato
- avanzare a milestone con gate chiari

## Cosa non fare
- Non usare questo pack come “custom output style” principale.
- Non fargli creare subito 20 subagent.
- Non chiedergli approvazione a ogni passo.
- Non lasciarlo uscire dal perimetro di v1 prima di avere una build o un percorso di build credibile.

## Contenuto
- `CLAUDE.md`
- `BOOTSTRAP_PROMPT.txt`
- `RESUME_PROMPT.txt`
- `ANTI_STALL_PROMPT.txt`
- `docs/ROADMAP_EXECUTION.md`
- `docs/MILESTONE_STATUS.md`
- `docs/TEST_MATRIX.md`
- `docs/RISK_REGISTER.md`
- `docs/STATE.md`
- `docs/TODO.md`
- `docs/VERIFICATION.md`
- `docs/DECISIONS.md`
- `docs/BUILD_RUNBOOK.md`
- `docs/EDITOR_BOOTSTRAP.md`
- `docs/ASSET_MANIFEST.md`
- `optional_after_M2_verified/.claude/agents/*.md`

## Suggerimento pratico
All’inizio tieni Claude aggressivo sulla costruzione ma conservativo sulla forma:
- repository truth prima di tutto
- milestone e verification log sempre aggiornati
- niente output styles custom finché il progetto non è stabile
