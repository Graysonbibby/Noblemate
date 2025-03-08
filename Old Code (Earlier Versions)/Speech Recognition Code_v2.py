
#-------------------------------------------------------------------------------------------------------------------
# Install SpeechRecognition Library 
import speech_recognition as sr
import re
import difflib # Fuzzy Matching

#-------------------------------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------------------------------
# Initialization Steps 
recognizer = sr.Recognizer()
recognizer.pause_threshold = 2               # Parameter to pause to change recording
recognizer.dynamic_energy_threshold = True     # Automatically adjust energy threshold based on ambient noise

#-------------------------------------------------------------------------------------------------------------------

# Error Handling 
# Saud - Custom Correction dictionary for similar sounding words
corrections = {
    # Corrections for "knight"
    "night": "knight",
    "nite": "knight",
    "knite": "knight",
    "nigt": "knight",
    "knife": "knight",

    # Corrections for "pawn"
    "han": "pawn",
    "todd": "pawn",
    "juan": "pawn",
    "pahn": "pawn",
    "pown": "pawn",
    
    # Corrections for "rook"
    "rock": "rook",
    "ruke": "rook",
    "book": "rook",
    "brooke": "rook",
    "brook": "rook",
    
    # Corrections for "bishop"
    "bishup": "bishop",
    "bischop": "bishop",
    "bisshop": "bishop",
    
    # Corrections for "queen"
    "quean": "queen",
    "quin": "queen",
    "keen": "queen",
    
    # Corrections for "king"
    "kin": "king",
    "thing": "king"
}

# Valid chess pieces for fuzzy matching
valid_pieces = ["pawn", "knight", "bishop", "rook", "queen", "king"]

# Return the correct chess piece using static corrections and fuzzy matching.
def get_correct_piece(word):
    word_lower = word.lower()
    if word_lower in corrections:
        return corrections[word_lower]
    matches = difflib.get_close_matches(word_lower, valid_pieces, n=1, cutoff=0.6)
    return matches[0] if matches else word_lower

#-------------------------------------------------------------------------------------------------------------------
# Prompting the user for an input

def get_user_move():
    while True:
        try:
            with sr.Microphone() as source:
                print("\nCalibrating for ambient noise (Please remain silent for a moment).")
                recognizer.adjust_for_ambient_noise(source, duration=1.0)
                print("Listening for your move (e.g., 'Knight from E2 to F4'). Please speak clearly:")
                audio_data = recognizer.listen(source)

            # This line uses Google's Speech to Text to recognize your raw input back to a guess of what your said
            # This serves as the raw input to the system. With word correction, you have a more useable input
            spoken_text = recognizer.recognize_google(audio_data) 
            print(f"\nYou said: \"{spoken_text}\"")

        # Error Handling from ChatGPT
        except sr.UnknownValueError:
            print("I couldn't understand your speech. Please try again and speak a bit more clearly.")
            continue

        except sr.RequestError as e:
            print(f"Could not request results from the speech service; {e}.")
            continue

        except Exception as e:
            print(f"An unexpected error occurred: {e}")
            continue

        # Process the spoken text
        # First we will apply Corrections, then remove filler words then isolate the important information to be saved later
        move_text = spoken_text.lower()
        words = move_text.split()
        # Apply static corrections first
        corrected_words = [corrections.get(word, word) for word in words]
        move_text = " ".join(corrected_words)
        
        # Combine separated rank and file (e.g., "e 2" -> "e2")
        move_text = re.sub(r'\b([a-h])\s+([1-8])\b', r'\1\2', move_text)

        # Remove filler words like "move", "from", "to"
        move_text = re.sub(r'\b(move|from|to)\b', '', move_text)

        # Clean up extra spaces
        move_text = re.sub(r'\s+', ' ', move_text).strip()
        
        # Split the processed text into components
        parts = move_text.split()
        piece = None
        start_sq = None
        end_sq = None

        if len(parts) == 2:
            # If only two parts are provided, assume it's a pawn move
            piece = "pawn"
            start_sq, end_sq = parts
        elif len(parts) == 3:
            # Use fuzzy matching for the piece name in three-part input
            piece = get_correct_piece(parts[0])
            start_sq, end_sq = parts[1], parts[2]
        else:
            print("Could not parse your move. Ensure you say something like 'Knight from A2 to B3'.")
            continue

        # Validate board squares (must be a letter a-h and a number 1-8)
        valid_square = lambda s: len(s) == 2 and s[0] in "abcdefgh" and s[1] in "12345678"
        if piece not in valid_pieces or not valid_square(start_sq) or not valid_square(end_sq):
            print("Invalid move format. Please use valid piece names and board coordinates (e.g., A2, B3).")
            continue

        # Format squares to uppercase for clarity
        start_sq = start_sq[0].upper() + start_sq[1]
        end_sq = end_sq[0].upper() + end_sq[1]
        return piece.capitalize(), start_sq, end_sq


if __name__ == "__main__":
    piece, from_square, to_square = get_user_move()
    print(f"\nFinal Move Parsed:\nPiece: {piece}, From: {from_square}, To: {to_square}")
