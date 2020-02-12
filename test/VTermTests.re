open Vterm;
open TestFramework;

describe("VTerm", ({describe, test}) => {
  test("make", ({expect}) => {
    let isFinalized = ref(false);
    let vterm = make(~rows=20, ~cols=20);

    Gc.finalise(_ => isFinalized := true, vterm);
    Gc.full_major();

    expect.equal(isFinalized^, true);
  });
  test("utf8", ({expect}) => {
    let vterm = make(~rows=20, ~cols=20);
    setUtf8(~utf8=true, vterm);
    expect.equal(getUtf8(vterm), true);
    setUtf8(~utf8=false, vterm);
    expect.equal(getUtf8(vterm), false);
  });
  test("size", ({expect}) => {
    let vterm = make(~rows=20, ~cols=30);
    let {rows, cols} = getSize(vterm);
    expect.equal(rows, 20);
    expect.equal(cols, 30);

    setSize(~size={rows: 10, cols: 15}, vterm);
    let {rows, cols} = getSize(vterm);
    expect.equal(rows, 10);
    expect.equal(cols, 15);
  });
  describe("input", ({test, _}) => {
    test("returns value", ({expect}) => {
      let vterm = make(~rows=20, ~cols=30);

      let res = write(~input="abc", vterm);
      expect.equal(res, 3);
    });

    test("beeps", ({expect}) => {
      let vterm = make(~rows=20, ~cols=30);

      let gotBell = ref(false);
      Screen.setBellCallback(~onBell=_ => gotBell := true, vterm);
      let _: int = write(~input=String.make(1, Char.chr(7)), vterm);
      expect.equal(true, gotBell^);
    });
    test("resize", ({expect}) => {
      let vterm = make(~rows=20, ~cols=30);

      let getRows = ref(0);
      let getCols = ref(0);
      Screen.setResizeCallback(
        ~onResize=
          (rows, cols) => {
            getRows := rows;
            getCols := cols;
          },
        vterm,
      );
      setSize(~size={rows: 5, cols: 6}, vterm);
      expect.equal(getRows^, 5);
      expect.equal(getCols^, 6);
    });
  });
  describe("output", ({test, _}) => {
    test("keyboard_unichar_test", ({expect}) => {
      let vterm = make(~rows=20, ~cols=30);

      let gotOutput = ref(false);
      setOutputCallback(~output=_ => gotOutput := true, vterm);
      let () = Keyboard.unichar(vterm, Int32.of_int(65), None);
      expect.equal(true, gotOutput^);
    })
  });
});
