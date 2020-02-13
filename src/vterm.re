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
  module CursorShape = {
    type t =
    | Block
    | Underline
    | BarLeft;
  }

  module Mouse = {
    type t =
    | None
    | Click
    | Drag
    | Move;
  }

  type t =
    | None
    | CursorVisible(bool)
    | CursorBlink(bool)
    | AltScreen(bool)
    | Title(string)
    | IconName(string)
    | Reverse(bool)
    | CursorShape(CursorShape.t)
    | Mouse(Mouse.t);
};

module Color = {
  type t =
    | DefaultForeground
    | DefaultBackground
    | Rgb(int, int, int)
    | Index(int);

  let show = fun
  | DefaultForeground => "DefaultForeground"
  | DefaultBackground => "DefaultBackground"
  | Rgb(r, g, b) => Printf.sprintf("rgb(%d, %d, %d)", r, g, b)
  | Index(idx) => Printf.sprintf("index(%d)", idx);
};

module ScreenCell = {
  type t = {
    chars: string,
    width: int,
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

  let empty: t = {
    chars: "",
    width: 0,
    fg: Color.DefaultForeground,
    bg: Color.DefaultBackground,
    bold: 0,
    underline: 0,
    italic: 0,
    blink: 0,
    reverse: 0,
    conceal: 0,
    strike: 0,
  }
};

type callbacks = {
  onTermOutput: ref(string => unit),
  onScreenDamage: ref(Rect.t => unit),
  onScreenMoveRect: ref((Rect.t, Rect.t) => unit),
  onScreenMoveCursor: ref((Pos.t, Pos.t, bool) => unit),
  onScreenSetTermProp: ref((TermProp.t) => unit),
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

  external screen_get_cell: (terminal, int, int) => ScreenCell.t = "reason_libvterm_vterm_screen_get_cell";

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

  let onScreenDamage = (id: int, rect: Rect.t) => {
    switch (Hashtbl.find_opt(idToOutputCallback, id)) {
    | Some({onScreenDamage, _}) => onScreenDamage^(rect)
    | None => ()
    };
  };

  Callback.register("reason_libvterm_onOutput", onOutput);
  Callback.register("reason_libvterm_onScreenBell", onScreenBell);
  Callback.register("reason_libvterm_onScreenResize", onScreenResize);
  Callback.register("reason_libvterm_onScreenDamage", onScreenDamage);
};

module Screen = {
  let setBellCallback = (~onBell, terminal) => {
    terminal.callbacks.onScreenBell := onBell;
  };

  let setResizeCallback = (~onResize, terminal) => {
    terminal.callbacks.onScreenResize := onResize;
  };

  let setDamageCallback = (~onDamage, terminal) => {
    terminal.callbacks.onScreenDamage := onDamage;
  };

  let getCell = (~row, ~col, {terminal, _}) =>  {
    Internal.screen_get_cell(terminal, row, col); 
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
  let onScreenSetTermProp = ref((_) => ());
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

let setOutputCallback = (~onOutput, terminal) => {
  terminal.callbacks.onTermOutput := onOutput;
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
