SUBDIRS=pacemaker heart
clean_SUBDIRS=$(addprefix clean_,$(SUBDIRS))
cleanall_SUBDIRS=$(addprefix cleanall_,$(SUBDIRS))

all: $(SUBDIRS)
clean: $(clean_SUBDIRS)
cleanall: $(clean_SUBDIRS)

.PHONY: force

$(SUBDIRS): force
	make -C $@

$(clean_SUBDIRS): force
	make -C $(patsubst clean_%,%,$@) clean

$(cleanall_SUBDIRS): force
	make -C $(patsubst cleanall_%,%,$@) cleanall

