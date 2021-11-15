#include <config/inish.h>
#include <string.h>
#include <test.h>

int main()
{
  describe("empty configuration file");
  inish_config_t* empty = inish_load("./tests/kernel/config/empty.inish");
  assert(empty != NULL, "accepts an empty file");
  inish_free(empty);
  end_describe();

  inish_config_t* conf = inish_load("./tests/kernel/config/test.inish");
  assert(conf != NULL, "reads the configuration file");

  describe("sections");
  inish_section_t* section = inish_get_section(conf, "section");
  assert(section != NULL, "returns a section");

  assert(inish_get_section(conf, "section-not-found") == NULL,
         "returns NULL when the section is not found");
  end_describe();

  describe("strings");
  char* str_value = inish_get_string(section, "str-value");
  assert(str_value != NULL, "returns a string value");

  assert(strcmp(inish_get_string(section, "multiline_str"),
                "this\nis\na\nmultiline string") == 0,
         "returns a multiline string value");

  assert(strlen(inish_get_string(section, "long_str")) == 123,
         "returns a long multiline string value");
  end_describe();

  describe("booleans");
  opt_bool_t enabled = inish_get_bool(section, "enabled");
  assert(enabled.has_value && enabled.value, "returns a boolean value");
  enabled = inish_get_bool(section, "enabled_int");
  assert(enabled.has_value && enabled.value,
         "returns a boolean value that was set with 1");
  opt_bool_t disabled = inish_get_bool(section, "disabled");
  assert(disabled.has_value && !disabled.value, "returns a boolean value");
  disabled = inish_get_bool(section, "disabled_int");
  assert(disabled.has_value && !disabled.value,
         "returns a boolean value that was set with 0");
  end_describe();

  describe("integers");
  opt_uint64_t plus_one = inish_get_int(section, "plus_one");
  assert(plus_one.has_value && plus_one.value == 1,
         "returns a positive integer");
  assert(inish_get_int(section, "minus_one").value == -1,
         "returns a negative integer");
  assert(inish_get_int(section, "zero").value == 0, "returns zero");
  assert(inish_get_int(section, "million").value == 1000000,
         "returns an integer with underscores");
  end_describe();

  describe("IPv4");
  uint8_t localhost[4] = { 0 };
  uint8_t bytes_set = inish_get_ipv4(section, "localhost", localhost);
  assert(bytes_set == 4, "returns the number of bytes set in the ip array");
  assert(localhost[0] == 127 && localhost[1] == 0 && localhost[2] == 0 &&
           localhost[3] == 1,
         "loads an IPv4 address");
  end_describe();

  describe("arrays");
  inish_array_t* array = inish_get_array(section, "array");
  assert(array != NULL, "returns an array");
  assert(array->n_values == 2, "sets the number of values in the array");
  assert(strcmp(array->values[0], "123") == 0, "contains the first value");
  assert(strcmp(array->values[1], "456") == 0, "contains the second value");
  end_describe();

  inish_free(conf);

  return test_summary();
}
