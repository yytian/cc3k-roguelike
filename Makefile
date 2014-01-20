CXX = g++
CXXFLAGS = -Wall -MMD
OBJECTS = character.o entities.o floor.o logic.o traits.o
DEPENDS = ${OBJECTS:.o=.d}
EXEC = cc3k

${EXEC}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}
