edit=AuthenRequest client encrpty rconf requestMessage wlog

all: clean ts
ts:$(edit:%=%.o)
	cc -o ts $(edit:%=%.o)

$(edit:%=%.o):%.o:%.c %.h

.PHONY:clean
clean:
	-rm ts $(edit:%=%.o) *.log
