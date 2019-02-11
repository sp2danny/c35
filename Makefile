.PHONY: clean All

All:
	@echo "----------Building project:[ c35 - Debug ]----------"
	@"$(MAKE)" -f  "c35.mk"
clean:
	@echo "----------Cleaning project:[ c35 - Debug ]----------"
	@"$(MAKE)" -f  "c35.mk" clean
