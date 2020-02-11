type terminal;

type t = {terminal};

module Internal = {
  external newVterm: (int, int) => terminal = "reason_libvterm_vterm_new";
  external freeVterm: terminal => unit = "reason_libvterm_vterm_free";
  external set_utf8: (terminal, bool) => unit = "reason_libvterm_vterm_set_utf8";
  external get_utf8: (terminal) => bool = "reason_libvterm_vterm_get_utf8";
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
