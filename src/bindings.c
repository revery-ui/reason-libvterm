#include <stdio.h>
#include <string.h>

#include <caml/alloc.h>
#include <caml/bigarray.h>
#include <caml/callback.h>
#include <caml/fail.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
#include <caml/threads.h>

#include <vterm.h>

void reason_libvterm_onOutputF(const char *s, size_t len, void *user) {
	CAMLparam0();
	CAMLlocal1(ret);

	static value *reason_libvterm_onOutput = NULL;

	if (reason_libvterm_onOutput == NULL) {
		reason_libvterm_onOutput = (value *)caml_named_value("reason_libvterm_onOutput");
	}
	

	ret = caml_alloc_string(len);
	memcpy(String_val(ret), s, len);
	
	caml_callback2(*reason_libvterm_onOutput, Val_int(user), ret);

	CAMLreturn0;
}

int reason_libvterm_onScreenBellF(void *user) {
	CAMLparam0();

	static value *reason_libvterm_onScreenBell = NULL;

	if (reason_libvterm_onScreenBell == NULL) {
		reason_libvterm_onScreenBell = (value *)caml_named_value("reason_libvterm_onScreenBell");
	}
	
	caml_callback(*reason_libvterm_onScreenBell, Val_int(user));

	CAMLreturn(0);
}

int reason_libvterm_onScreenResizeF(int rows, int cols, void *user) {
	CAMLparam0();

	// TODO
	CAMLreturn(0);
}

int VTermMod_val(value vMod) {
	switch (Int_val(vMod)) {
		case 0:
			return VTERM_MOD_NONE;
		case 1:
			return VTERM_MOD_SHIFT;
		case 2:
			return VTERM_MOD_ALT;
		case 3:
			return VTERM_MOD_CTRL;
		case 4:
			return VTERM_ALL_MODS_MASK;
		default:
			return VTERM_MOD_NONE;
	}
}

CAMLprim value reason_libvterm_vterm_keyboard_unichar(value vTerm, value vChar, value vMod) {
	CAMLparam3(vTerm, vChar, vMod);

	VTerm *pTerm = (VTerm*)vTerm;
	uint32_t c = Int32_val(vChar);
	VTermModifier mod = VTermMod_val(vMod);
	vterm_keyboard_unichar(pTerm, c, mod);


	CAMLreturn(Val_unit);
}
static VTermScreenCallbacks reason_libvterm_screen_callbacks = {
	.bell = &reason_libvterm_onScreenBellF,
	.resize = &reason_libvterm_onScreenResizeF,
};

CAMLprim value reason_libvterm_vterm_new(value vId, value vRows, value vCol) {
	CAMLparam3(vId, vRows, vCol);
	void* id = (void *)Int_val(vId);
	int rows = Int_val(vRows);
	int cols = Int_val(vCol);
	VTerm* pTerm = vterm_new(rows, cols);
	vterm_set_utf8(pTerm, true);
	vterm_output_set_callback(pTerm, &reason_libvterm_onOutputF, id);
	VTermScreen* pScreen = vterm_obtain_screen(pTerm);
	vterm_screen_set_callbacks(pScreen, &reason_libvterm_screen_callbacks, id);
	vterm_screen_reset(pScreen, 1);
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

CAMLprim value reason_libvterm_vterm_input_write(value vTerm, value vStr) {
	CAMLparam2(vTerm, vStr);
	VTerm *pTerm = (VTerm*)vTerm;
	int len = caml_string_length(vStr);
	char *bytes = String_val(vStr);
	int ret = vterm_input_write(pTerm, bytes, len);
	CAMLreturn(Val_int(ret));
}
