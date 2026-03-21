# Resources to follow-up on...

https://wadetregaskis.com/how-to-make-a-macos-screen-saver/

https://stackoverflow.com/questions/66861833/audio-keeps-playing-after-screensaver-ends/67161635#67161635

https://stackoverflow.com/questions/16944434/monitoring-screensaver-events-in-osx#16947042

https://zsmb.co/building-a-macos-screen-saver-in-kotlin/#macos-sonoma

http://preserve.mactech.com/articles/mactech/Vol.21/21.06/SaveOurScreens/index.html

http://preserve.mactech.com/articles/mactech/Vol.21/21.07/SaveOurScreens102/index.html

https://www.gabrieluribe.me/blog/how-to-distribute-a-screensaver-on-macos-2022

https://github.com/pedrommcarrasco/Brooklyn

https://github.com/JohnCoates/Aerial/issues/1396#issuecomment-3110063589



## Shutdown

DistributedNotificationCenter.default.addObserver(
            self,
            selector: #selector(MyView.willStop(_:)),
            name: Notification.Name("com.apple.screensaver.willstop"),
            object: nil
        )

@objc func willStop(_ aNotification: Notification) {
    // Do something!
}

## Swift?

The rumor is that Swift screensavers are only supported on MacOS 14.6 or later.

https://github.com/AerialScreensaver/ScreenSaverMinimal

## PowerPC

MacOS 10.5 is the last to support PowerPC natively.
