/*
  Copyright 2016 James Hunt <james@jameshunt.us>

  This file is part of libvigor.

  libvigor is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  libvigor is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with libvigor.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <vigor.h>
#include "impl.h"

/*

    ########     ###    ######## ##     ##
    ##     ##   ## ##      ##    ##     ##
    ##     ##  ##   ##     ##    ##     ##
    ########  ##     ##    ##    #########
    ##        #########    ##    ##     ##
    ##        ##     ##    ##    ##     ##
    ##        ##     ##    ##    ##     ##

 */

path_t* path_new(const char *s)
{
	path_t *p;
	if (!s) { return NULL; }

	p = calloc(1, sizeof(path_t));
	if (!p) { return NULL; }

	p->n = p->len = strlen(s);
	p->buf = calloc(p->len+2, sizeof(char));
	if (!p->buf) {
		free(p);
		return NULL;
	}

	memcpy(p->buf, s, p->len);
	return p;
}

void path_free(path_t *p)
{
	if (!p) return;
	free(p->buf);
	free(p);
}

const char *path(path_t *p)
{
	return p->buf;
}

int path_canon(path_t *p)
{
	char *s, *a, *b, *end;
	if (p->len == 0) { return 0; }

	s = p->buf;
	end = s + p->len;
	for (b = s+1; b <= end; b++) {
		if (b == end || *b == '/') {
			for (a = b-1; a != s && *a != '/'; a--)
				;
			if (b-a == 3 && memcmp(a, "/..", b-a) == 0) {
				for (a--; a >= s && *a != '/'; a--)
						;
				if (a <= s) a = s; // avoid underflow
				memset(a, 0, b-a);
			} else if (b-a == 2 && memcmp(a, "/.", b-a) == 0) {
				memset(a, 0, b-a);
			}
		}
	}

	for (a = b = s; a != end; a++) {
		if (*a) { *b++ = *a; }
	}
	if (b == s) {
		*b++ = '/';
	} else if (*(b-1) == '/') {
		*b-- = '\0';
	}
	*b = '\0';

	p->n = p->len = strlen(s);
	if (p->len == 0) {
		*s = '/';
		p->n = p->len = 1;
	}
	return 0;
}

int path_push(path_t *p)
{
	char *s = p->buf;
	while (*(++s))
		;
	if (*(s+1)) { *s = '/'; }
	p->n = strlen(s);
	return *s;
}

int path_pop(path_t *p)
{
	char *s;
	for (s = p->buf + p->n; s > p->buf; s--, p->n--) {
		if (*s == '/') {
			*s = '\0';
			return 1;
		}
	}
	return 0;
}
