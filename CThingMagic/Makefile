CUR_DIR=/home/debian/CThingMagic
LIB=-L./APILib -lmercuryapi -L./APILib -lltkc -L./APILib -lltkctm -lpthread -std=c++11

App_DIR=${CUR_DIR}/Application
Lib_DIR=${CUR_DIR}/libSargas

INC_DIR= -I${App_DIR} \
         -I${Lib_DIR} \
		 -I$(CUR_DIR)/c/src/api/ 
		 #-I$(CUR_DIR)/include/
		 #-I$(CUR_DIR)/c/src/api/lib/LTK/LTKC/Library/ \
		 #-I$(CUR_DIR)/c/src/api/lib/LTK/LTKC/Library/LLRP.org/ \

SRC = ${wildcard  ${App_DIR}/*.cpp} \
      ${wildcard  ${Lib_DIR}/*.cpp} 
   
OBJ = ${patsubst %.cpp, %.o, ${SRC}}

TARGET=sargas.out
CC=g++
CCFLAGS=-g -Wall ${INC_DIR}


${TARGET}: ${OBJ}
	${CC} ${OBJ} $(LIB) -o $@
	@echo "make successed !!!."


$(OBJ):%.o:%.cpp
	@echo "Compiling $< ==> $@"
	${CC} ${CCFLAGS} -c $< -o $@

clean:
	@rm -f ${OBJ}
	@echo "Clean object files done."

	#@rm -f *~
	#@echo "Clean tempreator files done."

	@rm -f ${TARGET}
	@echo "Clean target files done."

	@echo "Clean done."

