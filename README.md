# Spinning Wings

A graphics hack of spinning wings.

This is available under the AGPL 3.0 license for those who want it.

## History

When I was in college studying computer science, a friend of mine got ahold of an SGI.
The big 20" or so blue monitor sat on his desk every day, showing some spinning quads
that were so mesmerizing that every person who walked into his dorm room would eventually
stop and just stare at it, sometimes for minutes at a time before they were brought
back to reality.

This program was called `electropaint`.

Fast forward to graduate school.  I am studying computer graphics at the University of
North Carolina at Chapel Hill.  I get ahold of my own used SGI, but the previous owner
lost the passwords.  Annoyed at being so close to seeing `electropaint` again but being
stymied, I write a cleanroom copy purely from memory.  I make it available in one of the
public source code repository systems, but a while later I get a legal cease-and-desist
from the original `electropaint` program author, because I had published my repository
under the name `electropaint`.  I take down the repository and stash the source code
on one of my computers somewhere.

Nearly twenty years later I find the code again, and decide I should make it public
again, but under a unique name to make it clear that it shares absolutely no source
code with the program `electropaint` that was distributed with SGI Irix.  Here it is,
"spinning wings".

### Differences

First off, the win32 directory is a native Windows application and screensaver.
The cocoa directory is a native MacOS screensaver.  The core uses OpenGL 1.1 as
the rendering API.  I used the win32 version as an excuse to brush up on my
Windows API knowledge.  It does not use MFC or any other windowing toolkit.
It does not use GLUT, GLEW, or any other OpenGL extension-loading library.
(The original I wrote in school used GLUT.)  The MacOS version is similarly lean.

More importantly, when I later saw `electropaint` running on an SGI again, I realized
that my memory had been faulty.  I had recalled it as being very fluid and mezmerizing.
Therefore, when I wrote code to generate "random" curves to control the movement of the
quads, I used second-order polynomials to model the curves.  (Specifically, `x^2`, as in
acceleration.)  When I saw `electropaint` again, I realized it only used first-order
polynomials.  If you view it versus this side-by-side, the difference is quite obvious.

# Current Version

## Win32

This version has been cleaned up and refactored from the original source code I wrote
in school.  Most prominently it now uses the Win32 API rather than GLUT.  I also
improved and refined its use of STL and the encapsulation of the utility code that
generates the randomized curves.

The use of the Windows API is minimal.  Every function called should be available as
far back as Windows 2000.  (Possibly older, but Windows 2000 is the oldest version
documented in Microsoft's online documentation.)  Building using Visual Studio should
generate a manifest file that identifies the executable as per-monitor DPI-aware,
though the program has no text elements that need to be scaled.  The purpose of marking
the executable thusly is to prevent Windows from attempting to perform any sort of
bitmap-based window scaling.

### Multiple Versions

Later, after I had everything ported and working correctly, I had the idea of using
this as a testbed to clarify exactly what was different about the various major
versions of OpenGL.  As such, I kept the original OpenGL 1.1 version, but I also
created copies that I rewrote using OpenGL 2.0, 3.2, and 4.1.  The purpose was
to write idiomatic code, to embrace fully the "best practice" for each new version.
Due to the Win32 API only providing headers and libraries for OpenGL 1.1, and
the obscene complexity (and fragility) of trying to load dynamic libraries on
Windows, the versions that make use of newer OpenGL use the glew library, included
as a NuGet package.

#### OpenGL 2.0

The OpenGL 2.0 version makes use of vertex and fragment shaders in order to do
transformations and rendering.  OpenGL 2.0 was still considered a strict superset
of previous versions of OpenGL, so my implementation continues to make use of
various features such as display lists and the projection matrix where it makes sense.

#### OpenGL 3.2

OpenGL 3 introduced the concept of deprecated functionality, and OpenGL 3.2
formalized that with the introduction of the Core profile that actually removes
functionality declared "legacy".  My implementation uses the new WGL extensions
to explicitly create a context that implements the OpenGL 3.2 Core profile, and
solely uses the "modern" functionality.  The biggest change is that capturing
wing-specific transformations in a display list is no longer possible.  Instead,
The OpenGL 3.2 implementation has a separate vertex shader that re-implements
those transformations, and as each wing is created the wing geometry is passed
through this shader and captured using transform feedback into buffers.  When
all the wings are then rendered to the display, each wing's transformed geometry
is passed to a completely different vertex shader (and associated fragment shader)
to do the final rendering.

#### OpenGL 4.1

The OpenGL 4.1 version makes use of some additional functionality associated with
the transform feedback feature to save additional state in graphics memory instead
of passing it along each time the shaders are changed.

## Cocoa

The MacOS screensaver is written in Objective-C, because my understanding is that Apple
has not made Swift safe to use for screensavers yet.  It tries to use the native display
resolution and color range where possible, so it should take advantage of "retina displays".
