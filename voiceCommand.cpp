#include "voiceCommand.hpp"

Move moveInput(){
    // Add the current directory to the Python module search path
    PyRun_SimpleString("import sys; sys.path.insert(0, '/home/graysonbibby/Documents/chessboard_code2/newenv/lib/python3.11/site-packages')");
    PyRun_SimpleString("import sys; sys.path.insert(0, '/home/graysonbibby/Documents/chessboard_code2')");
       // Import the Python module (without the .py extension)
    PyObject* pName = PyUnicode_DecodeFSDefault("chess_voice");
    PyObject* pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    Move moveResult;


    if (pModule != nullptr) {
        // Get the get_user_move() function from the module
        PyObject* pFunc = PyObject_GetAttrString(pModule, "get_user_move");
        if (pFunc && PyCallable_Check(pFunc)) {
            // Call the function with no arguments
            PyObject* pResult = PyObject_CallObject(pFunc, nullptr);
            if (pResult != nullptr) {
                // Ensure the result is a tuple with three elements
                if (PyTuple_Check(pResult) && PyTuple_Size(pResult) == 3) {
                    // Extract the three returned items (all expected as strings)
                    PyObject* pPiece = PyTuple_GetItem(pResult, 0);
                    PyObject* pStart = PyTuple_GetItem(pResult, 1);
                    PyObject* pEnd   = PyTuple_GetItem(pResult, 2);

                    // Convert Python strings to C++ strings using UTF-8 encoding
                    const char* piece_c = PyUnicode_AsUTF8(pPiece);
                    const char* start_c = PyUnicode_AsUTF8(pStart);
                    const char* end_c   = PyUnicode_AsUTF8(pEnd);

                    moveResult.piece = piece_c;
                    moveResult.startSquare = start_c;
                    moveResult.endSquare = end_c;
                } else {
                    std::cerr << "Returned value is not a tuple of size 3." << std::endl;
                }
                Py_DECREF(pResult);
            } else {
                PyErr_Print();
                std::cerr << "Call to get_user_move() failed." << std::endl;
            }
            Py_XDECREF(pFunc);
        } else {
            if (PyErr_Occurred())
                PyErr_Print();
            std::cerr << "Cannot find function get_user_move." << std::endl;
        }
        Py_DECREF(pModule);
    } else {
        PyErr_Print();
        std::cerr << "Failed to load module 'chess_voice'." << std::endl;
    }
    
    return moveResult;

}

std::string voiceInput(const char* pythonModuleName){
        // Add the current directory to the Python module search path
    PyRun_SimpleString("import sys; sys.path.insert(0, '/home/graysonbibby/Documents/chessboard_code2/newenv/lib/python3.11/site-packages')");
    PyRun_SimpleString("import sys; sys.path.insert(0, '/home/graysonbibby/Documents/chessboard_code2')");

    // Import the Python module (without the .py extension)
    PyObject* pName = PyUnicode_DecodeFSDefault("chess_voice");
    PyObject* pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    
    std::string returnString;
    
    if (pModule != nullptr) {
        // Get the get_user_move() function from the module
        PyObject* pFunc = PyObject_GetAttrString(pModule, pythonModuleName);
        if (pFunc && PyCallable_Check(pFunc)) {
            // Call the function with no arguments
            PyObject* pResult = PyObject_CallObject(pFunc, nullptr);
            if (pResult != nullptr) {
                const char* c_result = PyUnicode_AsUTF8(pResult);
                if(!c_result) std::cerr << "Call failed." << std::endl;
                
                returnString = c_result;
                
                Py_DECREF(pResult);
            } else {
                PyErr_Print();
                std::cerr << "Call failed." << std::endl;
            }
            Py_XDECREF(pFunc);
        } else {
            if (PyErr_Occurred())
                PyErr_Print();
            std::cerr << "Cannot find function." << std::endl;
        }
        Py_DECREF(pModule);
    } else {
        PyErr_Print();
        std::cerr << "Failed to load module 'chess_voice'." << std::endl;
    }
    
    return returnString;
}

Coords voiceCommandProcessing(std::string square){
    return {(square)[0] - '0', (square)[1] - '0'};
}
