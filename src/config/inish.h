/**
 * @file
 *
 * inish configuration parser. inish supports more features than the INI format
 * and less than TOML... This module implements a recursive descent parser, see
 * http://www.rioki.org/2016/04/18/recusive-descent-parser.html for some more
 * information.
 */
#ifndef CONFIG_INISH_H
#define CONFIG_INISH_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#ifndef __is_libk
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

typedef struct inish_kv_pair
{
  char* key;
  char* value;
} inish_kv_pair_t;

typedef struct inish_array
{
  char* key;
  uint64_t n_values;
  char** values;
} inish_array_t;

typedef struct inish_section
{
  char* name;

  uint64_t n_kv_pairs;
  inish_kv_pair_t** kv_pairs;

  uint64_t n_arrays;
  inish_array_t** arrays;
} inish_section_t;

typedef struct inish_config
{
  uint64_t n_sections;
  inish_section_t** sections;
} inish_config_t;

/**
 * Parses and loads a inish configuration file in memory.
 *
 * @param filename the configuration filename to load
 * @return a inish configuration or `NULL` in case of an error
 */
inish_config_t* inish_load(const char* filename);

/**
 * Releases the memory allocated after `inish_read()` has run. This function
 * should be call once the inish configuration isn't used anymore.
 *
 * @param conf the inish configuration to free
 */
void inish_free(inish_config_t* conf);

/**
 * Returns a inish section.
 *
 * @param conf a inish configuration
 * @param name the name of the section to return
 * @param the section if found, `NULL` otherwise
 */
inish_section_t* inish_get_section(inish_config_t* conf, const char* name);

/**
 * Returns a inish key/value structure. This is the "raw" key/value but inish
 * also offers specialized functions to directly get the value for a given key.
 *
 * @param section a inish section
 * @param key the key of pair to return
 * @return the section if found, `NULL` otherwise
 */
inish_kv_pair_t* inish_get_kv_pair(inish_section_t* section, const char* key);

/**
 * Returns a inish array structure. This is the "raw" array.
 *
 * @param section a inish section
 * @param key the key of the array to return
 * @return the array if found, `NULL` otherwise
 */
inish_array_t* inish_get_array(inish_section_t* section, const char* key);

/**
 * Returns the string value for the given key.
 *
 * @param section a inish section
 * @param key the key of the value to return
 * @return the value if found, `NULL` otherwise
 */
char* inish_get_string(inish_section_t* section, const char* key);

/**
 * Returns the integer value for the given key.
 *
 * @param section a inish section
 * @param key the key of the value to return
 * @return an optional type, indicating whether there is a value for the key
 * and the value itself if that's the case
 */
opt_uint64_t inish_get_int(inish_section_t* section, const char* key);

/**
 * Returns the boolean value for the given key.
 *
 * @param section a inish section
 * @param key the key of the value to return
 * @return an optional type, indicating whether there is a value for the key
 * and the value itself if that's the case
 */
opt_bool_t inish_get_bool(inish_section_t* section, const char* key);

/**
 * Loads an IPv4 address based on a value for the given key.
 *
 * @param section a inish section
 * @param key the key of the value to return
 * @param ip the array that should hold the IPv4 address
 * @return the number of bytes loaded in the ip array
 */
uint8_t inish_get_ipv4(inish_section_t* section,
                       const char* key,
                       uint8_t ip[4]);

#endif
