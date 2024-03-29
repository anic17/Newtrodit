# Security Policy

## Supported Versions

As Newtrodit is currently in beta, only newer versions will be supported. Only report old vulnerabilities if they still exist on the latest supported version, and not on outdated versions.  

These are the known bugs:  
 - `^R` (file reloading) trims lines longer than 640 bytes and causes display errors
 - Syntax highlighting doesn't display correctly if the file is scrolled horizontally
 - Selection causes many visual bugs
 - Scrolling a file makes the cursor stick to the last line (not really a bug, just a badly-implemented feature)
 - Find always shows "No more matches"

If you find a bug that isn't on this list, please report it.

List of supported versions:

| Version | Supported          |
| ------- | ------------------ |
| 0.6     | :white_check_mark: |
| 0.5     | :white_check_mark: |
| 0.4     | :x:                |
| 0.3     | :x:                |
| 0.2     | :x:                |
| 0.1     | :x:                |


## Reporting a Vulnerability

To report a vulnerability, create an [issue](https://github.com/anic17/Newtrodit/issues) with the `vulnerability` tag
When reporting a crash, please add the following information:

 - What function where you using before the crash
 - All the information in the crash screen
 - The version you were using, the build date, compilation time. This data can be gathered with `newtrodit -v`
 
 Create an issue and I'll try to fix it as soon as possible, if you know what could be the cause, please let me know.
