sample05 sound
====
This sample shows you how to use sound functions. Source of sound data is wave (*.wav) file only.<br>
Read sample[01,02,03,04]/README.md before reading this content.<br>
No picture.

General rules
----
Below things are general rules

 * Sound data must be created before play
 * Sound data  is tied to id

Description structures
----
This library features structure and function based interface. Some functions with alot of input data uses data description structures like some Windows library functions do.

1. WaveDesc
```
struct sys::WaveDesc {
  ResourceDesc resource_desc;
  WaveDesc();
};
```
This structure describes sound data properties. WaveDesc includes ResourceDesc.

Sound play functions
----
These are some function related to sound play.

1. CreateWave
```
bool sys::CreateWave(const WaveDesc& desc, int* wave_id);
```
This function creates a wave data from user designated wave file. The wave data is tagged with identical wave id. You can't use one wave id to multiple waves, despite a wave is released with ReleaseWave. Error and duplicate id assign causes failure (return value is false), triggering error dialog.

2. ReleaseWave
```
bool sys::ReleaseWave(int wave_id);
```
This function releases wave data tagged with wave id. Error and duplicate release causes failure (return value is false), triggering error dialog.

3. PlayWave
```
bool sys::PlayWave(int wave_id);
```
This function play wave data tagged with wave id. If the sound id is invalid or expired, error dialog is triggered.

4. StopWave
```
bool sys::StopWave(int wave_id);
```
This function stop playing wave data tagged with wave id. If the sound id is invalid or expired, error dialog is triggered.

Credits
----
Copyright of files below goes to sound maker "[魔王魂](http://maoudamashii.jokersounds.com/)".<br>

 * sound_a.wav (se_maoudamashii_retro19.wav)
 * sound_b.wav (game_maoudamashii_7_event01.wav)
 * sound_c.wav (se_maoudamashii_se_drink02.wav)

