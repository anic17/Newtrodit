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

InputUTF8 get_newtrodit_input()
{
	DWORD fdwSaveOldMode, cNumRead;
	INPUT_RECORD irInBuf[128];
	InputUTF8 inputchar = {0};

	if (!GetConsoleMode(hStdin, &fdwSaveOldMode))
		return inputchar;

	SetConsoleMode(hStdin, 0);
	long other_flags = 0;

	int ukey_counter = 0, waiting_combination = FALSE;
	const int replacement_char = 0xefbfbd; // Replacement char is in hexadecimal EF BF BD
	static size_t pushback_char_count = 0;
	static INPUT_RECORD pushback_inputchar[128] = {0};
	INPUT_RECORD *ptr_irInBuf = NULL;
	char unicodestr[5] = {0};
	while (1)
	{
		// Wait for the input events

		WaitForSingleObject(hStdin, INFINITE);

		if (pushback_char_count > 0)
		{
			cNumRead = pushback_char_count;
		}
		else
		{
			if (!ReadConsoleInput(hStdin, irInBuf, 128, &cNumRead))
				return inputchar;
		}

		for (DWORD i = 0; i < cNumRead; i++)
		{
			if (pushback_char_count > 0)
			{
				ptr_irInBuf = &pushback_inputchar[0];

				for (size_t j = 0; j < pushback_char_count - 1; j++)
					pushback_inputchar[j] = pushback_inputchar[j + 1];

				pushback_char_count--;
			}
			else
			{
				ptr_irInBuf = &irInBuf[i];
			}

			switch (ptr_irInBuf->EventType)
			{
			case KEY_EVENT: // We are only interested in keyboard input
				if (ptr_irInBuf->Event.KeyEvent.uChar.UnicodeChar)
				{
					if (inputchar.utf8char == replacement_char) // If we receive the UTF-8 replacement character, stop the input
						break;

					ukey_counter++;
					if (ptr_irInBuf->Event.KeyEvent.bKeyDown && ukey_counter <= 4 && (((ptr_irInBuf->Event.KeyEvent.uChar.UnicodeChar & 0xff) < 0x80 && cNumRead <= 1) || ((ptr_irInBuf->Event.KeyEvent.uChar.UnicodeChar & 0xff) > 0x80 && cNumRead > 1)))
					{
						inputchar.utf8char <<= 8;
						inputchar.utf8char |= (ptr_irInBuf->Event.KeyEvent.uChar.UnicodeChar & 0xff);
					}
					else
					{
						if (ptr_irInBuf->Event.KeyEvent.bKeyDown)
						{
							if (pushback_char_count < 127 && pushback_char_count < cNumRead)
								pushback_inputchar[pushback_char_count++] = *ptr_irInBuf;
						}
					}
					waiting_combination = FALSE;
				}
				else
				{
					if (ptr_irInBuf->Event.KeyEvent.bKeyDown)
					{
						switch (ptr_irInBuf->Event.KeyEvent.wVirtualKeyCode)
						{
						case VK_CONTROL:
						case VK_LCONTROL:
						case VK_RCONTROL:
							other_flags |= CTRL_BITMASK;
							waiting_combination = TRUE;
							break;
						case VK_SHIFT:
						case VK_LSHIFT:
						case VK_RSHIFT:
							other_flags |= SHIFT_BITMASK;
							waiting_combination = TRUE;
							break;
						case VK_LWIN: // Ignore Windows keys
						case VK_RWIN:
							break;
						case VK_MENU: // Windows calls ALT keys 'menu keys' for some reason
						case VK_LMENU:
						case VK_RMENU:
							other_flags |= ALT_BITMASK;
							waiting_combination = TRUE;
							break;
						default:
							inputchar.flags <<= 8;
							inputchar.flags |= ptr_irInBuf->Event.KeyEvent.wVirtualKeyCode; // If not one of the above cases, save the virtual key code
							break;
						}
					}
					else
					{
						other_flags = 0;
					}
				}
				break;
			}
		}

		if (!(other_flags && inputchar.flags == 0))
			waiting_combination = FALSE;

		if (!waiting_combination)
		{
			if (GetKeyState(VK_CONTROL) & 0x8000) // Make sure the key is still pressed, for example, making another combination without releasing the control key
				other_flags |= CTRL_BITMASK;
			if (GetKeyState(VK_SHIFT) & 0x8000) // Same thing as above, now for shift
				other_flags |= SHIFT_BITMASK;
			if (GetKeyState(VK_MENU) & 0x8000) // ALT bitmask
				other_flags |= ALT_BITMASK;

			if (ptr_irInBuf->EventType == KEY_EVENT && ptr_irInBuf->Event.KeyEvent.bKeyDown)
			{
				inputchar.flags |= other_flags; // Add the other flags in case they are defined

				int32_to_char_array(inputchar.utf8char, unicodestr);
				if (utf8valid(unicodestr) != 0)
				{
					inputchar.utf8char = 0;
					inputchar.flags = -EILSEQ; // Invalid sequence
				}
				if (inputchar.utf8char != 0 && ukey_counter > 4)
				{
					inputchar.utf8char = 0;
					inputchar.flags = replacement_char; // If a possibly invalid sequence is inputted, replace it with the replacement char
				}
				SetConsoleMode(hStdin, fdwSaveOldMode);
				return inputchar;
			}
		}

		FlushConsoleInputBuffer(hStdin); // Flush the input buffer so previous keystrokes are ignored
	}
}
