ifeq ($(OMP),1)
	CFLAGS += -fopenmp
endif
ifeq ($(GPU),1)
        CFLAGS += -acc
	CXX = pgc++
else
	CXX = g++
endif


PublicPrivate: RSA_GPU.cpp
	$(CXX) $(CFLAGS) -o PublicPrivate RSA_GPU.cpp
