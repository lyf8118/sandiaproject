BYTE sendData_waitTilDone(BYTE* dataBuf, WORD size, BYTE intfNum, ULONG ulTimeout);
BYTE sendData_inBackground(BYTE* dataBuf, WORD size, BYTE intfNum, ULONG ulTimeout);
BYTE receiveData_waitTilDone(BYTE* dataBuf, WORD size, BYTE intfNum, ULONG ulTimeout);
WORD receiveDataInBuffer(BYTE*,WORD,BYTE);
