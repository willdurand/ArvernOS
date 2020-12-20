#include "inish.h"
#include <core/core-logging.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// We use `DEBUG()` to always output this message.
#define SYNTAX_ERROR(message, line)                                            \
  DEBUG("SYNTAX ERROR: %s near line %ld", message, line);                      \
  goto end;

#define DEFAULT_VALUE_LEN 50

typedef enum token_type
{
  NO_TOKEN,
  FILE_END,
  ERROR,
  COMMENT,
  NEWLINE,
  WHITESPACE,
  OPEN_BRACE,
  CLOSE_BRACE,
  EQUALS_SIGN,
  COMMA,
  IDENTIFIER,
  NUMBER,
  STRING,
} token_type_t;

typedef struct token
{
  token_type_t type;
  char* value;
  uint64_t len;
} token_t;

char inish_getchar(int fd);
token_t* inish_get_next_token(int fd);
token_t* inish_lex_token(int fd);
token_t* inish_lex_whitespace(int fd, token_t* t);
token_t* inish_lex_newline(int fd, token_t* t);
token_t* inish_lex_comment(int fd, token_t* t);
token_t* inish_lex_identifier(int fd, token_t* t);
token_t* inish_lex_string(int fd, token_t* t);
token_t* inish_lex_number(int fd, token_t* t);
void inish_add_char_to_token_value(token_t* t, char c);

static token_t* next_token;
static uint64_t line;

void inish_add_char_to_token_value(token_t* t, char c)
{
  if (t->len >= DEFAULT_VALUE_LEN) {
    t->value =
      (char*)realloc(t->value, (t->len + DEFAULT_VALUE_LEN) * sizeof(char));
  }

  t->value[t->len++] = c;
}

token_t* inish_get_next_token(int fd)
{
  token_t* token = next_token;
  next_token = inish_lex_token(fd);

  // discard rule
  while (next_token->type == WHITESPACE || next_token->type == COMMENT) {
    free(next_token->value);
    free(next_token);
    next_token = inish_lex_token(fd);
  }

  return token;
}

token_t* inish_lex_token(int fd)
{
  token_t* t = (token_t*)calloc(1, sizeof(token_t));
  t->value = (char*)calloc(1, DEFAULT_VALUE_LEN * sizeof(char));

  char c = inish_getchar(fd);

  switch (c) {
    case ' ':
    case '\t':
      inish_add_char_to_token_value(t, c);
      return inish_lex_whitespace(fd, t);
    case '\n':
      inish_add_char_to_token_value(t, c);
      return inish_lex_newline(fd, t);
    case '"':
      return inish_lex_string(fd, t);
    case '[':
      t->value[0] = c;
      t->len = 1;
      t->type = OPEN_BRACE;
      break;
    case ']':
      t->value[0] = c;
      t->len = 1;
      t->type = CLOSE_BRACE;
      break;
    case '=':
      t->value[0] = c;
      t->len = 1;
      t->type = EQUALS_SIGN;
      break;
    case ',':
      t->value[0] = c;
      t->len = 1;
      t->type = COMMA;
      break;
    case '#':
      return inish_lex_comment(fd, t);
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '-':
    case '+':
      inish_add_char_to_token_value(t, c);
      return inish_lex_number(fd, t);
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
      inish_add_char_to_token_value(t, c);
      return inish_lex_identifier(fd, t);
    case 0:
      t->type = FILE_END;
      break;
    default:
      t->type = ERROR;
  }

  return t;
}

token_t* inish_lex_whitespace(int fd, token_t* t)
{
  int pos = lseek(fd, 0, SEEK_CUR);
  char c = inish_getchar(fd);

  while (1) {
    switch (c) {
      case ' ':
      case '\t':
        inish_add_char_to_token_value(t, c);
        break;
      default:
        lseek(fd, pos, SEEK_SET);
        t->type = WHITESPACE;
        return t;
    }

    pos = lseek(fd, 0, SEEK_CUR);
    c = inish_getchar(fd);
  }
}

token_t* inish_lex_newline(int fd, token_t* t)
{
  line++;

  t->type = NEWLINE;
  return t;
}

token_t* inish_lex_comment(int fd, token_t* t)
{
  char c = inish_getchar(fd);

  while (1) {
    switch (c) {
      case '\n':
        line++;
      case 0:
        t->type = COMMENT;
        return t;
      default:
        inish_add_char_to_token_value(t, c);
    }

    c = inish_getchar(fd);
  }

  return NULL;
}

token_t* inish_lex_identifier(int fd, token_t* t)
{
  int pos = lseek(fd, 0, SEEK_CUR);
  char c = inish_getchar(fd);

  while (1) {
    switch (c) {
      case '_':
      case '-':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
      case 'g':
      case 'h':
      case 'i':
      case 'j':
      case 'k':
      case 'l':
      case 'm':
      case 'n':
      case 'o':
      case 'p':
      case 'q':
      case 'r':
      case 's':
      case 't':
      case 'u':
      case 'v':
      case 'w':
      case 'x':
      case 'y':
      case 'z':
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'G':
      case 'H':
      case 'I':
      case 'J':
      case 'K':
      case 'L':
      case 'M':
      case 'N':
      case 'O':
      case 'P':
      case 'Q':
      case 'R':
      case 'S':
      case 'T':
      case 'U':
      case 'V':
      case 'W':
      case 'X':
      case 'Y':
      case 'Z':
        inish_add_char_to_token_value(t, c);
        break;
      default:
        lseek(fd, pos, SEEK_SET);
        t->type = IDENTIFIER;
        return t;
    }

    pos = lseek(fd, 0, SEEK_CUR);
    c = inish_getchar(fd);
  }

  return NULL;
}

token_t* inish_lex_string(int fd, token_t* t)
{
  char c = inish_getchar(fd);
  int pos = lseek(fd, 0, SEEK_CUR);

  // multiline sniffing
  bool multiline = (c == '"' && inish_getchar(fd) == '"');

  if (multiline) {
    c = inish_getchar(fd);
  } else {
    lseek(fd, pos, SEEK_SET);
  }

  while (1) {
    switch (c) {
      case '"':
        if (multiline) {
          if (inish_getchar(fd) != '"' || inish_getchar(fd) != '"') {
            t->type = ERROR;
            return t;
          }
        }

        t->type = STRING;
        return t;
      case '\n':
        if (multiline) {
          inish_add_char_to_token_value(t, c);
          break;
        }
      case 0:
        t->type = ERROR;
        return t;
      default:
        inish_add_char_to_token_value(t, c);
        break;
    }

    c = inish_getchar(fd);
  }

  return NULL;
}

token_t* inish_lex_number(int fd, token_t* t)
{
  int pos = lseek(fd, 0, SEEK_CUR);
  char c = inish_getchar(fd);

  while (1) {
    switch (c) {
      case '_':
        // This is a separator that should not have any mathematical value so
        // let's skip it.
        break;
      case '.':
        // We don't skip the dot because it could be used in a floating number.
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        inish_add_char_to_token_value(t, c);
        break;
      default:
        lseek(fd, pos, SEEK_SET);
        t->type = NUMBER;
        return t;
    }

    pos = lseek(fd, 0, SEEK_CUR);
    c = inish_getchar(fd);
  }

  return NULL;
}

void inish_free(inish_config_t* conf)
{
  for (int i = 0; i < conf->n_sections; i++) {
    inish_section_t* s = conf->sections[i];

    for (int j = 0; j < s->n_kv_pairs; j++) {
      inish_kv_pair_t* kv = s->kv_pairs[j];

      free(kv->key);
      free(kv->value);
      free(kv);
    }

    for (int j = 0; j < s->n_arrays; j++) {
      inish_array_t* array = s->arrays[j];

      for (int k = 0; k < array->n_values; k++) {
        free(array->values[k]);
      }

      free(array->key);
      free(array->values);
      free(array);
    }

    free(s->name);
    free(s->kv_pairs);
    free(s->arrays);
    free(s);
  }

  free(conf->sections);
  free(conf);
}

char inish_getchar(int fd)
{
  char c;

  if (read(fd, &c, 1) != 1) {
    c = 0;
  }

  return c;
}

inish_config_t* inish_load(const char* filename)
{
  int fd = open(filename, O_RDONLY);

  if (fd < -1) {
    return NULL;
  }

  next_token = (token_t*)calloc(1, sizeof(token_t));
  next_token->type = NO_TOKEN;

  line = 1;

  inish_config_t* conf = (inish_config_t*)calloc(1, sizeof(inish_config_t));
  conf->sections = (inish_section_t**)calloc(0, sizeof(inish_section_t*));

  // look ahead
  token_t* no_token = inish_get_next_token(fd);

  while (next_token->type != FILE_END) {
    token_t* t = NULL;

    if (next_token->type == NEWLINE) {
      t = inish_get_next_token(fd);
    } else {
      // Parse section header.
      t = inish_get_next_token(fd);

      if (t->type != OPEN_BRACE) {
        free(t->value);
        free(t);
        SYNTAX_ERROR("expected open brace", line);
      }

      free(t->value);
      free(t);
      t = inish_get_next_token(fd);

      if (t->type != IDENTIFIER) {
        free(t->value);
        free(t);
        SYNTAX_ERROR("expected identifier", line);
      }

      inish_section_t* section =
        (inish_section_t*)calloc(1, sizeof(inish_section_t));
      section->name = (char*)calloc(1, sizeof(char) * (t->len + 1));
      memcpy(section->name, t->value, t->len);

      section->kv_pairs =
        (inish_kv_pair_t**)calloc(0, sizeof(inish_kv_pair_t*));
      section->arrays = (inish_array_t**)calloc(0, sizeof(inish_array_t*));

      conf->sections = (inish_section_t**)realloc(
        conf->sections, (conf->n_sections + 1) * sizeof(inish_section_t*));
      conf->sections[conf->n_sections++] = section;

      free(t->value);
      free(t);
      t = inish_get_next_token(fd);

      if (t->type != CLOSE_BRACE) {
        free(t->value);
        free(t);
        SYNTAX_ERROR("expected close brace", line);
      }

      free(t->value);
      free(t);
      t = inish_get_next_token(fd);

      if (t->type != NEWLINE && t->type != FILE_END) {
        free(t->value);
        free(t);
        SYNTAX_ERROR("expected newline", line);
      }

      // Parse section values.
      while (next_token->type == IDENTIFIER || next_token->type == NEWLINE) {
        free(t->value);
        free(t);
        t = inish_get_next_token(fd);

        if (t->type == NEWLINE) {
          free(t->value);
          free(t);
          continue;
        }

        if (t->type != IDENTIFIER) {
          free(t->value);
          free(t);
          SYNTAX_ERROR("expected identifier", line);
        }

        char* key = (char*)calloc(1, sizeof(char) * (t->len + 1));
        memcpy(key, t->value, t->len);

        free(t->value);
        free(t);
        t = inish_get_next_token(fd);

        if (t->type != EQUALS_SIGN) {
          free(t->value);
          free(t);
          SYNTAX_ERROR("expected equals sign", line);
        }

        free(t->value);
        free(t);
        t = inish_get_next_token(fd);

        if (t->type == OPEN_BRACE) {
          // Parse array.
          inish_array_t* array =
            (inish_array_t*)calloc(1, sizeof(inish_array_t));
          array->key = (char*)calloc(1, sizeof(char) * (strlen(key) + 1));
          memcpy(array->key, key, strlen(key));

          array->values = (char**)calloc(0, sizeof(char*));

          while (next_token->type == IDENTIFIER || next_token->type == STRING ||
                 next_token->type == NUMBER) {
            free(t->value);
            free(t);
            t = inish_get_next_token(fd);

            if (t->type != IDENTIFIER && t->type != STRING &&
                t->type != NUMBER) {
              free(t->value);
              free(t);
              SYNTAX_ERROR("expected identifier, string or number", line);
            }

            char* value = (char*)calloc(1, sizeof(char) * (t->len + 1));
            memcpy(value, t->value, t->len);

            array->values = (char**)realloc(
              array->values, (array->n_values + 1) * sizeof(char*));
            array->values[array->n_values++] = value;

            free(t->value);
            free(t);
            t = inish_get_next_token(fd);

            if (t->type != COMMA && t->type != CLOSE_BRACE) {
              free(t->value);
              free(t);
              SYNTAX_ERROR("expected comma or close brace", line);
            }
          }

          section->arrays = (inish_array_t**)realloc(
            section->arrays, (section->n_arrays + 1) * sizeof(inish_array_t*));
          section->arrays[section->n_arrays++] = array;
        } else {
          // Parse value.
          if (t->type != IDENTIFIER && t->type != STRING && t->type != NUMBER) {
            free(t->value);
            free(t);
            SYNTAX_ERROR("expected identifier, string or number", line);
          }

          inish_kv_pair_t* kv_pair =
            (inish_kv_pair_t*)calloc(1, sizeof(inish_kv_pair_t));

          kv_pair->key = (char*)calloc(1, sizeof(char) * (strlen(key) + 1));
          memcpy(kv_pair->key, key, strlen(key));
          kv_pair->value = (char*)calloc(1, sizeof(char) * (t->len + 1));
          memcpy(kv_pair->value, t->value, t->len);

          section->kv_pairs = (inish_kv_pair_t**)realloc(
            section->kv_pairs,
            (section->n_kv_pairs + 1) * sizeof(inish_kv_pair_t*));
          section->kv_pairs[section->n_kv_pairs++] = kv_pair;
        }

        free(key);

        free(t->value);
        free(t);
        t = inish_get_next_token(fd);

        if (t->type != NEWLINE && t->type != FILE_END) {
          free(t->value);
          free(t);
          SYNTAX_ERROR("expected newline", line);
        }
      }
    }

    if (t) {
      free(t->value);
      free(t);
    }
  }

end:
  free(no_token->value);
  free(no_token);
  free(next_token->value);
  free(next_token);

  close(fd);

  return conf;
}

inish_section_t* inish_get_section(inish_config_t* conf, const char* name)
{
  if (conf == NULL) {
    return NULL;
  }

  for (int i = 0; i < conf->n_sections; i++) {
    inish_section_t* s = conf->sections[i];

    if (strcmp(s->name, name) == 0) {
      return s;
    }
  }

  return NULL;
}

inish_kv_pair_t* inish_get_kv_pair(inish_section_t* section, const char* key)
{
  if (section == NULL) {
    return NULL;
  }

  for (int i = 0; i < section->n_kv_pairs; i++) {
    inish_kv_pair_t* kv = section->kv_pairs[i];

    if (strcmp(kv->key, key) == 0) {
      return kv;
    }
  }

  return NULL;
}

inish_array_t* inish_get_array(inish_section_t* section, const char* key)
{
  if (section == NULL) {
    return NULL;
  }

  for (int i = 0; i < section->n_arrays; i++) {
    inish_array_t* array = section->arrays[i];

    if (strcmp(array->key, key) == 0) {
      return array;
    }
  }

  return NULL;
}

char* inish_get_string(inish_section_t* section, const char* key)
{
  inish_kv_pair_t* kv = inish_get_kv_pair(section, key);

  if (kv == NULL) {
    return NULL;
  }

  return kv->value;
}

opt_uint64_t inish_get_int(inish_section_t* section, const char* key)
{
  inish_kv_pair_t* kv = inish_get_kv_pair(section, key);

  if (kv == NULL) {
    return (opt_uint64_t){ .has_value = false, .value = 0 };
  }

  return (opt_uint64_t){ .has_value = true, .value = atoi(kv->value) };
}

opt_bool_t inish_get_bool(inish_section_t* section, const char* key)
{
  inish_kv_pair_t* kv = inish_get_kv_pair(section, key);

  if (kv == NULL) {
    return (opt_bool_t){ .has_value = false, .value = false };
  }

  return (opt_bool_t){ .has_value = true,
                       .value = (strcmp(kv->value, "true") == 0) ||
                                (strcmp(kv->value, "1") == 0) };
}

uint8_t inish_get_ipv4(inish_section_t* section, const char* key, uint8_t ip[4])
{
  inish_kv_pair_t* kv = inish_get_kv_pair(section, key);

  if (kv == NULL) {
    return 0;
  }

  char* p = strtok(kv->value, ".");

  uint8_t i = 0;
  while (p != NULL && i < 4) {
    ip[i++] = atoi(p);
    p = strtok(NULL, ".");
  }

  return i;
}
