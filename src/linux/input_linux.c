struct termios orig_termios;

int rawMode(int fd)
{
	/* This is taken from kilo, make sure to check the project at https://github.com/antirez/kilo */
	struct termios raw;

	if (!isatty(STDIN_FILENO) || tcgetattr(fd, &orig_termios) == -1)
	{
		perror("rawMode()");
		return 0;
	}

	raw = orig_termios; /* modify the original mode */
	/* input modes: no break, no CR to NL, no parity check, no strip char,
	 * no start/stop output control. */
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	/* output modes - disable post processing */
	// raw.c_oflag &= ~(OPOST);
	/* control modes - set 8 bit chars */
	raw.c_cflag |= (CS8);
	/* local modes - choing off, canonical off, no extended functions,
	 * no signal chars (^Z,^C) */
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	/* control chars - set return condition: min number of bytes and timer. */
	raw.c_cc[VMIN] = 0;	 /* Return each byte, or zero for timeout. */
	raw.c_cc[VTIME] = 1; /* 100 ms timeout (unit is tens of second). */
	if (tcsetattr(fd, TCSAFLUSH, &raw) < 0)
	{
		perror("tcsetattr()");
		return 0;
	}
	return 1;
}

InputUTF8 getch()
{
    InputUTF8 inputchar = {0};

    if (!rawMode(STDIN_FILENO))
    {
        return inputchar;
    }
    const size_t maxreadsz = 1024;
    unsigned char *buf = calloc(sizeof(char), maxreadsz + 1), seq[100] = {0}; // Have enough space for all the buffer
    int numread;
    int key = 0;
    int shiftbits = 7;
    bool is_esc = false;
    bool is_ctrl = false;
    bool is_ctrl2 = false;
    bool is_tilde = false;

    is_esc = false;

    is_ctrl = false;
    is_ctrl2 = false;

    numread = read(STDIN_FILENO, buf, maxreadsz);
    if (buf[0] == 27)
    {
        is_esc = true;
        if (numread > 1) // Control code
        {
            is_ctrl = true;
            // Key encoding starts here
            if ((buf[1] == '[' || buf[1] == 'O')) // -30 is signed 226
            {
                if (buf[2] == 226) // Special case for Euro symbol in european layouts
                {
                    buf[2] = 'E';
                }
                if (numread > 7)
                    numread = 7;
                inputchar.flags |= 0xFF;
                for (size_t i = 2; i < numread; i++)
                {
                    inputchar.flags = inputchar.flags << 8; // Shift only seven bytes
                    inputchar.flags |= buf[i];
                }
                inputchar.inputlen = numread;
            }
            else
            {
                inputchar.flags = buf[1]; // Alt-[key] special case
                inputchar.inputlen = 2;
            }
        }
    }

    else
    {
        if (numread > 4)
            numread = 4;
        inputchar.inputlen = numread;

        key = buf[0];
        for (int i = 0; i < numread; i++)
        {
            inputchar.utf8char = inputchar.utf8char << 8;
            inputchar.utf8char |= buf[i];
            inputchar.equiv[i] = buf[i];
        }
    }
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) < 0) // Restore original mode
    {
    }
    free(buf);
    return inputchar;
}

InputUTF8 GetNewtroditInput()
{
	echo_on();
    canon_off();
	return getch();
}

int getch_n()
{
    InputUTF8 ret = GetNewtroditInput();
    return ret.utf8char;
}
