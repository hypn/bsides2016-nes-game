# What this is:
Source code for a mini Nintendo NES game (rom) I made as part of a challenge for "[B-Sides Cape Town](https://bsidescapetown.co.za/)" in 2016. 

If you just want to try out the rom, grab [**bsides.nes**](bsides.nes).

It uses lots of code from [Doug Fraker](https://nesdoug.com/) (specifically "Spacy" and "lesson7"), and the challenge required use of a Game Genie code to discover a hidden page with a pin, to complete the challenge.

**Note:** the challenge is no longer active, there's nothing to win, and the pin has been set to a default value.

# More info:

See my [blog post](https://www.hypn.za.net/blog/2016/12/05/bsides-2016-nes-game/) for more information, explanation, and screen shots.

Yes it contains a "Konami code" easter egg.


**Game Genie Codes:**
 Most emulators will allow you to enter "cheat codes", which are usually Game Genie code compatible. Functionality was hidden in this ROM and only accessible with these codes:

 * `AAYKAT` - show the "hacked" screen (challenge success screen)
 * `AELKIT` - show a megaman animation
 * `AEGKIT` - show "Hack The Planet" screen


# Compiling:

**Using Docker:**
```
docker run --rm -v $PWD:/src hypnza/cc65-for-nes bash -c "cd /src && ./compile.sh"
```

**Using Windows:**
* download "cc65" https://cc65.github.io/#Links
* extract somewhere (eg: in to this repo's directory, or parent directory)
* modify "compile.bat" and set `CC65_HOME` to extracted path
* run `compile.bat`

