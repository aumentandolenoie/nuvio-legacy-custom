// Regression for LG Custom app ID: protect against bare-token -D macro
// which produced: error: 'space' undeclared in src/dados.c.
#include "../src/app_id.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
#ifdef NV_CUSTOM_BUILD
#ifdef NV_APP_ID_DEFAULT
#error Custom build must not reuse the production identity
#endif
  assert(strcmp(NV_APP_ID, "space.nuvio.native.legacy.custom") == 0);
  assert(strcmp(NV_LS_MEDIA_CLIENT,
      "com.webos.media.client.space.nuvio.native.legacy.custom") == 0);
  assert(strcmp(NV_APP_ID, NV_APP_ID_PRODUCTION) != 0);
  puts("PASS: Custom app identity and webOS media client");
#else
  assert(strcmp(NV_APP_ID, NV_APP_ID_PRODUCTION) == 0);
  assert(strcmp(NV_LS_MEDIA_CLIENT, "com.webos.media.client.nuvio") == 0);
  puts("PASS: original Legacy identity unchanged");
#endif
  return 0;
}
