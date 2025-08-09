{ pkgs }: {
	deps = [
   pkgs.tinycc
   pkgs.ed
   pkgs.texinfoInteractive
		pkgs.clang
		pkgs.ccls
		pkgs.gdb
		pkgs.gnumake
	];
}