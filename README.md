Real-Time-Rendering-DirectX
===========================

What is it?
------------
A small application demonstrating Shadows, Bump Mapping, Anti-Aliasing and a Quaternion Camera under DirectX 10.
Besides using the keyboard and mouse to navigate through the scene, there's also a journey mode in which the camera automatically traverses control-points on the basis of a [Catmull-Rom interpolation](http://en.wikipedia.org/wiki/Cubic_Hermite_spline#Catmull.E2.80.93Rom_spline). Furthermore, the view's direction follows a [spherical linear interpolation](http://en.wikipedia.org/wiki/Slerp).

![Screenshot](/docs/Screenshot.png?raw=true)

How to play?
------------
| Key             | Action  |
| --------------: | :-------|
| ESC             | Exit    |
| W, S, A, D      | Move camera front (W), back (S), left (A) or right (D) |
| Arrow Keys      | Move light  front (↑), back (↓), left (←) or right (→) |
| Mouse movement  | Change view direction |
| Q, E            | Roll camera left (Q) or right (E) |
| SPACE           | Save current position as control-point for interpolated camera journey |
| ENTER           | Start interpolated camera journey (requires at least 4 saved control-points |
| 1               | Activate Bump Mapping and Shadow Mapping |
| 2               | Activate Shadow Mapping only |
| 3               | Enable wireframe mode |
| 4               | Disable wireframe mode |
| 5 followed by 6 | Toggle Multisampling anti-aliasing (MSAA); Sample-Count at beginning: 8x |

Requirements
------------
What you need for compiling:
* Visual C++ or rather Visual Studio 2010 or above
* DirectX SDK June 2010 (http://www.microsoft.com/en-us/download/details.aspx?id=6812)

How to compile?
------------
1. Get the source code and open the project in Visual Studio
2. In the menu bar, click on VIEW and open the PROPERTY MANAGER
3. In the Property Manager Window, expand the tree and double click "Microsoft.CPP.Win32.user" under "Debug | Win32"
4. You will see the Property Pages Window. Click VC++ Directories
5. Edit the Include Directories and add the include directory of your DirectX SDK, e.g. "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include".
6. Click okay and edit the Library Directories. Add the Lib Directories of your DirectX SDK, e.g. "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x86" and "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x64"
7. Close the Property Pages Window and repeat steps 4-9 but with "Microsoft.CPP.Win32.user" under "Release | Win32"
8. Hit Build :)

Where to get help?
------------
* Check the docs directory for documentation
* Have a look at the [project's website](http://www.pertiller.me/projects/)
* Contact the author at david@pertiller.me

License
------------
(The MIT License)

Copyright (c) 2012 [David Pertiller](http://www.pertiller.me)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
