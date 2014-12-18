let
  pkgs = import <nixpkgs> {};
in
{ stdenv ? pkgs.stdenv
, glib ? pkgs.glib
, pkgconfig ? pkgs.pkgconfig
, readline ? pkgs.readline
}:

stdenv.mkDerivation {
  name = "extendp" ;
  src = ./. ;
  buildInputs = [ pkgconfig glib readline ];
  meta = {
    description = "A pipe wildcard" ;
    homepage = "https://github.com/jkarni/extendp" ;
  };
}
