<div class="newtrodit-md">




# Newtrodit
Simple console text editor written in C. 

![newtrodit-stars](https://img.shields.io/github/stars/anic17/Newtrodit?color=yellow)
![newtrodit-forks](https://img.shields.io/github/forks/anic17/Newtrodit)
![newtrodit-stars](https://img.shields.io/github/license/anic17/Newtrodit)
![newtrodit-issues](https://img.shields.io/github/issues/anic17/Newtrodit)

<a href="https://anic17.github.io/Newtrodit/"><img src="https://user-images.githubusercontent.com/58483910/137585130-ca273f69-f83a-4313-a49b-671fb6fe8929.png"></img></a>


# Features

- Easy to use
- Fast and light
- Real time position of cursor
- Line counting
- Various string manipulation functions such as:
  - String finding
  - String replacing
  - ROT13 encoding
  - Uppercase/lowercase conversion



## More features to come

- Syntax highlighting supporting different languages
- Maybe hex editor?

## Small history

### Why I started to code Newtrodit?

It was at a day when I tried to make a clone of the old MS-DOS EDIT.COM in batch, but in the making I realized it would be really complicated to make in batch and performance issues were starting to appear.  
As I was very new to C, it was a challenge to see how good I could recode it, but with more features.  
The UI got changed a lot of times, before it had blue background, and it was a lot different.  
I started to love coding it, and that's why today I'm continuing to develop Newtrodit

## Project building

Compiling and running Newtrodit doesn't require any external libraries apart from the WinAPI simple libraries. Build Newtrodit with the following command line:  
`tcc newtrodit.c -o Newtrodit.exe -luser32`  

Or if you prefer using GCC:  
`gcc Newtrodit.c -o Newtrodit.exe -luser32 -O2`

## Bug reports

To report a bug, feel free to create an issue explaining the bug and the way to get it. Contribution is highly appreciated as Newtrodit is still on beta, so there's a bunch of bugs needing to be fixed.

## Contributing

If you want to contribute to Newtrodit, fork the project and create a pull request with the changes you want to do, describing a bit the changes you did.


## Manual
To get information about the usage of Newtrodit, press F1 at Newtrodit or run `newtrodit --help`  
It shows all the information you need to know to use Newtrodit. If you have any doubts, contact me on <a href="https://discord.gg/J628dBqQgb" style="text-decoration: none">Discord</a>.

## Contact

Join the Newtrodit development channel!  
<a href="https://discord.gg/J628dBqQgb"><img src="https://img.shields.io/discord/728958932210679869"></a>


## Release

Newtrodit will be released in a short time, but you can compile using <a href="https://bellard.org/tcc/" style="text-decoration: none">TCC</a> and running the script `make.bat`


#### Copyright (c) 2021 anic17 Software
</div>


<!-- 
View counter 
-->
<img src="https://hits.seeyoufarm.com/api/count/incr/badge.svg?url=https%3A%2F%2Fgithub.com%2Fanic17%2FNewetrodit&count_bg=%23FFFFFF&title_bg=%23FFFFFF&icon=&icon_color=%23FFFFFF&title=hits&edge_flat=false" style="display:none" height=0 width=0>

