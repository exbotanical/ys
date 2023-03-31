#ifndef JSOB_H
#define JSOB_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * jsob_getstr retrieves a string value from the given JSON using a dot-notated
 * getter string. Returns NULL if no value found
 *
 * @param json
 * @param getter
 * @return char*
 *
 * @example json = { "outer": { "inner": "value" } }
 * jsob_getstr(json, "outer.inner")
 */
char *jsob_getstr(const char *json, const char *getter);

/**
 * jsob_getint retrieves an int value from the given JSON using a dot-notated
 * getter string. Returns NULL if no value found
 *
 * @param json
 * @param getter
 * @return int
 *
 * @example json = { "outer": { "inner": 10 } }
 * jsob_getint(json, "outer.inner")
 */
int jsob_getint(const char *json, const char *getter);

#ifdef __cplusplus
}
#endif

#endif /* JSOB_H */
