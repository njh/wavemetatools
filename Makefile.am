AUTOMAKE_OPTIONS = foreign

SUBDIRS = src

EXTRA_DIST = bootstrap.sh
# Copy README.md to README when building distribution
dist-hook:
	[ -f README.md ] && cat README.md > README || true
	[ -f NEWS.md ] && cat NEWS.md > NEWS || true
