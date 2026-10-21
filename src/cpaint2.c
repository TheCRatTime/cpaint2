/*
 * Copyright (C) 2026 TheCRatTime
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Comment */
static void Comment(void) {
  fputs("\033[2m/* === Painted by C source highlight === */\033[0m\n"
        "\033[2m/* ---          Do not edit          --- */\033[0m\n"
        "\033[2m/* ---   Copyright (C) TheCRatTime   --- */\033[0m\n"
        "\033[2m/* ===       Begin painted code      === */\033[0m\n",
        stdout);
}

static const char* Next(const char** code) {
  return (**code ? ((*code)++) : '\0');
}

static const char Peek(const char* code) {
  return (*code ? *(code+1) : '\0');
}

static int ExcludePunct(const char punct) {
  if (punct == '_') {
    return 1;
  }

  return 0;
}

static void PrintPunct(const char** code) {
  if (!ExcludePunct(**code)) {
    fputs("\033[34m", stdout);
    putchar(*Next(code));
    fputs("\033[0m", stdout);
  } else {
    putchar(*Next(code));
  }
}

static void PrintQuotes(const char** code) {
  const char quote = **code;
  fputs("\033[32m", stdout);
  putchar(*Next(code));
  
  while (**code) {
    if (**code == quote) {
      int slashes = 0;
      const char* copy = *code;
      copy--;

      while (*copy == '\\') {
        slashes++;
        copy--;
      }

      if ((slashes & 1) == 0) {
        break;
      }
    }

    putchar(*Next(code));
  }

  putchar(*Next(code));
  fputs("\033[0m", stdout);
}

/* Print digit function */
static void PrintDigit(const char** code,
                       const char* const first_ptr) {
  if (*code != first_ptr) {
    if (isalpha(*((*code)-1)) || *((*code)-1) == '_' ||
        *((*code)-1) == ')') {
      while (isdigit(**code)) {
        putchar(*Next(code));
      }
      
      return;
    }
  }

  fputs("\033[36m", stdout);
  putchar(*Next(code));
  fputs("\033[0m", stdout);
}

/* Print comment function */
static void PrintComment(const char** code) {
  unsigned char type = 0;
  if (Peek(*code) != '/' && Peek(*code) != '*') {
    putchar(*Next(code));
    return;
  }

  type = (Peek(*code) == '/' ? 1 : 2);

  if (type == 1) {
    fputs("\033[2m", stdout);
    while (**code && **code != '\n') {
      putchar(*Next(code));
    }

    putchar(*Next(code));
    
    fputs("\033[0m", stdout);
  } else {
    char* end = strstr(*code, "*/");
    
    fputs("\033[2m", stdout);

    if (end != NULL) {
      fwrite(*code, 1, (end - *code) + 2, stdout);
      (*code) = end+2;
    } else {
      size_t left = strlen(*code);
      fwrite(*code, 1, left, stdout);
      (*code) += left;
    }
    
    fputs("\033[0m", stdout);
  }
}

/* C PreProcessor print */
static void PrintPP(const char** code,
                    const char* const first_ptr) {
  int is_endif = 0;
  if (*code != first_ptr) {
    if (*((*code)-1) != '\n') {
      putchar(*Next(code));
      return;
    }
  }

  fputs("\033[31m", stdout);

  putchar(*Next(code));

  is_endif = !strncmp(*code, "endif", 5);

  while (**code == ' ') {
    putchar(*Next(code));
  }

  while (**code && **code != ' ' && **code != '\n') {
    putchar(*Next(code));
  }

  putchar(*Next(code));
  
  if (is_endif) {
    fputs("\033[0m", stdout);
    return;
  }

  if (*((*code)-1) == '\n') {
    return;
  }

  fputs("\033[35m", stdout);

  while (**code) {
    if (**code == '\n') {
      if (*((*code)-1) != '\\') {
        break;
      }
    }

    putchar(*Next(code));
  }

  putchar(*Next(code));
  
  fputs("\033[0m", stdout);
}

#define TOTAL_KEYWORDS 35

static int CantBeAtKeywordStart(const char ch) {
  if (ch != ' ' && ch != '\t' && ch != '(' && ch != '\n' && ch != ';') {
    return 1;
  }

  return 0;
}

static int CantBeAtKeywordEnd(const char ch) {
  if (ch != ' ' && ch != '*' && ch != ';' && ch != '(' &&
      ch != ':' && ch != ')' && ch != '\0') {
    return 1;
  }

  return 0;
}

/* Print keywords function */
static int PrintKeyword(const char** code,
                        const char* const first_ptr) {
  int i = 0;
  const char* j = NULL;
  
  const struct {
    char* name;
    int len;
  } Keywords[TOTAL_KEYWORDS] = {
    {"if", 2},
    {"int", 3},
    {"for", 3},
    {"void", 4},
    {"char", 4},
    {"long", 4},
    {"goto", 4},
    {"case", 4},
    {"FILE", 4},
    {"else", 4},
    {"bool", 4},
    {"true", 4},
    {"union", 5},
    {"const", 5},
    {"short", 5},
    {"while", 5},
    {"false", 5},
    {"break", 5},
    {"float", 5},
    {"struct", 6},
    {"switch", 6},
    {"double", 6},
    {"static", 6},
    {"extern", 6},
    {"size_t", 6},
    {"sizeof", 6},
    {"signed", 6},
    {"inline", 6},
    {"return", 6},
    {"typedef", 7},
    {"default", 7},
    {"volatile", 8},
    {"unsigned", 8},
    {"continue", 8},
    {"restrict", 8}
  };

  for (; i < TOTAL_KEYWORDS; i++) {
    int equals = !strncmp(*code, Keywords[i].name, Keywords[i].len);
    
    if (!equals) {
      continue;
    }

    if (*code != first_ptr) {
      if (CantBeAtKeywordStart(*((*code)-1))) {
        continue;
      }
    }

    if (CantBeAtKeywordEnd(*((*code)+Keywords[i].len))) {
      continue;
    }

    fputs("\033[31m", stdout);
    for (j = *code; j < (*code)+Keywords[i].len; Next(&j)) {
      putchar(*j);
    }

    (*code) += Keywords[i].len;
    fputs("\033[0m", stdout);
    return 1;
  }
  
  return 0;
}

/* Source highlight function */
static void SourceHighlight(const char* code) {
  const char* const first_ptr = code;
  Comment();
  while (*code) {
    if (PrintKeyword(&code, first_ptr)) {
      continue;
    } else if (*code == '"' || *code == '\'') {
      PrintQuotes(&code);
    } else if (*code == '/') {
      PrintComment(&code);
    } else if (*code == '#') {
      PrintPP(&code, first_ptr);
    } else if (isdigit(*code)) {
      PrintDigit(&code, first_ptr);
    } else if (ispunct(*code)) {
      PrintPunct(&code);
    } else {
      putchar(*Next(&code));
    }
  }
}

/* Find at end function. */
static int HasAtEnd(const char* str1, const char* str2) {
  size_t len1 = strlen(str1);
  size_t len2 = strlen(str2);
  int i = 0;
  int j = 0;

  if (len1 < len2) {
    return 0;
  }

  i = (int)len1-1;
  j = (int)len2-1;
  
  for (; i >= 0 && j >= 0; i--, j--) {
    if (str1[i] != str2[j]) {
      return 0;
    }
  }
  
  return 1;
}

/* Main function. */
int main(int argc, char** argv) {
  char* buffer = NULL;
  FILE* file = NULL;
  size_t size = 0;
  
  if (argc < 2) {
    printf("Usage: %s FILE\n", argv[0]);
    return 1;
  }

  if (!HasAtEnd(argv[1], ".c") && !HasAtEnd(argv[1], ".h")) {
    printf("%s: %s: isn't a header or source\n", argv[0], argv[1]);
    return 1;
  }

  file = fopen(argv[1], "rb");
  if (file == NULL) {
    perror(argv[0]);
    return 1;
  }

  fseek(file, 0, SEEK_END);
  size = (size_t)ftell(file);
  fseek(file, 0, SEEK_SET);

  if (size <= 0) {
    printf("%s: file size %lu ignored\n", argv[0], (unsigned long)size);
    fclose(file);
    return 1;
  }

  buffer = (char*)malloc(size+1);
  if (buffer == NULL) {
    perror(argv[0]);
    fclose(file);
    return 1;
  }

  if (fread(buffer, 1, size, file) <= 0) {
    perror(argv[0]);
    fclose(file);
    free(buffer);
    return 1;
  }

  buffer[size] = '\0';

  fclose(file);

  SourceHighlight(buffer);

  free(buffer);
  return 0;
}
