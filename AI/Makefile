# Top-level Makefile for AI.TAZ package
# Part of AI.TAZ package for Minix

SUBDIRS = src/eliza src/ask_gemini

all clean install:
	@for dir in $(SUBDIRS); do \
		echo "Making $@ in $$dir..."; \
		$(MAKE) -C $$dir $@ || exit 1; \
	done

.PHONY: all clean install

