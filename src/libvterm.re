module VTerm = {
	type t;

	module Internal = {
		external newVterm: (int, int) => t = "reason_libvterm_vterm_new";
		external freeVterm: (t) => unit = "reason_libvterm_vterm_free";
	}

	let make = (~rows, ~cols) => {
		let term = Internal.newVterm(rows, cols);
		let () = Gc.finalise((termToDispose) => {
			Internal.freeVterm(termToDispose);
		}, term);
		term;
	};
}
