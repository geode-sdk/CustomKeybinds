## v1.11.0
- Add optional API for developers!

## v1.10.4
- Fix the keybind search box being offset by half its width
- Fix the controller notification sometimes showing a fallback texture

## v1.10.3
- Fixed reset button appearing for default keybinds
- Fixed command/control and arrow key keybinds not working on MacOS
  - Thanks hiimjasmine00!

## v1.10.2
- MacOS support
  - Thanks hiimjasmine00!

## v1.10.1
 - Fixed an issue where all keybinds could get unset on a fresh installation

## v1.10.0
 - Updated for GD 2.2074

## v1.9.0
 - Unseparate jump/move/checkpoint keybinds for different level modes (1P/2P, Classic/Platformer)
 - Fix outdated controller defaults

## v1.8.0
 - Separate jump/move/checkpoint keybinds for different level modes (1P/2P, Classic/Platformer)
 - Offset binds in inner categories to improve readability
 - Enable controller support on Android
 - Bump to Geode `v3.6.0`

## v1.7.1

 - Fix modifiers not working with mouse buttons. This most likely breaks existing mouse button bindings
 - Restart keybind no longer requires "Quick Keys" option enabled, as it ended up confusing users
 - Internal fixes

## v1.7.0

 - Allow assigning mouse side buttons as keybinds
 - Add Android support

## v1.6.4

 - Fix spacebar in PlayLayer popups

## v1.6.3

 - Fix various issues in PlayLayer
 - Restart keybind now requires "Quick Keys" option enabled (like in the vanilla game)

## v1.6.2

 - Re-enable `Pan editor`
 - Bump to Geode `v3.1.0`

## v1.6.1

 - Remove `Pan editor` keybind temporarily
 - Bump to Geode `v3.0.0`

## v1.6.0

 - Bump to Geode v3.0.0-beta.1

## v1.5.10

 - Fix "Key + Modifier" combos behaving weird when repeating

## v1.5.9

 - Bump to Geode v2.0.0-beta.26
 - Fix 'shift' not being passed in CCKeyboardDispatcher::dispatchKeyboardMSG

## v1.5.8

 - Bump to Geode v2.0.0-beta.25
 - Fix Alt getting stuck when alt-tabbing out of the game

## v1.5.7

 - Fix some other PlayLayer binds from stopping PauseLayer binds

## v1.5.6

 - Fix jump-p2 using the wrong PlayLayer function

## v1.5.5

 - Fix player jump breaking the space key in EndLevelLayer and RetryLevelLayer

## v1.5.4

 - Fix repeating binds entering a repeat loop on press
 - Fix Pause Menu not having vanilla functionality with Confirm Exit
 - Fix Player not jumping when holding on death
 - Bump to Geode v2.0.0-beta.24

## v1.5.3

 - Add custom setting that opens the keybind menu
 - Fix modifiers not being properly handled on release
 - Fix "unpause" sometimes not working
 - Fix the "Confirm Exit" popup being able to be spawned multiple times through keybinds
 - Simplify the keybind repeat logic, fixing some bugs in the process
 - Allow multiple key combinations to trigger the same non-repeating bind at once

## v1.5.2

 - Fix unpausing with space also causing the player to jump
 - Add missing "Pan editor while swiping" editor keybind
 - Add missing "Transform" editor keybind

## v1.5.1

 - Fix not being able to hold from respawn in PlayLayer

## v1.5.0

 - Add editor save & load position keybinds
 - Fix a lot of the internal implementations of keybinds to just use GD's implementations, potentially fixing a bunch of inconsistencies in the editor

## v1.4.0
 
 - Fix being able to reset levels during level end animation
 - Remove editor half-move and big-move keybinds, since those are not in the vanilla game and as such should be implemented through mods instead

## v1.3.3

- Fix editor keybinds
- Add F-Keys to editor keybinds
- Add half moves and big moves to editor keybinds

## v1.3.2

- Add DPad Up as default for P1 Jump
- Make keybind info popup wider

## v1.3.1

- Changed the name of the default platformer binds (move left right)
- Made platformer move binds unrepeatable
- Added "W" for Jump P1 as a default
- Fixed non-repeatable binds repeating (a lot)
- Fixed default platformer move binds for P2

## v1.3.0

- Fix controller support
- Add sprites for missing controller buttons (RB, RT, LB, LT)
- Improve controller sprite display in the keybind picker
- Add platformer keybinds
- Add reset, full reset, and toggle hitboxes (this one is experimental)

## v1.2.4

- Add 2.204 support

## v1.2.3

- Fix issues caused by the impostor PlayLayer bug

## v1.2.2

- Fix jump key not working after a level restart

## v1.2.1

- Fix issues with MegaHack
- Add EditorUI controller keybinds
- Improve controller compatibility in UILayer

## v1.1.2

- Fix crashes on exit for Windows

## v1.1.1

- Fix issues on MacOS
- Make repeatable binds false by default
