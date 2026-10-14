# Nuvio Legacy Custom per LG webOS (prima versione sperimentale)

Questo ramo crea una **seconda applicazione** per LG webOS, indipendente da
`space.nuvio.native.legacy` (l'installazione originale dal Homebrew Channel).

- **ID nuovo:** `space.nuvio.native.legacy.custom`
- **Nome:** Nuvio Legacy Custom
- **Cataloghi StreamViX:** la Guida TV richiede `genre=...` per TV, Live
  ed Eventi; mantiene gli ID originali dei canali e il percorso di riproduzione.
- **Ricerca:** i cataloghi `*_search` non vengono mostrati nella Guida TV.
- **Aggiornamenti:** la build Custom non tenta di installare release della
  versione Legacy originale. Per gli aggiornamenti usiamo release e `repo.json`
  della nostra fork.

## Stato

Modifiche sorgenti predisposte. **La compatibilità del player, del login
e la compilazione LG devono ancora essere validate sulla TV.**
La prima implementazione usa le categorie StreamViX 12.4.23, non un
lettore generico degli extra. Il limite nativo di 900 canali e 48
categorie e le quote per catalogo restano validi.

## Verifica senza TV

Su Linux con dipendenze SDL2 e OpenGL ES:

```sh
bash tests/guia_catalogo_tv.sh
bash tests/guia_streamvix.sh
bash tests/guiacorte.sh
bash -n tools/build-custom.sh
python3 -m py_compile tools/custom-homebrew.py
```

I test automatizzati nella scheda GitHub **Actions** utilizzano questi comandi,
non il player reale della TV.

## Build privata e separata per LG

Lo script `tools/build-custom.sh` è basato sullo script DTS Debug del progetto
originale, che compila in una **copia temporanea** e verifica che l'IPK non
includa credenziali personali.

Serve:

1. Docker/Podman e l'immagine `nuvio-webos-sdk` costruita con le istruzioni
   dell'[upstream](https://github.com/iqui27/nuvio-native-legacy#building).
2. Il comando LG `ares-package` installato.
3. Un `local.properties` locale con i valori backend di Nuvio previsti da
   `tools/env.sh`. **Non inserirli nel repository pubblico** e non inviare
   segreti personali in chat. Il repository pubblico non è sufficiente da solo
   a fornire questi parametri.

Comando esemplificativo:

```sh
NUVIO_CONTAINER_RUNTIME=docker \
NUVIO_BUILD_PLATFORM=linux/amd64 \
NUVIO_ARES_PACKAGE=/percorso/assoluto/ares-package \
bash tools/build-custom.sh
```

L'output atteso è in `build/custom/`, compresi IPK, SHA256 e metadati.
La variante Custom viene generata con `-DNV_CUSTOM_BUILD` e
`-DNV_APP_ID="space.nuvio.native.legacy.custom"`.
La build non installa nulla in automatico.

## Installazione sulla LG C2 tramite Homebrew Channel

Dopo aver **compilato e verificato il vero IPK**, creare i metadati:

```sh
python3 tools/custom-homebrew.py \
  build/custom/space.nuvio.native.legacy.custom_2.0.5_arm.ipk \
  --repository aumentandolenoie/nuvio-legacy-custom \
  --tag v2.0.5-custom.1
```

Questo è un *esempio* di nome file e tag: usare quelli dell'IPK effettivo.

Pubblicare su **GitHub Releases** della nostra fork, nella **stessa release
pubblica** con lo stesso tag, i tre file:

- `space.nuvio.native.legacy.custom_..._arm.ipk`
- `build/homebrew-custom/repo.json`
- `build/homebrew-custom/webosbrew.manifest.json`

Solo **dopo la pubblicazione**, aggiungere su Homebrew Channel il repository:

```text
https://github.com/aumentandolenoie/nuvio-legacy-custom/releases/latest/download/repo.json
```

L'originale `iqui27/nuvio-native-legacy` resta installato e con
il suo repository Homebrew. I due pacchetti hanno identità e
cartelle dati distinte: la Custom probabilmente richiede un nuovo login.

## Primo collaudo

1. Verificare avvio, login, immagini e navigazione.
2. Riprodurre un contenuto di test non StreamViX e verificare che la
   nuova identità sia accettata dalle API video webOS.
3. Aprire Guida TV: RAI, Mediaset, Sky, Sport, ecc. devono arrivare
   dal catalogo StreamViX originale, raggruppate secondo il genere.
4. Verificare un canale disponibile e la risoluzione della sua traccia.
5. Confermare che Nuvio Legacy originale continui a funzionare invariato.

**Non confondere un test passato su Linux con una build LG validata.**
