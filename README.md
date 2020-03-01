# AudioShake

The main goal of this project is to create a lightweight audio ducking program. 

This need arises while playing games. If the user wants to communicate through
an application such as TeamSpeak, it can be difficult to get the volumes of both TeamSpeak
and the game at optimal levels. 

This program will monitor the audio output of a user-indicated "Primary" program(s). When 
there is output from the Primary program, all other user-indicated audio 
sources will be dimmed to allow the user to better hear the Primary program. The non-primary
sources will be brought up to their previous levels as soon as the Primary program is silent.

The user can also designate programs to mute on a keypress. Pressing the key again will
un-mute those programs.
