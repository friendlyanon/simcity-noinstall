#include <Windows.h>
#include <stddef.h>
#include <string.h>

#include "array_size.h"

#define STRINGIFYX(x) #x
#define STRINGIFY(x) STRINGIFYX(x)

#define STRING(name, str) static char const name[lengthof(str)] = str

#define CRLF "\r\n"

struct string_view
{
  char const* data;
  size_t size;
};

static struct string_view sv(char const* data, size_t size)
{
  struct string_view string;
  string.data = data;
  string.size = size;
  return string;
}

static int is_whitespace(char c)
{
  return ((unsigned char)(c - 9U) < 5U) | (' ' == c);
}

static void sv_remove_prefix(struct string_view* string, size_t amount)
{
  if (string->size <= amount) {
    *string = sv(NULL, 0);
  } else {
    *string = sv(string->data + amount, string->size - amount);
  }
}

static void sv_remove_suffix(struct string_view* string, size_t amount)
{
  if (string->size > amount) {
    string->size -= amount;
  } else {
    *string = sv(NULL, 0);
  }
}

static void sv_left_trim(struct string_view* string)
{
  size_t i = 0;
  for (; i != string->size; ++i) {
    if (!is_whitespace(string->data[i])) {
      break;
    }
  }

  if (i != 0) {
    sv_remove_prefix(string, i);
  }
}

static char const* sv_find(struct string_view string, struct string_view needle)
{
  if (string.size == 0 || string.size < needle.size) {
    return NULL;
  }

  if (needle.size == 0) {
    return string.data;
  } else if (needle.size == 1) {
    return memchr(string.data, needle.data[0], string.size);
  }

  {
    struct string_view tail = string;
    do {
      size_t remaining = 0;
      char const* it = memchr(tail.data, needle.data[0], tail.size);
      if (it == NULL) {
        return NULL;
      }

      remaining = tail.size - (size_t)(it - tail.data);
      if (remaining < needle.size) {
        return NULL;
      }

      if (memcmp(it + 1, needle.data + 1, needle.size - 1) == 0) {
        return it;
      }

      sv_remove_prefix(&tail, tail.size - remaining + 1);
    } while (tail.size != 0);
  }

  return NULL;
}

static struct string_view sv_get_field(struct string_view string, size_t n)
{
  size_t i = 0;
  sv_left_trim(&string);

  for (; string.size != 0; ++i) {
    char const* it = memchr(string.data, ' ', string.size);
    if (it == NULL) {
      if (i != n) {
        return sv(NULL, 0);
      }

      break;
    }

    if (i == n) {
      string.size = (size_t)(it - string.data);
      break;
    }

    sv_remove_prefix(&string, (size_t)(it - string.data));
    sv_left_trim(&string);
  }

  return string;
}

STRING(crlf, CRLF);

static void sv_chomp_eol(struct string_view* string)
{
  size_t size = string->size;
  size_t index = sizeof(crlf);
  size_t count = 0;
  if (size == 0) {
    return;
  }

  while (1) {
    int is_equal = string->data[--size] == crlf[--index];
    count += is_equal;
    if (!is_equal || size == 0 || index == 0) {
      break;
    }
  }

  if (count != 0) {
    sv_remove_suffix(string, count);
  }
}

static HANDLE stdin;
static HANDLE stdout;
static HANDLE stderr;

#define BUFFER_SIZE 512

struct buffer
{
  char buffer[BUFFER_SIZE];
  size_t size;
};

static struct buffer output;

static int flush(HANDLE handle, struct buffer* buffer)
{
  int result = 0;
  size_t size = buffer->size;
  DWORD bytes_written = 0;
  if (WriteFile(handle, buffer->buffer, size, &bytes_written, NULL) == 0) {
    return 1;
  }

  result = bytes_written != size;
  if (result == 0) {
    buffer->size = 0;
  }

  return result;
}

static int append(struct buffer* buffer, void const* data, size_t size)
{
  size_t sum = buffer->size + size;
  if (sum < size || sum > sizeof(buffer->buffer)) {
    return 1;
  }

  if (size != 0) {
    (void)memcpy(buffer->buffer + buffer->size, data, size);
    buffer->size = sum;
  }

  return 0;
}

static int append_lf(struct buffer* buffer)
{
  return append(buffer, crlf, sizeof(crlf));
}

#define append_sv(buffer, sv) (append(buffer, sv.data, sv.size))

static struct buffer input;

static int read_input(size_t* bytes_read)
{
  DWORD _bytes_read = 0;
  if (input.size == sizeof(input.buffer)) {
    return 0;
  }

  SetLastError(0);
  if (ReadFile(stdin,
               input.buffer + input.size,
               sizeof(input.buffer) - input.size,
               &_bytes_read,
               NULL)
      == 0)
  {
    DWORD error = GetLastError();
    if (error != 0 && error != ERROR_BROKEN_PIPE) {
      STRING(message, "Could not read stdin" CRLF);
      return append(&output, message, sizeof(message)) || flush(stderr, &output)
          ? 2
          : 1;
    }
  }

  *bytes_read = _bytes_read;
  return 0;
}

static int state;

static int process_current_line(struct string_view string, int* done)
{
  sv_chomp_eol(&string);

  if (state == 0) {
    STRING(needle, "RVA");
    if (sv_find(string, sv(needle, sizeof(needle))) != NULL) {
      state = 1;
    }
  } else if (state == 1) {
    state = 2;
  } else if (state == 2) {
    if (string.size == 0) {
      *done = 1;
    } else {
      struct string_view field = sv_get_field(string, 3);
      if (field.size != 0) {
        return append_sv(&output, field) || append_lf(&output)
            || flush(stdout, &output);
      }
    }
  }

  return 0;
}

#ifdef NO_CRT
void mainCRTStartup(void)
#else
int main(void)
#endif
{
  int code = 0;
  int done = 0;
  int more_data = 1;
  stdin = GetStdHandle(STD_INPUT_HANDLE);
  {
    DWORD mode = 0;
    if (GetConsoleMode(stdin, &mode) != 0) {
      /* Input is not piped */
      goto exit;
    }
  }

  stdout = GetStdHandle(STD_OUTPUT_HANDLE);
  stderr = GetStdHandle(STD_ERROR_HANDLE);

  while (1) {
    if (more_data) {
      size_t bytes_read = 0;
      code = read_input(&bytes_read);
      if (code != 0) {
        break;
      } else if (bytes_read == 0) {
        more_data = 0;
      } else if (!done) {
        input.size += bytes_read;
      }
    }

    if (!more_data && (done || input.size == 0)) {
      break;
    }

    if (!done) {
      int short_read = 0;
      int input_read_until_end = 0;
      size_t begin = 0;
      do {
        char const* data = input.buffer + begin;
        char const* newline = memchr(data, '\n', input.size - begin);
        if (newline == NULL) {
          short_read = input.size != sizeof(input.buffer);
          break;
        }

        {
          size_t size = (size_t)(newline - data + 1);
          code = process_current_line(sv(data, size), &done);
          if (code != 0) {
            goto exit;
          }
          if (done) {
            input.size = 0;
            goto main_loop;
          }
          begin += size;
        }

        input_read_until_end = begin == input.size;
      } while (!input_read_until_end);

      if (!more_data || short_read) {
        if (!input_read_until_end) {
          code = process_current_line(
              sv(input.buffer + begin, input.size - begin), &done);
          if (code != 0) {
            goto exit;
          }
          if (done) {
            input.size = 0;
            goto main_loop;
          }
        }

        goto exit;
      }

      if (begin == 0) {
        STRING(message,
               "Line too long (longer than " STRINGIFY(
                   BUFFER_SIZE) " bytes). Part of the line:" CRLF);
        code =
            (append(&output, message, sizeof(message)) || flush(stderr, &output)
             || append(&output, input.buffer, input.size)
             || flush(stderr, &output) || append_lf(&output)
             || flush(stderr, &output))
            + 1;
        goto exit;
      } else if (input_read_until_end) {
        input.size = 0;
      } else {
        size_t new_size = input.size - begin;
        (void)memmove(input.buffer, input.buffer + begin, new_size);
        input.size = new_size;
      }
    }

  main_loop:;
  }

exit:
#ifdef NO_CRT
  ExitProcess(code);
#else
  return code;
#endif
}
