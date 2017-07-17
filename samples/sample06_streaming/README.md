sample06 streaming
====
This sample shows you how to use streaming functions. Source of sound data for streaming is wave (*.wav) file only.<br>
Read sample[01,02,03,04,05]/README.md before reading this content.<br>
No picture.

General rules
----
Below things are general rules

 * Sound data is created during play
 * Streaming doesn't use id

Description structures
----
This library features structure and function based interface. Some functions with alot of input data uses data description structures like some Windows library functions do.

1. StreamingDesc
```
struct sys::StreamingDesc {
  ResourceDesc resource_desc;
  bool use_loop;
  StreamingDesc() :
    resource_desc(),
    use_loop(false) { }
};
```
This structure describes sound data properties. StreamingDesc includes ResourceDesc. If use_loop is set, music is looped until  be stopped by StopStreaming.

Streaming functions
----
These are some function related to sound play.

1. PlayStreaming
```
bool sys::PlayStreaming(const StreamingDesc& desc);
```
This function starts to stream a wave file. This library is only able to play one wave file at a time. If StreamingDesc::use_loop is set, streaming will never stop until StopStreaming is called.

2. PauseStreaming
```
bool sys::PauseStreaming();
```
This function pauses streaming until ContinueStreaming or StopStreaming is called. If former function is called, streaming is restart form stopped cursor position. If StopStreaming is called, current streaming is aborted.

3. ContinueStreaming
```
bool sys::ContinueStreaming();
```
This function restarts streaming that paused by PauseStreaming from current cursor position.

4. StopStreaming
```
bool sys::StopStreaming();
```
This function stops streaming.

Credits
----
Copyright of files below goes to sound maker "[魔王魂](http://maoudamashii.jokersounds.com/)".<br>

 * bgm_maoudamashii_8bit25.wav (bgm_maoudamashii_8bit25.mp3)
