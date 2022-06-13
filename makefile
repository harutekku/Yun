# Compiler & Linker
#-----------------------------------------------------------
export CXX       := g++
export LD        := g++

# Flags
#-----------------------------------------------------------
export CXXFLAGS  := -c -g -O0 -std=c++20 -Wall -Wextra -Wpedantic
export LDFLAGS   := -o
CPPFLAGS         := -I include
DEPFLAGS          = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.temp.d

# Directories
#-----------------------------------------------------------
INCDIR           := include
SRCDIR           := src
OBJDIR           := bin
DEPDIR           := dep

# File extensions
#-----------------------------------------------------------
INCEXT           := hpp
SRCEXT           := cpp

# Files
#-----------------------------------------------------------
SRCFILES         := main.cpp \
                    Value.cpp \
                    Emit.cpp \
                    Exceptions.cpp \
                    VM.cpp \
                    Assembler.cpp \
                    Containers.cpp \
                    Lexer.cpp # Source files
export OBJFILES  := $(SRCFILES:%.$(SRCEXT)=%.o)
DEPFILES         := $(SRCFILES:%.$(SRCEXT)=$(DEPDIR)/%.d)

# Target
#-----------------------------------------------------------
export TARGET    := program

# Final commands
#-----------------------------------------------------------
COMPILE.cpp       = $(CXX) $(CPPFLAGS) $(DEPFLAGS) $(CXXFLAGS) 
POSTCOMPILE.cpp   = mv -f $(DEPDIR)/$*.temp.d $(DEPDIR)/$*.d && touch $(OBJDIR)/$@
export LINK       = $(LD) $(LDFLAGS)

# Paths
#-----------------------------------------------------------
vpath %.$(INCEXT)   $(INCDIR) 
vpath %.$(SRCEXT)   $(SRCDIR) 
vpath %.o           $(OBJDIR)

# Rules
#-----------------------------------------------------------
$(TARGET): $(OBJFILES)
	make --directory=$(OBJDIR)/

%.o: %.$(SRCEXT)
%.o: %.$(SRCEXT) $(DEPDIR)/%.d | $(DEPDIR)
	@echo Compiling $@
	@$(COMPILE.cpp) -o $(OBJDIR)/$@ $<
	@echo Updating timestamps...
	@$(POSTCOMPILE.cpp)

$(DEPDIR): ; @mkdir -p $@

$(DEPFILES):
include $(wildcard $(DEPFILES))
