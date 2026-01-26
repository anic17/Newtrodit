#include "newtrodit_core.h"

const int DEFAULT_ALLOC_SIZE = 512;
const size_t LINE_SIZE = 64;
const size_t DEFAULT_ALLOC_LINES = 10;
const size_t LINE_Y_INCREASE = 5;

size_t utf8len_n(const char *s)
{
	if (!s)
		return 0;
	return utf8len(s);
}

size_t utf8nlen_n(const char *s, size_t n)
{
	if (!s)
		return 0;
	return utf8nlen(s, n);
}

size_t utf8len_null(const char *s, size_t max_bytes) // Get the number of UTF-8 characters in a non-null terminated string with a known byte length
{
	if (!s)
		return 0;
	const char *ptr = s;
	utf8_int32_t cp = 0;
	size_t ulen = 0;
	while (*ptr != '\0' && ulen < max_bytes)
	{
		ptr = utf8codepoint(ptr, &cp); // Get a pointer to the next codepoint
		ulen++;						   // Increase the Unicode character counter
	}
	return ulen;
}

size_t strlen_n(const char *s)
{
	if (!s)
		return 0;
	return strlen(s);
}

int vt_settings(bool enabled)
{
#ifdef _WIN32
	DWORD lmode; // Process ANSI escape sequences

	if (!GetConsoleMode(hStdout, &lmode))
		return 0;
	if (enabled)
		lmode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING & ~DISABLE_NEWLINE_AUTO_RETURN;
	else
		lmode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;

	return SetConsoleMode(hStdout, lmode);
#endif
	return 0;
}

/* Safe version of strcpy() and strncpy() */
char *strncpy_n(char *dest, const char *src, size_t count)
{
	// Better version that str8ncpy() because it always null terminates strings

	if (count)
	{
		memset(dest, 0, count);
		strncat(dest, src, count);
		return dest;
	}
	return NULL;
}

size_t strrpbrk(const char *s, const char *find) // Reverse strpbrk, just like strrchr but for multiple characters
{
	size_t findlen = utf8len_n(find);
	size_t slen = utf8len_n(s);
	for (size_t i = slen; i > 0; i--)
	{
		for (size_t j = 0; j < findlen; j++)
		{
			if (s[i - 1] == find[j])
				return i - 1;
		}
	}
	return 0;
}

void *realloc_n(void *old, size_t old_sz, size_t new_sz)
{
	void *new = malloc(new_sz);
	if (!new)
		return NULL;
	memcpy(new, old, old_sz);
	free(old);
	return new;
}

int valid_file_name(const char *filename)
{
	return utf8pbrk(filename, "*?\"<>|\x1b") == NULL;
}

char *remove_quotes(char *s)
{
	size_t len = strlen_n(s);
	if (s[0] == '\"' && s[len - 1] == '\"')
	{
		memmove(s, s + 1, len - 2);
		s[len] = '\0';
	}
	return s;
}

int trim_message(char *msg, size_t max_len)
{
	size_t ulen = utf8len(msg);
	if (ulen > max_len)
	{
		msg[max_len] = '\0';
		return 1;
	}
	return 0;
}

int set_status_msg(bool display_once, char *msg, ...)
{
	va_list args;
	va_start(args, msg);

	ed.status_msg = calloc(DEFAULT_ALLOC_SIZE + 1, sizeof(utf8_int32_t));
	if (!ed.status_msg)
		return 0;

	size_t copyamount = DEFAULT_ALLOC_SIZE * sizeof(utf8_int32_t);
	vsnprintf(ed.status_msg, copyamount, msg, args);
	trim_message(ed.status_msg, ed.xsize - (fullCursorInfoDisplay ? 45 : 20));

	ed.dirty = true;
	ed.displayStatusOnce = display_once;
	return 1;
}

int clear_status_msg()
{
	if (ed.status_msg != NULL)
		free(ed.status_msg);

	ed.status_msg = NULL;
	ed.dirty = true;
	return 1;
}

int last_token_position(const char *s, const char *token)
{

	int lastpos = -1;
	if (!s || !token)
		return lastpos;

	for (size_t i = 0; i < strlen_n(s); i++)
	{
		for (size_t j = 0; j < strlen_n(token); j++)
		{
			if (s[i] == token[j])
				lastpos = i;
		}
	}
	return lastpos;
}

char *last_token(char *tok, const char *char_token)
{
	if (!tok || !char_token)
		return NULL;

	int pos = last_token_position(tok, char_token);
	return tok + pos + 1;
}
