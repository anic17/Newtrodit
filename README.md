> [!NOTE]
>  Newtrodit is undergoing a complete code rework, which will significantly enhance its performance, cross-compatibility, support for 24-bit RGB colors, memory management, and large file handling. Additionally, it will offer full Unicode support! 

<a href="https://github.com/anic17/Newtrodit"><img src="https://github.com/anic17/Newtrodit/blob/main/res/logo_transp.png" align="right" width="150" height="150" /></a>

#### [💡 Features](https://github.com/anic17/Newtrodit#features) - [📖 Compatibility](https://github.com/anic17/Newtrodit#compatibility) - [⚙️ Configuration](https://github.com/anic17/Newtrodit#configuration) - [🔨 Build](https://github.com/anic17/Newtrodit#building) - [❔ Contributors](https://github.com/anic17/Newtrodit#contributors)

# Newtrodit <br><a href="https://github.com/anic17/Newtrodit/stargazers">![newtrodit-stars](https://img.shields.io/github/stars/anic17/Newtrodit?color=yellow&style=flat-square)</a> <a href="https://github.com/anic17/Newtrodit/network/members">![newtrodit-forks](https://img.shields.io/github/forks/anic17/Newtrodit?style=flat-square)</a> <a href="https://www.gnu.org/licenses/gpl-3.0">![newtrodit-license](https://img.shields.io/github/license/anic17/Newtrodit?style=flat-square)</a> <a href="https://github.com/anic17/Newtrodit/issues">![newtrodit-issues](https://img.shields.io/github/issues/anic17/Newtrodit?style=flat-square)</a> <a href="https://github.com/anic17/Newtrodit/pulls">![newtrodit-issues](https://img.shields.io/github/issues-pr/anic17/Newtrodit?style=flat-square)</a>

**Efficient and simple** yet powerful console text editor written in C

###### Editing a C header file with Newtrodit
<a href="https://anic17.github.io/Newtrodit/"><img src="../main/res/screenshot_main.png"></img></a>
**See the [Newtrodit website](https://anic17.github.io/Newtrodit) for more information**

## Features
- Fast and light
- Real time cursor position
- Line numbering (with highlighting of the current line)
- Highly configurable
- Various string manipulation functions such as:
   - String finding (case sensitive and insensitive)
   - String replacing
   - ROT13 encoding
   - Uppercase/lowercase conversion
- Fully configurable syntax highlighting including custom rules
- Code autocompletion based on syntax highlighting rules
- Built-in manual
- Mouse support (Windows exclusive)
- Can edit files up to 6400 lines
- Some file utilities:
   - File compare
   - File locating
   - File type checking
- Customizable macros
- Insertion of current time
- Up to 64 files open at once
- Vim-like simple command palette

## Small history
### Why I started to code Newtrodit?

It was a day when I tried to make a clone of the old MS-DOS EDIT.COM in batch, but in the making, I realized it would be complicated to make it in batch, and performance issues were starting to appear.  
As I was very new to C, it was a challenge to see how well I could recode it, but with more features. The UI slowly changed, having a blue background and lateral bars. I realized I wanted to make my editor and not just a clone, and that's why today I'm continuing to develop Newtrodit.
The name "Newtrodit" comes from 3 words: _neutron_ (hence the atom logo), _edit_, and _new_.

## Building
Compiling and running Newtrodit doesn't require any external libraries apart from the WinAPI simple libraries. Build Newtrodit with the following command line:  
`tcc newtrodit.c -o newtrodit.exe -luser32`  

Or if you prefer using GCC:  
`gcc newtrodit.c -o newtrodit.exe -luser32 -O2`

## Compatibility
Newtrodit is compatible with OSes starting from Windows XP, but you can enable a small support for Windows 95/98/ME by changing the `_NEWTRODIT_OLD_SUPPORT` constant to **1** in [`newtrodit_core.h`](src/win32/newtrodit_core_win.h#L44).  However, this isn't recommended for regular builds, as the compiled executable will lack some features such as UTF-8 file reading.

## Bug reports
To report a bug, feel free to create an issue explaining the bug and the way to get it. Contribution is highly appreciated as Newtrodit is still in beta, so there are a bunch of bugs that need to be fixed.

## Contributing
If you want to contribute to Newtrodit, fork the project and create a pull request with the changes you want to do, describing the changes you made.

## Documentation
To get information about the usage of Newtrodit, press F1 at Newtrodit or run `newtrodit --help`  
The documentation contains everything you need to know to use Newtrodit. If you have any questions, contact me on <a href="https://discord.gg/J628dBqQgb" style="text-decoration: none">Discord</a>.

## Configuration

Newtrodit offers some configuration capabilities by editing the file `newtrodit.config`, with the purpose of customizing the editor as per your preferences. The file format is INI-like. All the information required can be found in the documentation, with an example configuration file. Keep in mind that incorrectly modifying Newtrodit's settings can lead to unexpected issues.  
For an example configuration file, you can see an [example](newtrodit.config) of a configuration file.

## Contact
Join the Newtrodit development channel on Discord!  
<a href="https://discord.gg/J628dBqQgb"><img src="https://img.shields.io/discord/728958932210679869?style=flat-square&color=blue"></a>

## Release
Newtrodit will be released in a short time, but you can compile using <a href="https://bellard.org/tcc/" style="text-decoration: none">TCC</a> and running the script [`make.bat`](../main/make.bat)
Or you can run the binaries inside the [`bin`](../main/bin/) directory.

Meanwhile, you can enjoy the 0.6 release candidate 1 version! Keep in mind that it has some known issues which have to be fixed. See more in the [security readme file](../main/SECURITY.md).


## Newtrodit-LCL

Newtrodit Linux Compatibility Layer (aka Newtrodit-LCL) is, as the name says, a compatibility layer to run Newtrodit on Linux natively. It is currently being developed by **[@ZackeryRSmith](https://github.com/ZackeryRSmith)** and myself (anic17). Newtrodit-LCL will be released when Newtrodit 0.6 (the actual release) comes out, which shouldn't take long.

<br />
<a name="contributors"></a>
<b>Huge</b> thanks to <i>all</i> the contributers who helped make Newtrodit possible! ❤️
<hr>
<table align="center">
  <tr>
    <td align="center"><a href="https://github.com/anic17"><img src="https://avatars.githubusercontent.com/u/58483910?v=4?s=100" width="100px;" /><br /><sub><b>anic17</b></sub></a><br /><a href="" title="Maintainer">:hammer:</a> <a href="" title="Code">:computer:</a></td>
    <td align="center"><a href="https://github.com/ZackeryRSmith"><img src="https://avatars.githubusercontent.com/u/72983221?v=4?s=100" width="100px;" alt=""/><br /><sub><b>ZackeryRSmith</b></sub></a><br /><a href="" title="Code">:computer:</a></td>
    <td align="center"><a href="https://github.com/timlg07"><img src="https://avatars.githubusercontent.com/u/33633786?v=4?s=100" width="100px;" /><br /><sub><b>timlg07</b></sub></a><br /><a href="" title="Bug fixes">:bug:</a></td>
    <td align="center"><a href="https://github.com/KeithBrown39423"><img src="https://avatars.githubusercontent.com/u/74946768?v=4?s=100" width="100px;" /><br /><sub><b>KeithBrown39423</b></sub></a><br /><a href="" title="Bug fixes">:bug:</a></td>
    <td align="center"><a href="https://github.com/TheKvc"><img src="https://avatars.githubusercontent.com/u/46124093?v=4?s=100" width="100px;" /><br /><sub><b>TheKvc</b></sub></a><br /><a href="" title="Testing">:test_tube:</a></td>
  </tr>
</table>

<hr>

**Copyright &copy; 2025 anic17 Software**

<!-- 
View counter 
-->
<img src="https://hits.seeyoufarm.com/api/count/incr/badge.svg?url=https%3A%2F%2Fgithub.com%2Fanic17%2FNewtrodit&count_bg=%23FFFFFF&title_bg=%23FFFFFF&icon=&icon_color=%23FFFFFF&title=hits&edge_flat=false" style="display:none" height=0 width=0>

