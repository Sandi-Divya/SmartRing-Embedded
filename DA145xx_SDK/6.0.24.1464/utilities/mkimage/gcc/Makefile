# /**
# ****************************************************************************************
# *
# * @file Makefile
# *
# * Copyright (C) 2017-2019 Renesas Electronics Corporation and/or its affiliates.
# * All rights reserved. Confidential Information.
# *
# * This software ("Software") is supplied by Renesas Electronics Corporation and/or its
# * affiliates ("Renesas"). Renesas grants you a personal, non-exclusive, non-transferable,
# * revocable, non-sub-licensable right and license to use the Software, solely if used in
# * or together with Renesas products. You may make copies of this Software, provided this
# * copyright notice and disclaimer ("Notice") is included in all such copies. Renesas
# * reserves the right to change or discontinue the Software at any time without notice.
# *
# * THE SOFTWARE IS PROVIDED "AS IS". RENESAS DISCLAIMS ALL WARRANTIES OF ANY KIND,
# * WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. TO THE
# * MAXIMUM EXTENT PERMITTED UNDER LAW, IN NO EVENT SHALL RENESAS BE LIABLE FOR ANY DIRECT,
# * INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING FROM, OUT OF OR IN
# * CONNECTION WITH THE SOFTWARE, EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY OF
# * SUCH DAMAGES. USE OF THIS SOFTWARE MAY BE SUBJECT TO TERMS AND CONDITIONS CONTAINED IN
# * AN ADDITIONAL AGREEMENT BETWEEN YOU AND RENESAS. IN CASE OF CONFLICT BETWEEN THE TERMS
# * OF THIS NOTICE AND ANY SUCH ADDITIONAL LICENSE AGREEMENT, THE TERMS OF THE AGREEMENT
# * SHALL TAKE PRECEDENCE. BY CONTINUING TO USE THIS SOFTWARE, YOU AGREE TO THE TERMS OF
# * THIS NOTICE.IF YOU DO NOT AGREE TO THESE TERMS, YOU ARE NOT PERMITTED TO USE THIS
# * SOFTWARE.
# *
# ****************************************************************************************
# */

CC=gcc

STATIC_BUILD?=y

# verbosity switch
V?=0

ifeq ($(STATIC_BUILD),y)
	LDFLAGS+=-static
endif

ifeq ($(V),0)
	V_CC = @echo "  CC    " $@;
	V_LINK = @echo "  LINK  " $@;
	V_CLEAN = @echo "  CLEAN ";
	V_CLEAN_TEMP_FILES = @echo "  CLEAN_TEMP_FILES ";
	V_STRIP = @echo "  STRIP " $@;
else
	V_OPT = '-v'
endif

CFLAGS+=-std=gnu99 -Wall -O2 -Wl,-Map,$@.map
INC=-I ../../../sdk/platform/core_modules/crypto -I ../../../third_party/micro_ecc -I ../../../third_party/sha2

ifeq ($(V),2)
	CFLAGS+=--verbose --save-temps -fverbose-asm
	LDFLAGS+=-Wl,--verbose
endif

vpath %.c ../../../third_party/crc32
vpath %.c ../../../sdk/platform/core_modules/crypto
vpath %.c ../../../third_party/micro_ecc
vpath %.c ../../../third_party/sha2
vpath %.c ..

EXEC=mkimage.exe
OBJS=crc32.o mkimage.o sw_aes.o uECC.o sha2.o

# how to compile C files
%.o : %.c
	$(V_CC)$(CC) $(CFLAGS) $(INC) -c $< -o $@ 

all: $(EXEC)

$(EXEC): $(OBJS)
	$(V_LINK)$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)
	$(V_STRIP)strip -s $@
	$(V_CLEAN_TEMP_FILES)rm -f $(OBJS)
	
clean:
	$(V_CLEAN)rm -f $(V_OPT) $(EXEC) *.[ois]
