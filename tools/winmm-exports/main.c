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
  struct string_view string = {data, size};
  return string;
}

static int is_whitespace(char c)
{
  return c == '\r' || c == '\n' || c == ' ' || c == '\t' || c == '\f'
      || c == '\v';
}

static void sv_remove_prefix(struct string_view* string, size_t amount)
{
  if (string->size <= amount) {
    *string = sv(NULL, 0);
  } else {
    *string = sv(string->data + amount, string->size + amount);
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

static int sv_equals(struct string_view a, struct string_view b)
{
  if (a.size != b.size) {
    return 0;
  }

  return a.size == 0 ? 1 : memcmp(a.data, b.data, a.size) == 0;
}

static struct string_view sv_substr(struct string_view string,
                                    size_t offset,
                                    size_t length)
{
  size_t size = string.size;
  string.size = 0;
  if (size >= offset) {
    string.data += offset;
    if (size - offset >= length) {
      string.size = length;
    }
  }

  return string;
}

static char const* sv_find(struct string_view string, struct string_view needle)
{
  if (string.size == 0 || string.size < needle.size) {
    return NULL;
  }

  if (needle.size == 0) {
    return string.data;
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

      if (memcmp(it, needle.data, needle.size) == 0) {
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
        string.size = 0;
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
  struct string_view eol = {crlf, 2};
  if (string->size < eol.size) {
    return;
  }

  {
    struct string_view tail = sv_substr(*string, string->size - 2, 2);
    if (!sv_equals(tail, eol)) {
      sv_remove_prefix(&tail, 1);
      sv_remove_prefix(&eol, 1);
      if (!sv_equals(tail, eol)) {
        return;
      }
    }
  }

  sv_remove_suffix(string, eol.size);
}

static HANDLE stdin;
static HANDLE stdout;
static HANDLE stderr;

#define DWORD_MAX 4294967295U

static int output(HANDLE handle, void const* data, size_t size)
{
  DWORD bytes_written = 0;
  if (size > DWORD_MAX) {
    return 1;
  }

  if (WriteFile(handle, data, size, &bytes_written, NULL) == 0) {
    return 1;
  }

  return bytes_written != size;
}

static int output_lf(HANDLE handle)
{
  return output(handle, crlf, sizeof(crlf));
}

#define output_sv(handle, sv) (output(handle, sv.data, sv.size))

#define LINE_BUFFER_SIZE 512

struct line
{
  char buffer[LINE_BUFFER_SIZE];
  size_t size;
};

static struct line line;

static int read_line(size_t* bytes_read)
{
  DWORD _bytes_read = 0;
  if (line.size == sizeof(line.buffer)) {
    return 0;
  }

  SetLastError(0);
  if (ReadFile(stdin,
               line.buffer + line.size,
               sizeof(line.buffer) - line.size,
               &_bytes_read,
               NULL)
      == 0)
  {
    DWORD error = GetLastError();
    if (error != 0 && error != ERROR_BROKEN_PIPE) {
      STRING(message, "Could not read stdin" CRLF);
      return output(stderr, message, sizeof(message)) ? 2 : 1;
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
      if (field.size != 0 && (output_sv(stdout, field) || output_lf(stdout))) {
        return 1;
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
    size_t bytes_read = 0;
    if (more_data) {
      if (read_line(&bytes_read)) {
        code = 1;
        break;
      } else if (bytes_read == 0) {
        more_data = 0;
      } else if (!done) {
        line.size += bytes_read;
      }
    }

    if (!more_data && (done || line.size == 0)) {
      break;
    }

    if (!done) {
      int short_read = 0;
      int line_read_until_end = 0;
      size_t begin = 0;
      do {
        char const* data = line.buffer + begin;
        char const* newline = memchr(data, '\n', line.size - begin);
        if (newline == NULL) {
          if (line.size != sizeof(line.buffer)) {
            short_read = 1;
          }

          break;
        }

        {
          size_t size = (size_t)(newline - data + 1);
          if (process_current_line(sv(data, size), &done)) {
            code = 1;
            goto exit;
          }
          if (done) {
            line.size = 0;
            goto main_loop;
          }
          begin += size;
        }

        line_read_until_end = begin == line.size;
      } while (!line_read_until_end);

      if (!more_data || short_read) {
        if (!line_read_until_end) {
          if (process_current_line(sv(line.buffer + begin, line.size - begin),
                                   &done))
          {
            code = 1;
          }
          if (done) {
            line.size = 0;
            goto main_loop;
          }
        }

        goto exit;
      }

      if (begin == 0) {
        STRING(message,
               "Line too long (longer than " STRINGIFY(LINE_BUFFER_SIZE)
               " bytes). Part of the line:" CRLF);
        code = output(stderr, message, sizeof(message))
                || output(stderr, line.buffer, line.size) || output_lf(stderr)
            ? 2
            : 1;
        goto exit;
      } else if (line_read_until_end) {
        line.size = 0;
      } else {
        size_t new_size = line.size - begin;
        (void)memmove(line.buffer, line.buffer + begin, new_size);
        line.size = new_size;
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
