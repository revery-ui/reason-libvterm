open Vterm;
open TestFramework;


describe("VTerm", ({describe, test}) => {
  test("make", ({expect}) => {
    let isFinalized = ref(false);
    let vterm = make(~rows=20, ~cols=20);
    
    Gc.finalise((_) => isFinalized := true, vterm);
    Gc.full_major();

    expect.equal(isFinalized^, true);
  })
});
