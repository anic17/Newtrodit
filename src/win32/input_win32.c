HANDLE hStdin;

int getch_n()
{
	int gc_n = 0;
	gc_n = getch();
	if (gc_n == 0)
	{
		gc_n = getch();
		gc_n |= BIT_ESC0;
	}
	else if (gc_n == 0xE0)
	{
		gc_n = getch();
		gc_n |= BIT_ESC224;
	}

	return gc_n;
}

InputUTF8 GetNewtroditInput()
{
	DWORD fdwSaveOldMode;
	INPUT_RECORD irInBuf[128];
	DWORD cNumRead, i;
		InputUTF8 inputchar = {0};

	if (!GetConsoleMode(hStdin, &fdwSaveOldMode))
		return inputchar;
	SetConsoleMode(hStdin, 0); // Set console mode to zero to capture events like Alt-F4 or Control-C
	int counter = 0;
	char unicodestr[5] = {0};
	// Wait for the events.

	WaitForSingleObject(hStdin, INFINITE);

	if (!ReadConsoleInput(
			hStdin,		// input buffer handle
			irInBuf,	// buffer to read into
			128,		// size of read buffer
			&cNumRead)) // number of records read
		return inputchar;

	// Dispatch the events to the appropriate handler.
	for (i = 0; i < cNumRead; i++)
	{
		switch (irInBuf[i].EventType)
		{
		case KEY_EVENT: // keyboard input
			if (irInBuf[i].Event.KeyEvent.uChar.UnicodeChar)
			{
				if (irInBuf[i]	.Event.KeyEvent.bKeyDown)
					//printf("%s\tUChar: %hd\t0x%04x\n", irInBuf[i].Event.KeyEvent.bKeyDown ? "Press" : "Release", irInBuf[i].Event.KeyEvent.uChar.UnicodeChar, irInBuf[i].Event.KeyEvent.uChar.UnicodeChar);
				counter++;
				if (irInBuf[i].Event.KeyEvent.bKeyDown && i < 4)
				{
					inputchar.utf8char = inputchar.utf8char << 8;
					inputchar.utf8char |= (irInBuf[i].Event.KeyEvent.uChar.UnicodeChar) & 0xff; //  & ~0xff00
				}
			}
			else
			{
				inputchar.flags = irInBuf[i].Event.KeyEvent.wVirtualKeyCode;
			}

			break;
		}
	}
	FlushConsoleInputBuffer(hStdin);

	if (irInBuf[0].EventType == KEY_EVENT && irInBuf[0].Event.KeyEvent.bKeyDown)
	{
		int32_to_char_array(inputchar.utf8char, unicodestr);
		if (utf8valid(unicodestr) != 0)
		{
			inputchar.utf8char = 0;
			inputchar.flags = EILSEQ; // Invalid sequence
		}
			//print_message(NEWTRODIT_ERROR_INVALID_UNICODE_SEQUENCE);

		//else
			//printf("\nUTF8 char: 0x%02x (%03d)\tCTRL: 0x%02x\n", inputchar.utf8char, inputchar.utf8char, inputchar.flags);
	}



	SetConsoleMode(hStdin, fdwSaveOldMode);
	return inputchar;
}
