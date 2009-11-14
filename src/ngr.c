/*
 * Copyright (c) 2009 Artur Bergman <sky@crucially.net>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "NGR.h"
#include <sys/time.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>

#include <dlfcn.h>

typedef int (*func_t)();


int usage (char * message, ...) {
  va_list ap;
  va_start(ap, message);
  char *string;

  if (message) {
	if (string=va_arg(ap, char *)) {
		WARN_FMT(message, string);
	} else {
		WARN(message);
	}
  } 
  WARN("Usage:");
  WARN("ngr <command> ");
  WARN("ngr help <command> ");
  WARN("Available commands: ");
  WARN("    info");
  WARN("    dump");
  WARN("    insert");
  WARN("    create");
  WARN("    agg");
  return 1;
}

int main(int argc, char **argv) {
  char ** args;
  if (argc<=1) {
	return usage("You must pass in a command\n");
  }	

  void* handle = dlopen(argv[0], RTLD_LAZY);
  if (!handle) {
	WARN_FMT("Couldn't load handle: %s\nSomething has gone seriously wrong.", dlerror());
	return 1;
  }

  char* func_name;

  /* skip the arguments forward */
  argv++;
  argc--;
 
  int ret = 0;
  if (strneq("help", argv[0], 4)) {
    int  length = 7+strlen(argv[1]);
	func_name   = malloc(length);
	snprintf(func_name, length, "%s_usage", argv[1]);
    func_t f  = (func_t) dlsym(handle, func_name);
    if (!f) {
		ret = usage("Couldn't find help for '%s'\n", argv[1]);
    } else {
		WARN_FMT("Usage: ngr %s [options]\n", argv[1]);
	    ret = f();
	}
  } else {
	int length  = 6+strlen(argv[0]);
	func_name   = malloc(length);
	snprintf(func_name, length, "%s_main", argv[0]); 
    func_t f  = (func_t) dlsym(handle, func_name);
    if (!f) {
		ret = usage("Couldn't find command '%s'\n", argv[0]);
    } else {
    	ret = f(argc, argv);	
	}
  }
  

  dlclose(handle);

  return ret;
}
