#ifndef JSON_H
#define JSON_H

/**
 * json_getstr retrieves a string value from the given JSON using a dot-notated
 * getter string. Returns NULL if no value found
 *
 * @param json
 * @param getter
 * @return char*
 *
 * @example json = { "outer": { "inner": "value" } }
 * json_getstr(json, "outer.inner")
 */
char *json_getstr(const char *json, const char *getter);

/**
 * json_getint retrieves an int value from the given JSON using a dot-notated
 * getter string. Returns NULL if no value found
 *
 * @param json
 * @param getter
 * @return int
 *
 * @example json = { "outer": { "inner": 10 } }
 * json_getint(json, "outer.inner")
 */
int json_getint(const char *json, const char *getter);

#endif /* JSON_H */
