ifeq ($(OMP),1)
	CFLAGS += -fopenmp
endif

PublicPrivate: RSA_fastexp.cpp
	g++ $(CFLAGS) -o PublicPrivate RSA_fastexp.cpp
