#include <stdio.h>

#include <caml/alloc.h>
#include <caml/bigarray.h>
#include <caml/callback.h>
#include <caml/fail.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
#include <caml/threads.h>

#include <vterm.h>

CAMLprim value reason_libvterm_test() {
	CAMLparam0();
	printf("Hello, world");
	CAMLreturn(Val_unit);
}
