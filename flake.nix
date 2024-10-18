{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      nixpkgs,
      flake-utils,
      ...
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      with pkgs;
      {
        devShells.default = mkShell {
          buildInputs = [
            gcc
            ccls
            clang-tools
            libcxxStdenv
            llvmPackages_latest.libcxxStdenv
            llvmPackages_latest.libcxxClang
            llvmPackages_latest.llvm
            llvmPackages_latest.bintools
            cmake
            glfw
            emscripten
            just
          ];
        };
      }
    );
}
