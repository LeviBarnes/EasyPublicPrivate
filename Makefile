ifeq ($(OMP),1)
	CFLAGS += -fopenmp
endif

PublicPrivate: RSA_randomsalt.cpp
	g++ $(CFLAGS) -o PublicPrivate RSA_randomsalt.cpp
