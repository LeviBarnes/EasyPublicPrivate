ifeq ($(OMP),1)
	CFLAGS += -fopenmp
endif
ifeq ($(GPU),1)
        CFLAGS += -acc
	CXX = pgc++
else
	CXX = g++
endif

ifeq ($(CUDA),1)
	CFLAGS += -x cu -arch=sm_35  -D__CUDA
	CXX = nvcc
endif


PublicPrivate: RSA_CUDA.cpp
	$(CXX) $(CFLAGS) -o PublicPrivate RSA_CUDA.cpp
