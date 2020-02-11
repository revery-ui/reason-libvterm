#include <stdio.h>

#include <caml/alloc.h>
#include <caml/bigarray.h>
#include <caml/callback.h>
#include <caml/fail.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
#include <caml/threads.h>

#include <vterm.h>

CAMLprim value reason_libvterm_vterm_new(value vRows, value vCol) {
	CAMLparam0();
	int rows = Int_val(vRows);
	int cols = Int_val(vCol);
	VTerm* pTerm = vterm_new(rows, cols);
	CAMLreturn((value)pTerm);
}

CAMLprim value reason_libvterm_vterm_free(value vTerm) {
	CAMLparam0();
	VTerm *pTerm = (VTerm*)vTerm;
	vterm_free(pTerm);
	CAMLreturn(Val_unit);
}
