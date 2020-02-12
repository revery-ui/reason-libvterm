type terminal;

type t = {terminal};

type size = {
  rows: int,
  cols: int,
};

module Internal = {
  external newVterm: (int, int) => terminal = "reason_libvterm_vterm_new";
  external freeVterm: terminal => unit = "reason_libvterm_vterm_free";
  external set_utf8: (terminal, bool) => unit = "reason_libvterm_vterm_set_utf8";
  external get_utf8: (terminal) => bool = "reason_libvterm_vterm_get_utf8";
  external get_size: (terminal) => size = "reason_libvterm_vterm_get_size";
  external set_size: (terminal, size) => unit = "reason_libvterm_vterm_set_size";
  external input_write: (terminal, string) => int = "reason_libvterm_vterm_input_write";
};

let make = (~rows, ~cols) => {
  let terminal = Internal.newVterm(rows, cols);
  let wrappedTerminal: t = {terminal: terminal};
  let () =
    Gc.finalise(
      ({terminal}: t) => {Internal.freeVterm(terminal)},
      wrappedTerminal,
    );
  wrappedTerminal;
};

let setUtf8 = (~utf8, {terminal}) => {
  Internal.set_utf8(terminal, utf8);
}

let getUtf8 = ({terminal}) => {
  Internal.get_utf8(terminal);
}

let setSize = (~size, {terminal}) => {
  Internal.set_size(terminal, size);
};

let getSize = ({terminal}) => {
  Internal.get_size(terminal);
};

let write = (~input, {terminal}) => {
  Internal.input_write(terminal, input); 
};
