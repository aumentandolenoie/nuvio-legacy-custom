// Regression tests: StreamViX requires category extras for TV Guide catalog entries.
// No live streams or personal StreamViX credentials are used in this test.
#include "../src/guia.c"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The Guide-to-player handoff uses i18n for the channel subtitle. No UI
// translation service is linked in the isolated host regression test.
const char *i18n(const char *s) { return s; }

static int requests;
// StreamViX Live.py: up to 60 characters of slug, a dash, YYYYMMDD, then
// the Stremio catalog prefixes the ID with "tv:" (72 characters total).
static char romaEventId[96];
char *rede_baixar(const char *url, int timeout) {
  (void)timeout;
  requests++;
  if (strstr(url, "/streamvix_tv/genre=RAI.json?example=1"))
    return strdup("{\"metas\":[{\"id\":\"tv:rai-test\",\"type\":\"tv\",\"name\":\"Rai Test\",\"poster\":\"https://example.com/rai.png\"}]}");
  if (strstr(url, "/streamvix_tv/genre=Mediaset.json?example=1"))
    return strdup("{\"metas\":[{\"id\":\"tv:mediaset-test\",\"type\":\"tv\",\"name\":\"Mediaset Test\"}]}");
  if (strstr(url, "/streamvix_eventi/genre=Premier%20League.json?example=1"))
    return strdup("{\"metas\":[{\"id\":\"tv:premier-test\",\"type\":\"tv\",\"name\":\"Premier Test\"}]}");
  if (strstr(url, "/streamvix_eventi/genre=Coppe.json?example=1")) {
    char body[512];
    snprintf(body, sizeof body,
      "{\"metas\":[{\"id\":\"%s\",\"type\":\"tv\",\"name\":\"Roma - European Cup\",\"genres\":[\"Coppe\"]}]}", romaEventId);
    return strdup(body);
  }
  if (strstr(url, "/streamvix_tv/genre=RAI&skip=100.json?example=1"))
    return strdup("{\"metas\":[]}");
  fprintf(stderr, "Unexpected URL in StreamViX test: %s\n", url);
  abort();
}
int addons_base_desligada(const char *base) { (void)base; return 0; }

int main(void) {
  GFonte f = {0};
  int nuovi = -1;
  // Construct a maximum-length genuine-format StreamViX event identifier.
  memcpy(romaEventId, "tv:", 3);
  memset(romaEventId + 3, 'x', 60);
  memcpy(romaEventId + 3, "roma-vs-", 8);
  memcpy(romaEventId + 63, "-20261015", 10);
  assert(strlen(romaEventId) == 72);

  strcpy(f.base, "https://example.invalid/sv?example=1");
  strcpy(f.tipo, "tv");
  strcpy(f.id, "streamvix_tv");
  strcpy(f.nome, "StreamViX TV");

  assert(svGeneri("streamvix_tv") && svGeneri("streamvix_live"));
  assert(svGeneri("streamvix_eventi"));
  assert(!svGeneri("other-addon"));
  assert(svCatalogoSoloRicerca("streamvix_tv_search"));
  assert(!svCatalogoSoloRicerca("streamvix_tv"));

  assert(lerPaginaEx(&f, 0, 10, &nuovi, "RAI") == 1);
  assert(nuovi == 1 && sNCanais == 1);
  assert(strcmp(sCanais[0].id, "tv:rai-test") == 0);
  assert(sCanais[0].cat >= 0);
  // Deduplicate by channel ID and preserve original metadata.
  assert(lerPaginaEx(&f, 0, 10, &nuovi, "RAI") == 1);
  assert(nuovi == 0 && sNCanais == 1);

  assert(lerPaginaEx(&f, 0, 10, &nuovi, "Mediaset") == 1);
  assert(nuovi == 1 && sNCanais == 2);
  assert(strcmp(sCanais[1].id, "tv:mediaset-test") == 0);

  strcpy(f.id, "streamvix_eventi");
  assert(lerPaginaEx(&f, 0, 10, &nuovi, "Premier League") == 1);
  assert(nuovi == 1 && sNCanais == 3);
  assert(strcmp(sCanais[2].id, "tv:premier-test") == 0);
  assert(lerPaginaEx(&f, 0, 10, &nuovi, "Coppe") == 1);
  assert(nuovi == 1 && sNCanais == 4);
  assert(!strcmp(sCanais[3].id, romaEventId));
  { // The Guide -> CatItem handoff previously lost the final 9+ chars.
    CatItem item;
    canalParaItem(&sCanais[3], &item);
    assert(!strcmp(item.imdb, romaEventId));
    assert(!strcmp(item.tipo, "channel"));
    assert(strlen(item.imdb) == strlen(romaEventId));
  }
  assert(requests == 5);
  puts("PASS: StreamViX Coppe long Roma event ID, genre requests, dedup");
}
