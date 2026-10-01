ACLOCAL_AMFLAGS = -I m4
ZZZ = 0
SUBDIRS = src test
pkgconfig_DATA = libmtx.pc
mtxlibdir = @mtxlibdir@

# Generate in the build tree, even when the installed directory is read-only.
if BUILD_TABLES
all-local: tables.stamp

tables.stamp: src/mtx-gentables
	$(MKDIR_P) tables
	MTXLIB="$(abs_builddir)/tables" ./src/mtx-gentables
	touch $@

install-data-local: tables.stamp
	$(MKDIR_P) "$(DESTDIR)$(mtxlibdir)"
	$(INSTALL_DATA) tables/p*.zzz "$(DESTDIR)$(mtxlibdir)"

uninstall-local:
	$(RM) "$(DESTDIR)$(mtxlibdir)"/p*.zzz
endif

clean-local:
	$(RM) tables.stamp
	$(RM) -r tables

EXTRA_DIST = etc/Doxyfile etc/meataxe-footer.html \
	test/t.config test/data/a5reg.1  test/data/ac.2 test/data/m11.1 \
	test/data/Mat2 test/data/Mat5 test/data/Perm1 test/data/Ru-K378.2 \
	test/data/Ru-M378.1 test/data/Th-m.2 test/data/Th-r.2 test/data/a5reg.2 \
	test/data/C0.1 test/data/m11.2 test/data/Mat25 test/data/Mat67 test/data/Perm2 \
	test/data/Ru-K378.3 test/data/Ru-M378.2 test/data/Th-m.3 test/data/ac.1 \
	test/data/C0.2 test/data/Mat125 test/data/Mat256 test/data/Mat9 \
	test/data/Ru-K378.1 test/data/Ru-K378.4 test/data/Th-m.1 test/data/Th-r.1 \
	test/t-0000  test/t-0105  test/t-0107  test/t-0109  test/t-0111  test/t-0113 \
	test/t-0115  test/t-0117  test/t-0200  test/t-0210  test/t-0212  test/t-0214 \
	test/t-0100  test/t-0106  test/t-0108  test/t-0110  test/t-0112  test/t-0114 \
	test/t-0116  test/t-0118  test/t-0201  test/t-0211  test/t-0213  test/t-0215

@DX_RULES@

.PHONY: doc

doc: $(docdir) doxygen-doc

$(docdir):
	@echo "Creating" $(docdir)
	@$(MKDIR_P) "$(docdir)"

uninstall-hook:
	@$(RM) -r $(docdir)
