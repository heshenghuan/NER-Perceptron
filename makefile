CXX ?= g++
CFLAGS = -std=c++11 -Wall -O3 -fPIC
OBJ = src/CharType.o src/StrFun.o src/Tagset.o src/NERDict.o src/NERFeat.o src/Vocabulary.o src/Probability.o src/Recognizer.o src/MultiPerceptron.o src/Pretreatment.o

all: nerp_train nerp_recog convert
	rm -rf *.o *.a
	cd src; rm -rf *.a *.o
.PHONY: all

test: test.o ${OBJ}
	$(CXX) $(CFLAGS) -o test test.o ${OBJ}
	rm -rf test.o

test.o: test.cpp
	$(CXX) $(CFLAGS) -I./include -c test.cpp

# nerp.so: src/nerp.o $(OBJ)
# 	$(CXX) $(CFLAGS) -shared -o nerp.so $(OBJ) src/nerp.o

# src/nerp.o: src/CWSP.cpp $(OBJ)
#	cd src; $(CXX) $(CFLAGS) -I../include -c CWSP.cpp -o nerp.o

# NUSTM_CWSP.so: $(OBJ)
#	$(CXX) $(CFLAGS) -shared -o NUSTM_CWSP.so $(OBJ)

convert: convert.o $(OBJ)
	$(CXX) $(CFLAGS) -o convert convert.o $(OBJ)

convert.o: convert.cpp
	$(CXX) $(CFLAGS) -I./include -c convert.cpp

nerp_train: nerp_train.o $(OBJ) 
	$(CXX) $(CFLAGS) -o nerp_train nerp_train.o $(OBJ)

nerp_train.o: nerp_train.cpp
	$(CXX) $(CFLAGS) -I./include -c nerp_train.cpp

nerp_recog: nerp_recog.o $(OBJ)
	$(CXX) $(CFLAGS) -o nerp_recog nerp_recog.o $(OBJ)

nerp_recog.o: nerp_recog.cpp
	$(CXX) $(CFLAGS) -I./include -c nerp_recog.cpp

src/CharType.o: src/CharType.cpp
	cd src; $(CXX) $(CFLAGS) -I../include -c CharType.cpp -o CharType.o

src/StrFun.o: src/StrFun.cpp
	cd src; $(CXX) $(CFLAGS) -I../include -c StrFun.cpp -o StrFun.o

src/Tagset.o: src/Tagset.cpp
	cd src; $(CXX) $(CFLAGS) -I../include -c Tagset.cpp -o Tagset.o

src/NERDict.o: src/NERDict.cpp
	cd src; $(CXX) $(CFLAGS) -I../include -c NERDict.cpp -o NERDict.o 

src/NERFeat.o: src/NERFeat.cpp
	cd src; $(CXX) $(CFLAGS) -I../include -c NERFeat.cpp -o NERFeat.o

src/Vocabulary.o: src/Vocabulary.cpp
	cd src; $(CXX) $(CFLAGS) -I../include -c Vocabulary.cpp -o Vocabulary.o 

src/Probability.o: src/Probability.cpp
	cd src; $(CXX) $(CFLAGS) -I../include -c Probability.cpp -o Probability.o

src/MultiPerceptron.o: src/MultiPerceptron.cpp
	cd src; $(CXX) $(CFLAGS) -I../include -c MultiPerceptron.cpp -o MultiPerceptron.o

src/Pretreatment.o: src/Pretreatment.cpp
	cd src; $(CXX) $(CFLAGS) -I../include -c Pretreatment.cpp -o Pretreatment.o

src/Recognizer.o: src/Recognizer.cpp
	cd src; $(CXX) $(CFLAGS) -I../include -c Recognizer.cpp -o Recognizer.o

clean:
	rm -rf *.o *.a
	cd src; rm -f *.a *.o
	cd data; rm -rf train

cleanall:
	rm -rf nerp_train nerp_recog convert NUSTM_CWSP.so nerp.so
	rm -rf *.o *.a
	cd src; rm -f *.a *.o
	cd data; rm -rf train
