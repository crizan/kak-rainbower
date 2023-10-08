{
  description = "Description for the project";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs";
    flake-compat.flake = false;
    flake-compat.url = "github:edolstra/flake-compat";

  };

  outputs = inputs@{ self, nixpkgs, flake-parts, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {

      systems = [
        "x86_64-linux"
      ];

      imports = [
        inputs.flake-parts.flakeModules.easyOverlay
      ];


      perSystem = { config, self', inputs', pkgs, system, ... }:
        let
          rainbowerPackage = pkgs.stdenv.mkDerivation {
            pname = "rainbower";
            version = "0.1.0";

            # The source file.
            # This means the current directory.
            src = ./.;
            buildInputs = with pkgs; [
              cmake
            ];
            # configurePhase = ''
            #   		cmake .
            # '';
            # buildPhase = ''
            #   	make .
            # '';

            installPhase = ''
              mkdir -p $out/bin

              mv rainbower $out/bin/rainbower
            '';
          };
        in
        rec
        {

          # This is the overlay for the packages available.
          # Thank god flake.parts exists.
          overlayAttrs = {
            inherit (config.packages) rainbower;
          };


          packages = {
            rainbower = rainbowerPackage;
          };
          packages.default = packages.rainbower;

          devShells = {
            rainbower = pkgs.mkShell {
              packages = with pkgs; [
                cmake
                ninja
              ];
            };

          };

          devShells.default = devShells.rainbower;

        };

    };
}
