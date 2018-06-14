#qepp makefile

include make.inc

TARGET=libqepp.so
ITARGET = qepp

_DEPS = \
mpi_base.h \
my_lapack.h \
qepp_constants.h \
qepp_err.h \
qepp_file.h \
qepp_function.h \
qepp_io.h \
qepp_main.h \
qepp_mem.h \
qepp_mpi.h \
qepp_read.h \
qepp_struct.h \
wrapper_libxml.h

DEPS = $(patsubst %,$(INC)/$(ITARGET)/%,$(_DEPS))

_OBJ = \
function_eps_calc.o \
function_eps_kk.o \
function_nscf_data.o \
function_opt_data.o \
function_wf_pp.o \
mpi_base.o \
my_lapack.o \
qepp_err.o \
qepp_file.o \
qepp_io.o \
qepp_mem.o \
qepp_mpi.o \
qepp_read.o \
qepp_struct.o \
struct_band_data.o \
struct_bands_pp.o \
struct_cd.o \
struct_data_file.o \
struct_data_set.o \
struct_fit_params.o \
struct_m_elem.o \
struct_mpi_data.o \
struct_nscf_data.o \
struct_nscf_md.o \
struct_opt_data.o \
struct_pdos_data.o \
struct_pdos_state.o \
struct_pseudo.o \
struct_spin_data.o \
struct_wfc.o \
wrapper_libxml.o \
ylmr.o
  
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

.PHONY : clean veryclean install uninstall

$(TARGET) : $(OBJ)
	$(CC) -shared $(CFLAGS) $(CPPFLAGS) -o $@ $(OBJ)

$(ODIR)/%.o : $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS)

install : $(TARGET)
	@mkdir -p $(LDIR); \
	mkdir -p $(IDIR); \
	mkdir -p $(IDIR)/$(ITARGET);
	ln -sf $(CURDIR)/$(TARGET) $(LDIR) ; \
	ln -sf $(CURDIR)/inc/$(ITARGET)/* $(IDIR)/$(ITARGET)/
	@mkdir -p $(LDIR)/pkgconfig
	@echo "Name: qepp\nDescription: custom postproc for QE\nprefix=$(PREFIX)\nlibdir=$(LDIR)" > $(ITARGET).pc
	@echo "Version: alfa\nincludedir=$(IDIR)/$(ITARGET)\nLibs: -L$(LDIR) -lqepp -lm" >> $(ITARGET).pc
	@echo "Cflags: $(CFLAGS)" >> $(ITARGET).pc
	mv $(ITARGET).pc $(LDIR)/pkgconfig

uninstall: 
	if test -f $(LDIR)/$(TARGET); then \
		rm $(LDIR)/$(TARGET); fi
	if test -d $(IDIR)/$(ITARGET)/; then \
		rm -r $(IDIR)/$(ITARGET)/; fi
	if test -f $(LDIR)/pkgconfig/$(ITARGET).pc; then \
		rm -f $(LDIR)/pkgconfig/$(ITARGET).pc; fi

all : fold $(TARGET)

fold :
	@mkdir -p obj; \
	if ! test -d src; then \
	(exit 1) ; fi ;\
	if ! test -d inc; then \
	(exit 1) ; fi \

clean :
	rm -f $(ODIR)/*.o *~ core $(INC)/*~ $(SDIR)/*~

veryclean : clean uninstall
	rm -f *.so make.inc
