DemonPro
========


License
-------

See [LICENSE](LICENSE).


Parameters
----------


Building
--------

### Prerequisites

**VST3 SDK**

Download [VST 3 Audio Plug-Ins SDK](https://www.steinberg.net/en/company/developers.html)
and extract it anywhere you wish, but remember that the directory you choose
will be the "installation path" for the SDK.

**fftw**

`fftw` is required by the exciter unit of this plugin effect.

On MacOS you can download `fftw` using the script in the `fftw` directory. This
will download and extract `fftw` for both x86-64 and ARM64 targets.

```
cd fftw
./runme.sh
```

### Running the build

Now you are ready to build the actual plugin.

```
mkdir build
cd build
cmake -DSMTG_RUN_VST_VALIDATOR=OFF -DSMTG_ADD_VSTGUI=ON -G"Xcode" -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

or make a release build by running:

```
mkdir build
cd build
cmake -DSMTG_RUN_VST_VALIDATOR=OFF -DSMTG_ADD_VSTGUI=ON -G"Xcode" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

If everything went fine the build script should make a symlink in the system VST
plugin path, pointing to the build result of the build. Meaning that you can now
start your DAW/VST host and it should be able to discover the plugin. The
resulting bundle should work on both x86-64 and ARM64 (M1) Macs.

#### Code signing

If you want to sign the binary for distribution the easiest way to sign and
staple the VST3 plugin is shown here.

First you should create an Application Specific Password for the project as
explained [here](https://support.apple.com/en-us/HT204397).
Then run the following command to store the credentials:

```
xcrun notarytool store-credentials --apple-id "youremail" --team-id "team-id"
```

This step needs to be done only once.

Next you need to sign the VST3 plugin:

```
codesign --force -s "Developer ID Application" VST3/Release/DemonPro.vst3
```

Then zip the plugin and run `notarytool` for the zip file:

```
/usr/bin/ditto -c -k --keepParent VST3/Release/DemonPro.vst3 DemonPro.zip
xcrun notarytool submit DemonPro.zip --keychain-profile "DemonPro" --wait
```

If that step passed then you can staple the plugin:

```
 xcrun stapler staple VST3/Release/DemonPro.vst3
```

and remove the intermediate zip:

```
rm DemonPro.zip
```


### Troubleshooting

Some pointers:

**I can't see the plugin in my DAW**

Some DAWs don't look from all possible "legal" VST paths nor it's configurable
You might need to copy the symlink or the bundle/binary to a path that your DAW
supports.

Editing the Interface
---------------------

```
../../VST_SDK/VST3_SDK/build/bin/Debug/editorhost.app/Contents/MacOS/editorhost VST3/Debug/DemonPro.vst3
```

Finally *Save As* on top of `resource/editor.uidesc`. (*Save* should work too,
on the latest SDK version).
