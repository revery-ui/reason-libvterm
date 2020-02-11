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

CAMLprim value reason_libvterm_vterm_get_size(value vTerm) {
	CAMLparam1(vTerm);
	CAMLlocal1(ret);
	VTerm *pTerm = (VTerm*)vTerm;
	int rows, cols;
	vterm_get_size(pTerm, &rows, &cols);

	ret = caml_alloc(2, 0);
	Store_field(ret, 0, Val_int(rows));
	Store_field(ret, 1, Val_int(cols));

	CAMLreturn(ret);
}

CAMLprim value reason_libvterm_vterm_set_size(value vTerm, value vSize) {
	CAMLparam2(vTerm, vSize);
	VTerm *pTerm = (VTerm*)vTerm;
	int rows = Int_val(Field(vSize, 0));
	int cols = Int_val(Field(vSize, 1));
	vterm_set_size(pTerm, rows, cols);
	CAMLreturn(Val_unit);
}
