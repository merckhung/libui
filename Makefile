# CPPLINT
CPPLINT        := tools/cpplint.py
CPPLINT_FILTER := --filter=-whitespace/line_length,-build/include,-readability/function,-readability/streams,-readability/todo,-runtime/references,-runtime/sizeof,-runtime/threadsafe_fn,-runtime/printf
CPPLINT_SRC    := $(shell find ./ -name *.h -o -name *.cc | grep -v third_party)

all:
	@mkdir out
	@cd out; cmake ..; make

clean:
	@rm -rf out
	
cpplint:
	@$(CPPLINT) $(CPPLINT_FILTER) $(CPPLINT_SRC)
