type terminal;

type screen;

type modifier =
  | None
  | Shift
  | Alt
  | Control
  | All;

type size = {
  rows: int,
  cols: int,
};

module Rect = {
  type t = {
    startRow: int,
    endRow: int,
    startCol: int,
    endCol: int,
  };
};

module Pos = {
  type t = {
    row: int,
    col: int,
  };
};

module TermProp = {
  type t =
    | None
    | CursorVisible
    | CursorBlink
    | AltScreen
    | Title
    | IconName
    | Reverse
    | CursorShape
    | Mouse;
};

module Color = {
  type t =
    | Rgb(int, int, int)
    | Index(int);
};

module TermValue = {
  type t =
    | Bool(bool)
    | Int(int)
    | String(string)
    | Color;
};

module ScreenCell = {
  type t = {
    chars: string,
    fg: Color.t,
    bg: Color.t,
    // Attributes
    bold: int,
    underline: int,
    italic: int,
    blink: int,
    reverse: int,
    conceal: int,
    strike: int,
    // TODO:
    //font: int,
    //dwl: int,
    //dhl: int,
  };
};

type callbacks = {
  onTermOutput: ref(string => unit),
  onScreenDamage: ref(Rect.t => unit),
  onScreenMoveRect: ref((Rect.t, Rect.t) => unit),
  onScreenMoveCursor: ref((Pos.t, Pos.t, bool) => unit),
  onScreenSetTermProp: ref((TermProp.t, TermValue.t) => unit),
  onScreenBell: ref(unit => unit),
  onScreenResize: ref((int, int) => unit),
  onScreenScrollbackPushLine: ref(array(ScreenCell.t) => unit),
  onScreenScrollbackPopLine: ref(array(ScreenCell.t) => unit),
};

type t = {
  uniqueId: int,
  terminal,
  callbacks,
};

let idToOutputCallback: Hashtbl.t(int, callbacks) = Hashtbl.create(8);

module Internal = {
  let uniqueId = ref(0);

  external newVterm: (int, int, int) => terminal = "reason_libvterm_vterm_new";
  external freeVterm: terminal => unit = "reason_libvterm_vterm_free";
  external set_utf8: (terminal, bool) => unit =
    "reason_libvterm_vterm_set_utf8";
  external get_utf8: terminal => bool = "reason_libvterm_vterm_get_utf8";
  external get_size: terminal => size = "reason_libvterm_vterm_get_size";
  external set_size: (terminal, size) => unit =
    "reason_libvterm_vterm_set_size";
  external input_write: (terminal, string) => int =
    "reason_libvterm_vterm_input_write";

  external keyboard_unichar: (terminal, Int32.t, modifier) => unit =
    "reason_libvterm_vterm_keyboard_unichar";

  let onOutput = (id: int, output: string) => {
    switch (Hashtbl.find_opt(idToOutputCallback, id)) {
    | Some({onTermOutput, _}) => onTermOutput^(output)
    | None => ()
    };
  };

  let onScreenBell = (id: int) => {
    switch (Hashtbl.find_opt(idToOutputCallback, id)) {
    | Some({onScreenBell, _}) => onScreenBell^()
    | None => ()
    };
  };

  let onScreenResize = (id: int, rows: int, cols: int) => {
    switch (Hashtbl.find_opt(idToOutputCallback, id)) {
    | Some({onScreenResize, _}) => onScreenResize^(rows, cols)
    | None => ()
    };
  };

  Callback.register("reason_libvterm_onOutput", onOutput);
  Callback.register("reason_libvterm_onScreenBell", onScreenBell);
  Callback.register("reason_libvterm_onScreenResize", onScreenResize);
};

module Screen = {
  let setBellCallback = (~onBell, terminal) => {
    terminal.callbacks.onScreenBell := onBell;
  };

  let setResizeCallback = (~onResize, terminal) => {
    terminal.callbacks.onScreenResize := onResize;
  };
};

module Keyboard = {
  let unichar = ({terminal, _}, key: Int32.t, mods: modifier) => {
    Internal.keyboard_unichar(terminal, key, mods);
  };
};

let make = (~rows, ~cols) => {
  incr(Internal.uniqueId);
  let uniqueId = Internal.uniqueId^;
  let terminal = Internal.newVterm(uniqueId, rows, cols);
  let onTermOutput = ref(_ => ());
  let onScreenDamage = ref(_ => ());
  let onScreenMoveRect = ref((_, _) => ());
  let onScreenMoveCursor = ref((_, _, _) => ());
  let onScreenBell = ref(() => ());
  let onScreenResize = ref((_, _) => ());
  let onScreenSetTermProp = ref((_, _) => ());
  let onScreenScrollbackPushLine = ref(_ => ());
  let onScreenScrollbackPopLine = ref(_ => ());
  let callbacks = {
    onTermOutput,
    onScreenDamage,
    onScreenMoveRect,
    onScreenMoveCursor,
    onScreenSetTermProp,
    onScreenBell,
    onScreenResize,
    onScreenScrollbackPushLine,
    onScreenScrollbackPopLine,
  };
  let wrappedTerminal: t = {terminal, uniqueId, callbacks};
  Hashtbl.add(idToOutputCallback, uniqueId, callbacks);
  let () =
    Gc.finalise(
      ({terminal, uniqueId, _}: t) => {
        Internal.freeVterm(terminal);
        Hashtbl.remove(idToOutputCallback, uniqueId);
      },
      wrappedTerminal,
    );
  wrappedTerminal;
};

let setOutputCallback = (~output, terminal) => {
  terminal.callbacks.onTermOutput := output;
};

let setUtf8 = (~utf8, {terminal, _}) => {
  Internal.set_utf8(terminal, utf8);
};

let getUtf8 = ({terminal, _}) => {
  Internal.get_utf8(terminal);
};

let setSize = (~size, {terminal, _}) => {
  Internal.set_size(terminal, size);
};

let getSize = ({terminal, _}) => {
  Internal.get_size(terminal);
};

let write = (~input, {terminal, _}) => {
  Internal.input_write(terminal, input);
};
