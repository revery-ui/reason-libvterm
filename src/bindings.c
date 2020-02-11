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
	CAMLparam2(vRows, vCol);
	int rows = Int_val(vRows);
	int cols = Int_val(vCol);
	VTerm* pTerm = vterm_new(rows, cols);
	CAMLreturn((value)pTerm);
}

CAMLprim value reason_libvterm_vterm_free(value vTerm) {
	CAMLparam1(vTerm);
	VTerm *pTerm = (VTerm*)vTerm;
	vterm_free(pTerm);
	CAMLreturn(Val_unit);
}

CAMLprim value reason_libvterm_vterm_set_utf8(value vTerm, value vUtf8) {
	CAMLparam2(vTerm, vUtf8);
	VTerm *pTerm = (VTerm*)vTerm;
	int isUtf8 = Bool_val(vUtf8);
	vterm_set_utf8(pTerm, isUtf8);
	CAMLreturn(Val_unit);
}

CAMLprim value reason_libvterm_vterm_get_utf8(value vTerm) {
	CAMLparam1(vTerm);
	VTerm *pTerm = (VTerm*)vTerm;
	int isUtf8 = vterm_get_utf8(pTerm);
	CAMLreturn(Val_int(isUtf8));
}
