{
  description = "Extension of C standard library.";

  inputs = {
    flake-utils.url = "github:numtide/flake-utils";
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs, flake-utils, ... }@inputs:
    let
      outputsWithoutSystem = { };
      outputsWithSystem = flake-utils.lib.eachDefaultSystem
        (system:
          let
            pkgs = import nixpkgs {
              inherit system;
            };
            lib = pkgs.lib;
          in
          {
            devShells = {
              default = pkgs.mkShell rec {
                buildInputs = with pkgs; [
                  clang-tools
                  clang
                  check
                  pkg-config
                ];

                LD_LIBRARY_PATH = "${lib.makeLibraryPath buildInputs}";
              };
            };
          });
    in
    outputsWithSystem // outputsWithoutSystem;
}
