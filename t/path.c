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

#include "test.h"

static void is_canonical(const char *orig_path, const char *expected)
{
	path_t *p;
	isnt_null(p = path_new(orig_path), "path_new succeeds");
	ok(path_canon(p) == 0, "path_canon returns 0");
	is_string(path(p), expected, orig_path);
	path_free(p);
}

TESTS {
	alarm(5);
	subtest {
		path_t *p = NULL;
		path_free(p);
		pass("path_free(NULL) doesn't segfault");
	}

	subtest {
		path_t *p;

		is_null(p = path_new(NULL), "NULL string = NULL path");
		isnt_null(p = path_new("/etc/test"), "created new path");
		is_string(path(p), "/etc/test", "path buffer");

		path_free(p);
	}

	subtest {
		is_canonical("/usr/local/sbin", "/usr/local/sbin");
		is_canonical("", "");

		is_canonical("/usr/./local/sbin", "/usr/local/sbin");
		is_canonical("/usr/./././local/./sbin", "/usr/local/sbin");

		is_canonical("/usr/local/opt/../sbin", "/usr/local/sbin");
		is_canonical("/usr/share/man/../../root/../local/opt/../sbin", "/usr/local/sbin");

		is_canonical("/usr/local/sbin/.", "/usr/local/sbin");
		is_canonical("/usr/local/sbin/././.", "/usr/local/sbin");

		is_canonical("/usr/local/sbin/./", "/usr/local/sbin");
		is_canonical("/usr/local/sbin/./././", "/usr/local/sbin");

		is_canonical("/usr/local/sbin/..", "/usr/local");
		is_canonical("/usr/local/sbin/../..", "/usr");

		is_canonical("/usr/local/sbin/../", "/usr/local");
		is_canonical("/usr/local/sbin/../../", "/usr");

		is_canonical("/usr/local/sbin/../../../etc", "/etc");
		is_canonical("/etc/../../../", "/");

		is_canonical("/usr/local/sbin/../../../", "/");
		is_canonical("/usr/local/sbin/../../..",  "/");

		is_canonical("/././././", "/");

		is_canonical("/etc/sysconfig/../.././etc/././../usr/local/./bin/../sbin", "/usr/local/sbin");

		is_canonical("/a/b/c/d/e/f/../", "/a/b/c/d/e");
	}

	subtest {
		path_t *p;

		isnt_null(p = path_new("/path/tothe/thing/youwanted"), "got a path");

		ok(path_pop(p) != 0, "pop(1) returns non-zero (more to go)");
		is_string(path(p), "/path/tothe/thing", "popped last component");

		ok(path_pop(p) != 0, "pop(2) returns non-zero (more to go)");
		is_string(path(p), "/path/tothe", "popped last component");

		ok(path_pop(p) != 0, "pop(3) returns non-zero (more to go)");
		is_string(path(p), "/path", "popped last component");

		ok(path_pop(p) == 0, "pop(4) finished it out");


		ok(path_push(p) != 0, "push(1) returns non-zero (more to go)");
		is_string(path(p), "/path/tothe", "pushed last component");

		ok(path_push(p) != 0, "push(2) returns non-zero (more to go)");
		is_string(path(p), "/path/tothe/thing", "pushed last component");

		ok(path_push(p) != 0, "push(3) returns non-zero (more to go)");
		is_string(path(p), "/path/tothe/thing/youwanted", "pushed last component");

		ok(path_push(p) == 0, "push(4) finished it out");

		path_free(p);
	}

	alarm(0);
	done_testing();
}
