with import <nixpkgs> {};
stdenv.mkDerivation {
  src = ./.;
  name = "ff-glitch";
  enableParallelBuilding = true;

  installPhase = ''
      make install PREFIX=$out
    '';
}
