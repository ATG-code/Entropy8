#ifndef ENTROPY8_CONFIG_H
#define ENTROPY8_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_WIN64)
#  ifdef ENTROPY8_EXPORTS
#    define ENTROPY8_API __declspec(dllexport)
#  else
#    define ENTROPY8_API __declspec(dllimport)
#  endif
#else
#  define ENTROPY8_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
}
#endif

#endif /* ENTROPY8_CONFIG_H */
