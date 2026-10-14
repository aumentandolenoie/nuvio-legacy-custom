#ifndef NV_APP_ID_H
#define NV_APP_ID_H
/* Package metadata and the runtime must be built with the same identity. */
#define NV_APP_ID_PRODUCTION "space.nuvio.native.legacy"
#ifndef NV_APP_ID
#ifdef NV_CUSTOM_BUILD
// Use a C string literal rather than -DNV_APP_ID=...: the ARM build creates
// a compiler response file, whose quote parser strips quotes from -D flags.
// That produced 'space' undeclared in src/dados.c (LG Custom build).
#define NV_APP_ID "space.nuvio.native.legacy.custom"
#elif defined(NV_DTS_DEBUG)
#error "NV_DTS_DEBUG requires an isolated NV_APP_ID package identity"
#else
#define NV_APP_ID NV_APP_ID_PRODUCTION
#define NV_APP_ID_DEFAULT 1
#endif
#endif
#ifdef NV_APP_ID_DEFAULT
#define NV_LS_MEDIA_CLIENT "com.webos.media.client.nuvio"
#else
#define NV_LS_MEDIA_CLIENT "com.webos.media.client." NV_APP_ID
#endif
#endif
