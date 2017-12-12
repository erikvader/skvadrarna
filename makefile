

.PHONY: format
format:
	find ./src -name *.c | xargs -d'\n' -n1 astyle -n --options=astylerc


