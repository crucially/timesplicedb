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
int dispatch(void* handle, char* command, ...);


int usage (const char * message, ...) {
  va_list ap;
  va_start(ap, message);
  char *string;

  if (message) {
	if (string=va_arg(ap, char *)) {
		WARN(message, string);
	} else {
		WARN(message);
	}
  } 
  WARN("Usage:");
  WARN("ngr <command>\n");
  WARN("ngr help <command>\n");
  WARN("Available commands:\n");
  WARN("    info\n");
  WARN("    dump\n");
  WARN("    insert\n");
  WARN("    create\n");
  WARN("    agg\n");
  return 1;
}

int main(int argc, char **argv) {
  char ** args;
  if (argc<=1) {
	return usage("You must pass in a command\n");
  }	

  void* handle = dlopen(NULL, RTLD_LAZY);
  if (!handle) {
	WARN("Couldn't load handle: %s\nSomething has gone seriously wrong.\n", dlerror());
	return 1;
  }



  /* skip the arguments forward */
  argv++;
  argc--;
 
  int ret = 0;
  if (strneq("help", argv[0], 4)) {
	if (argc<2) {
		ret = usage(NULL);
	} else {
		ret = dispatch(handle, argv[1], 0);
	}
  } else {
    ret = dispatch(handle, argv[0], argc, argv);
  }
  
  if (handle)
	  dlclose(handle);

  return ret;
}

int dispatch(void* handle, char * command, ...) {
  va_list ap;
  va_start(ap, command);
  int argc = 0;
  char **argv;	
  char *func_name;	

  // if we're being passed argc and argv then we call <command>_main
  if ((argc=va_arg(ap, int)) && (argv=va_arg(ap, char **))) {
	int length = 6+strlen(command);
	func_name = malloc(length);
	snprintf(func_name, length, "%s_main", command);	
  } else {
  // otherwise we're calling <command>_usage
	int length = 7+strlen(command);
	func_name = malloc(length);
	snprintf(func_name, length, "%s_usage", command);
  }

  func_t f  = (func_t) dlsym(handle, func_name);
  if (!f) {
  	return usage("Couldn't find command '%s'\n", command);
  }

  if (argc) {
    return f(argc, argv);
  } else {
  	WARN("Usage: ngr %s [options]\n", command);
	return f();
  }
  return 0;
}
