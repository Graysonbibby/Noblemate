# NobleMate (Automated Voice Controlled Chess Game)
- ### Project Contributors Include: Saud Amjad, Grayson Bibby, Hamzah Nadeem, Kunj Patel, Megh Patel, Nihar Trivedi

**Project Description:** Over the past semester, our team, NobleMate, had designed a voice-controlled, completely automated chess game system against Stockfish.  Our focus being hands-free control with minimal intervention from the user, utilizing an X-Y Coordinate Mechanical Gantry system with an electromagnet for autonomous movement. Each chess piece was 3D printed with slots for magnets at the base. You say your moves by speaking into the microphone when prompted, saying "Pawn from C2 to C3" and this data is serially communicated to the gantry system. You can select your colour (White/Black) and can choose a difficulty (Easy, Medium, Difficult, Insane)
# --------------------------------------------------
- Note: main.cpp and Makefile are given as well
### Documentation + Comprehensive BOM

- Final Design Specification Report Outlines full design implementation + design considerations 
- Page 60 outlines the full BOM of the project

### PCB (Altium Designer files)
- PCB design for a 12V/5V DC-DC buck converter in the shape of a chess piece. You are going from the 12V wall supply and powering the Arduino Uno, Raspberry Pi, and Electromagnet all from this PCB

### Voice Recognition (Python - Requires SpeechRecognition, google-cloud-speech, difflib)

- For the recognizer: Google Cloud Speech-To-Text v1 REST API or Google Web Speech API is sufficient for this project. Google Cloud will require Application Default Credentials to be defined on your system. This is easily doable with a Google Account and downloading Google SDK. 
- All of the voice recognition code is under "chess_voice.py" in the Final Code Folder and the function is then called in a C++ environment using "voiceCommand.cpp" (Requires "voiceCommand.hpp")
- This will require WiFi and the code will be on the Raspberry Pi

### Gantry Movement (Embedded C++ - Code for Arduino Uno)

- This code is developed for the required stepper motor control for the Gantry System. The code was developed using the Arduino Uno IDE

### Chess Game Logic and Functionality (C++)

- Largely developed this ourself and the required code is in the Final Code folder. 
- REST API for Stockfish was used 
- We used https://github.com/awsomeCStutorials/chess-game/blob/master/README.md as a reference for a starting point, but wrote all the C++ code ourselves
- Files include (chess_board, stockfish_service, FENConverter, main, pieces)
- Stockfish is running locally on the Raspberry Pi

### Serial Communication 

- This project utilized a Raspberry Pi 3 Model B for the Chess Logic/Game functionality and an Arduino Uno for the Gantry System Control
- Sends a 7 digit number to the Arduino -> First four digits are the initial and end coordinates of the piece of interest, Final three digits are corresponding flags for special moves, etc. 
