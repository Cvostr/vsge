exec /usr/bin/mono --gc-params=nursery-size=64m $MONO_OPTIONS /../lib/mono/4.5/csc.exe "$@"
