type terminal;

type t = {terminal};

module Internal = {
  external newVterm: (int, int) => terminal = "reason_libvterm_vterm_new";
  external freeVterm: terminal => unit = "reason_libvterm_vterm_free";
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
