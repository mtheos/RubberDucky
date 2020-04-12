# RubberDucky

Excuse the mess, Arduino projects don't play nice with structure :'(

Edit: (Mostly) fixed the mess :)

Blank linkes are ignored
Any line that doesn't start with a command is sent as is (including carriage return)

**Commands:**
- ::term => Open a terminal. Requires ::OS to be set
- ::run => Run a program. Requires ::OS to be set
- ::keys => Send key combination. Special keys are handled in specialKeys.cpp but typical form is KEY_\<KEY\>
- ::os => Sets the OS for term/run commands. Options are windows/linux/osx (case insensitive)
- ::sleep => Pause for x milliseconds
- ::delay => Same as sleep.
- ::setdelay => Change the default delay between commands (default 100ms)
- ::rem => Comment, this line is ignored. Comments are whole line or nothing
- ::enter => Send enter key. Short for "::keys KEY_ENTER"

Commands are case insensitive, special keys are not
