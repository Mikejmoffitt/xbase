
.PHONY: all clean $(OUTDIR)/$(APPNAME).x

#
# XBase Sources
#
$(XBOBJDIR)/%.o: $(XB)/%.c
	@mkdir -p $(@D)
	@bash -c 'printf "\t\e[96m[  C  ]\e[0m $<\n"'
	$(CC) $(CFLAGS) -c $< -o $@

$(XBOBJDIR)/%.o: $(XB)/%.a68
	@mkdir -p $(@D)
	@bash -c 'printf "\t\e[95m[ ASM ]\e[0m $<\n"'
	gawk '{gsub(/;/,";#"); printf("%s", $$0 RT)}' RS='"[^"]*"' $< | gawk '{gsub(/\$$/,"0x"); printf("%s", $$0 RT)}' RS='"[^"]*"' | $(AS) $(ASFLAGS) -o $@ -c -

#
# Application Sources
#

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(SOURCES_H) prebuild
	@mkdir -p $(@D)
	@bash -c 'printf "\t\e[96m[  C  ]\e[0m $<\n"'
	$(CC) $(CFLAGS) $< -c -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.a68 $(SOURCES_H) prebuild
	@mkdir -p $(@D)
	@bash -c 'printf "\t\e[95m[ ASM ]\e[0m $<\n"'
	gawk '{gsub(/;/,";#"); printf("%s", $$0 RT)}' RS='"[^"]*"' $< | gawk '{gsub(/\$$/,"0x"); printf("%s", $$0 RT)}' RS='"[^"]*"' | $(AS) $(ASFLAGS) -o $@ -c -

#
# Output
#

$(PROJECT_OUTPUT): $(XB_OBJECTS_ASM) $(XB_OBJECTS_C) $(OBJECTS_ASM) $(OBJECTS_C)
	@bash -c 'printf "\t\e[94m[ LNK ]\e[0m $(OBJECTS_ASM) $(OBJECTS_C)\n"'
	$(CC) -o $(APPNAME).bin $(LDFLAGS) $(CFLAGS) $^ $(LIBS)
	mkdir -p $(@D)
	$(OBJCOPY) -v -O xfile $(APPNAME).bin $@ > /dev/null
	@rm $(APPNAME).bin
	@cp -r $(DISKROOT)/* $(OUTDIR)/
	@bash -c 'printf "\e[92m\n\tBuild Complete. \e[0m\n\n"'

.PHONY: xbvars
xbvars:
	@echo "XB_SOURCES_ASM is " "$(XB_SOURCES_ASM)"
	@echo "XB_OBJECTS_ASM is " "$(XB_OBJECTS_ASM)"
	@echo "XB_SOURCES_C is " "$(XB_SOURCES_C)"
	@echo "XB_OBJECTS_C is " "$(XB_OBJECTS_C)"
	@echo "SOURCES_ASM is " "$(SOURCES_ASM)"
	@echo "OBJECTS_ASM is " "$(OBJECTS_ASM)"
	@echo "SOURCES_C is " "$(SOURCES_C)"
	@echo "OBJECTS_C is " "$(OBJECTS_C)"
