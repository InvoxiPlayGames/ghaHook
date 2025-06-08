#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ghaHook_config
{
    int EnableIOHooks;
    int EnableDongleHooks;
} ghaHook_config;

extern ghaHook_config config;

void load_config();

#ifdef __cplusplus
}
#endif

#endif // CONFIG_H
