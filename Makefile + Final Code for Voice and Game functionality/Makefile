CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra $(shell python3-config --includes)
LDFLAGS = $(shell python3-config --ldflags) -lpython3.11

SRCS = main.cpp chess-board.cpp FENConverter.cpp stockfish_service.cpp voiceCommand.cpp serial_communication.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = chess_game

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(EXEC)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

venv:
	python3 -m venv newenv
	./env/bin/pip install --upgrade pip
	./env/bin/pip install google-cloud-speech SpeechRecognition pyaudio
	
clean:
	rm -f $(OBJS) $(EXEC)
	
.PHONY: all clean venv run


