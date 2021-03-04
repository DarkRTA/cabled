CC      := cc
CFLAGS  := -Isrc -Ideps -I. -MMD
LDFLAGS := -ljack

OBJ = $(patsubst %.c,obj/%.o,$(shell find src deps -type f -name *.c))
DEPS = $(patsubst %.o,%.d,$(OBJ))

.PHONY: clean all tags
.SUFFIXES:

S=@

all: cabled

clean: 
	@rm -rfv obj cabled

tags:
	@ctags --totals=yes --c-kinds=+defghlmpstuvxz -R \
		src/** \
		deps/**

obj/%.o: %.c obj/%.d
	@echo "CC   "$<
	@mkdir -p $(shell dirname $@)
	$S$(CC) -c -o $@ $< $(CFLAGS)

$(DEPS):

include $(DEPS)

cabled: $(OBJ)
	@echo "LINK "$@
	$S$(CC) -o $@ $(OBJ) $(LDFLAGS)
