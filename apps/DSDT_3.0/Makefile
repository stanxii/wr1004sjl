include switch/tools/setenv

MVL_QD_DIR := $(DSDT_USER_BASE)


all:	SW


SW:
	mkdir -p $(MVL_QD_DIR)/Library
	make -C $(MVL_QD_DIR)/src;

clean :
	mkdir -p $(MVL_QD_DIR)/Library
	make -C $(MVL_QD_DIR)/src clean
distclean:
	rm -rf $(MVL_QD_DIR)/Library
