#qepp makefile

include make.inc

TARGET=libqepp.so
ITARGET = qepp

_DEPS = \
functions.h \
mpi_base.h \
my_lapack.h \
other.h \
qepp_constants.h \
qepp_err.h \
qepp_io.h \
qepp_file.h \
qepp_read.h \
qepp_main.h \
qepp_mem.h \
qepp_mpi.h \
qepp_struct.h 

DEPS = $(patsubst %,$(INC)/$(ITARGET)/%,$(_DEPS))

_OBJ = \
band_data.o \
bands_pp.o \
cd.o \
data_file.o \
data_set.o \
evc.o \
egv.o \
fit_params.o \
functions.o \
gkv.o \
m_elem.o \
mpi_base.o \
mpi_data.o \
my_lapack.o \
nscf_data.o \
nscf_md.o \
opt_data.o \
other.o \
pdos_data.o \
pdos_state.o \
qepp_err.o \
qepp_io.o \
qepp_file.o \
qepp_main.o \
qepp_mem.o \
qepp_mpi.o \
qepp_read.o \
qepp_struct.o \
spin_data.o \
wfc.o
  
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

.PHONY : clean veryclean install uninstall

$(TARGET) : $(OBJ)
	$(MPICC) -shared $(CFLAGS) -o $@ $(OBJ)

$(ODIR)/%.o : $(SDIR)/%.c $(DEPS)
	$(MPICC) -c -o $@ $< $(CFLAGS)

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
	rm -f $(ODIR)/*.o *~ core $(INC)/*~ 

veryclean : clean uninstall
	rm -f *.so
