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

static value 
reason_libvterm_Val_color(VTermScreen *pScreen, VTermColor *pColor) {
	CAMLparam0();
	CAMLlocal1(ret);

	int tag;

	vterm_screen_convert_color_to_rgb(pScreen, pColor);

	if (VTERM_COLOR_IS_DEFAULT_FG(pColor)) {
		ret = Int_val(0);
	} else if (VTERM_COLOR_IS_DEFAULT_BG(pColor)) {
		ret = Int_val(1);
	}
	if (VTERM_COLOR_IS_RGB(pColor)) {
		ret = caml_alloc(3, 0);
		Store_field(ret, 0, Val_int(pColor->rgb.red));
		Store_field(ret, 1, Val_int(pColor->rgb.green));
		Store_field(ret, 2, Val_int(pColor->rgb.blue));
	} else {
		ret = caml_alloc(1, 1);
		Store_field(ret, 0, Val_int(pColor->indexed.idx));
	}

	CAMLreturn(ret);
}

static value 
reason_libvterm_Val_screencell(VTermScreen* pScreen, VTermScreenCell *pScreenCell) {
	CAMLparam0();
	CAMLlocal2(ret, str);


	str = caml_alloc_string(VTERM_MAX_CHARS_PER_CELL);
	memcpy(String_val(str), pScreenCell->chars, VTERM_MAX_CHARS_PER_CELL);
	ret = caml_alloc(11, 0);
	Store_field(ret, 0, str);
	Store_field(ret, 1, Val_int(pScreenCell->width));
	Store_field(ret, 2, reason_libvterm_Val_color(pScreen, &pScreenCell->fg));
	Store_field(ret, 3, reason_libvterm_Val_color(pScreen, &pScreenCell->bg));
	Store_field(ret, 4, Val_int(pScreenCell->attrs.bold));
	Store_field(ret, 5, Val_int(pScreenCell->attrs.underline));
	Store_field(ret, 6, Val_int(pScreenCell->attrs.italic));
	Store_field(ret, 7, Val_int(pScreenCell->attrs.blink));
	Store_field(ret, 8, Val_int(pScreenCell->attrs.reverse));
	Store_field(ret, 9, Val_int(pScreenCell->attrs.conceal));
	Store_field(ret, 10, Val_int(pScreenCell->attrs.strike));

	CAMLreturn(ret);
}

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

int reason_libvterm_onScreenMoveRectF(VTermRect dest, VTermRect src, void *user) {
	CAMLparam0();

	printf("!!!!\n MOVERECT\n !!!!\n");

	CAMLreturn(0);
}

int reason_libvterm_onScreenMoveCursorF(VTermPos pos, VTermPos oldPos, int visible, void *user) {
	CAMLparam0();

	printf("!!!!\n MOVECURSOR\n !!!!\n");

	CAMLreturn(0);
}

int reason_libvterm_onScreenSbPushLineF(int cols, const VTermScreenCell *cells, void *user) {
	CAMLparam0();

	printf("!!!!\n SB_PUSHLINE\n !!!!\n");

	CAMLreturn(0);
}

int reason_libvterm_onScreenSbPopLineF(int cols, VTermScreenCell *cells, void *user) {
	CAMLparam0();

	printf("!!!!\n SB_POPLINE\n !!!!\n");

	CAMLreturn(0);
}

int reason_libvterm_onScreenResizeF(int rows, int cols, void *user) {
	CAMLparam0();

	static value *reason_libvterm_onScreenResize = NULL;

	if (reason_libvterm_onScreenResize == NULL) {
		reason_libvterm_onScreenResize = (value *)caml_named_value("reason_libvterm_onScreenResize");
	}
	
	caml_callback3(*reason_libvterm_onScreenResize, Val_int(user), Val_int(rows), Val_int(cols));
	CAMLreturn(0);
}

int reason_libvterm_onScreenDamageF(VTermRect rect, void* user) {
	CAMLparam0();
	CAMLlocal1(outRect);

	outRect = caml_alloc(4, 0);
	Store_field(outRect, 0, Val_int(rect.start_row));
	Store_field(outRect, 1, Val_int(rect.end_row));
	Store_field(outRect, 2, Val_int(rect.start_col));
	Store_field(outRect, 3, Val_int(rect.end_col));
	static value *reason_libvterm_onScreenDamage = NULL;

	if (reason_libvterm_onScreenDamage == NULL) {
		reason_libvterm_onScreenDamage = (value *)caml_named_value("reason_libvterm_onScreenDamage");
	}
	
	caml_callback2(*reason_libvterm_onScreenDamage, Val_int(user), outRect);
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

CAMLprim value reason_libvterm_vterm_screen_get_cell(value vTerm, value vRow, value vCol) {
	CAMLparam3(vTerm, vRow, vCol);
	CAMLlocal1(ret);

	int row = Int_val(vRow);
	int col = Int_val(vCol);
	VTerm *pTerm = (VTerm*)vTerm;
	VTermScreen* pScreen = vterm_obtain_screen(pTerm);

	VTermPos pos;
	pos.row = row;
	pos.col = col;

	VTermScreenCell cell;
	vterm_screen_get_cell(pScreen, pos, &cell);

	ret = reason_libvterm_Val_screencell(pScreen, &cell);


	CAMLreturn(ret);
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
	.damage = &reason_libvterm_onScreenDamageF,
	.moverect = &reason_libvterm_onScreenMoveRectF,
	.movecursor = &reason_libvterm_onScreenMoveCursorF,
	.sb_pushline = &reason_libvterm_onScreenSbPushLineF,
	.sb_popline = &reason_libvterm_onScreenSbPopLineF,
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
