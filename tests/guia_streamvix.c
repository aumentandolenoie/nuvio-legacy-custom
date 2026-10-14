// Regression tests: StreamViX requires category extras for TV Guide catalog entries.
// No live streams or personal StreamViX credentials are used in this test.
#include "../src/guia.c"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int requests;
char *rede_baixar(const char *url, int timeout) {
  (void)timeout;
  requests++;
  if (strstr(url, "/streamvix_tv/genre=RAI.json?example=1"))
    return strdup("{\"metas\":[{\"id\":\"tv:rai-test\",\"type\":\"tv\",\"name\":\"Rai Test\",\"poster\":\"https://example.com/rai.png\"}]}");
  if (strstr(url, "/streamvix_tv/genre=Mediaset.json?example=1"))
    return strdup("{\"metas\":[{\"id\":\"tv:mediaset-test\",\"type\":\"tv\",\"name\":\"Mediaset Test\"}]}");
  if (strstr(url, "/streamvix_eventi/genre=Premier%20League.json?example=1"))
    return strdup("{\"metas\":[{\"id\":\"tv:premier-test\",\"type\":\"tv\",\"name\":\"Premier Test\"}]}");
  if (strstr(url, "/streamvix_tv/genre=RAI&skip=100.json?example=1"))
    return strdup("{\"metas\":[]}");
  fprintf(stderr, "Unexpected URL in StreamViX test: %s\n", url);
  abort();
}
int addons_base_desligada(const char *base) { (void)base; return 0; }

int main(void) {
  GFonte f = {0};
  int nuovi = -1;
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
  assert(requests == 4);
  puts("PASS: StreamViX genre requests, escaping, dedup, native TV meta parsing");
}
