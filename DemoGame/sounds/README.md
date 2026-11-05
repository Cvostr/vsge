# Replaceable game audio

Drop one or more OGG Vorbis files directly into the folders below. Sound effects
randomly pick a valid file on each playback. `ambient/` and `ending/` pick one
valid track when the game starts and loop it. The complete `sounds/` directory
is copied next to `DemoGame.exe` during the build.

- `steps/` - player footsteps
- `attack/` - player attack, swing and impact sounds
- `monsters_attack/` - monster attack vocals
- `monsters/` - monster idle and alert vocals
- `ambient/` - looped background music; overrides the legacy OGG in the parent folder
- `pickups/` - artifact, health and stamina pickups
- `scare/` - scare and exit-screamer sounds
- `doors/` - door sounds
- `ui/` - interface and flashlight sounds
- `ending/` - looped win music

When an effects folder is empty or its OGG files cannot be decoded, the game
falls back to its built-in procedural sounds. Empty `ambient/` falls back to
`sounds/duhovoy-vopl--latunnaya-pechal.ogg`; if that file is absent or invalid,
no background track is played. Empty `ending/` uses synthesized ending music.
