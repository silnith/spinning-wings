# WingsSaver

This is the MacOS screensaver.

A screensaver on MacOS X is not like a normal application.  It cannot be
launched directly.  It must be invoked by the System Preferences application.
That makes debugging difficult, hence why the SpinningWings application exists.

Originally MacOS X screensavers were extensions.  They were loaded by System
Preferences.  As the security posture of Apple changed over the years this
design was removed and instead a separate screensaver application created that
loads a "legacy" screensaver application extension, which in turn runs the
screensavers.  This change seems to have happened in Sonoma.

Unfortunately, this legacy system has a few problems.  It seems to have
difficulty actually determining when to terminate a screensaver, and there are
reports from a wide variety of sources that screensavers continue running in
the background after they should have been terminated.  Furthermore, the system
will sometimes reuse an existing zombie screensaver rather than start a new one,
causing rendering problems and failures to update from modified preferences.
Apple has acknowledged the problems but provided no timeline for fixes or any
alternative screensaver mechanism.

TODO: Add a listener for the `com.apple.screensaver.willstop` event, and shut
down manually.

[GitHub thread](https://github.com/JohnCoates/Aerial/issues/1305)

## Idiosyncrasies

There are a couple idiosyncrasies of bundles that link against the ScreenSaver
framework.

### App Icon

Screensavers are not normal applications, so including an AppIcon asset does
nothing.  Although Xcode will use the AppIcon asset for the icon of the build
target, the icon will not appear anywhere else once the bundle is built and
distributed.  Finder and System Preferences both ignore the AppIcon.

### Thumbnail Image

This feature appears to be completely undocumented.  If a screensaver project
happens to contain two images meeting these criteria:

1. Named `thumbnail.png` with dimensions of 90 by 58 pixels.
1. Named `thumbnail@2x.png` with dimensions of 180 by 116 pixels.

Then these two images will be combined into a file `thumbnail.tiff` and placed
in the Resources folder of the resulting bundle.  The System Preferences
application will then use this image as the icon for the screensaver in the
list of screensavers presented to the user.

This thumbnail will not be used by the Finder in any way.
