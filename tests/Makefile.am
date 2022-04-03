EXTRA_DIST = CMakeLists.txt

if GNU_GAMA_G3_ENABLED
   GAMA_G3 = gama-g3
endif

if GNU_GAMA_LOCAL_EXTRA_TESTS
   EXTRA_TESTS = matvec statan acord2 extra-tests
endif

SUBDIRS = gama-local krumm $(GAMA_G3) $(EXTRA_TESTS)
