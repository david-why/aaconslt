# ----------------------------
# Makefile Options
# ----------------------------

NAME := AACONSLT
DESCRIPTION := "A console"
COMPRESSED := NO
ARCHIVED := YES

HAS_PRINTF := NO

CFLAGS := -Wall -Wextra -Oz -DLINKLIB_KEY="\"AACONSLE_LINK\""
CXXFLAGS := -Wall -Wextra -Oz

# ----------------------------

ifndef CEDEV
$(error CEDEV environment path variable is not set)
endif

include $(CEDEV)/meta/makefile.mk
