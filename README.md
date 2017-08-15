System Library
====

Windowsで動くゲームを作るためのDirectXを使用したライブラリです。<br>
2D画像の表示、音声の再生、キーボード・ジョイパッド（ゲームパッド）入力の処理を行うことができます。また、wavファイルのストリーミング再生をサポートします。<br>

Translation:<br>
The library for game development on Windows with DirectX inside.<br>
The System Library provides you 2D graphics, sound and keyboard and Joypad, or Gamepad, input functions. It also enables your game to play the streaming of a wave file.

特徴 (Features)
----

 * 構造体と関数ベースの設計
 * ドットの鮮明な描画が可能（補完なし）
 * Vecmath (Java(TM) 3D API 1.2.) のC++実装を使用
 * スタティックリンクライブラリ (system_lib.lib) で機能を提供
 * MIT ライセンス

Translation:<br>

 * The interface is based on structures and functions
 * Good at dot plotting (No interpolation)
 * Vecmath (Java(TM) 3D API 1.2.) C++ implementation is used
 * The functions are provided with a static link library (system_lib.lib)
 * Available under the MIT License

要件 (Requirements)
----

このライブラリを使用するにあたり、事前に以下の開発環境およびライブラリがインストールされている必要があります。<br>

 * [Microsoft Visual Studio Community 2017](https://www.microsoft.com/ja-jp/dev/products/community.aspx)<br>
   Microsoftが提供する統合開発環境です。Visual C++で開発を行います。
 * [Windows SDK](https://developer.microsoft.com/ja-jp/windows/downloads/windows-8-1-sdk)<br>
   Windowsアプリ開発のためにVisual Studioとは別途で導入が必要なライブラリです。
 * [Microsoft DirectX SDK (June 2010)](https://www.microsoft.com/en-us/download/details.aspx?id=6812)<br>
   高速に描画や音声再生、入力などの処理を行うことができるAPIです。
 * [vecmath](http://objectclub.jp/download/vecmath1) (API 1.2 C++ port)<br>
   Java(TM) 3D API 1.2.で提供されている線形代数などを扱いやすくするライブラリです。C++での実装が公開されており、そちらを使用します。

Translation:<br>
The system library requires libraries shown below are installed to your computer befor use.<br>

 * [Microsoft Visual Studio Community 2017](https://www.microsoft.com/ja-jp/dev/products/community.aspx)<br>
   This is an IDE from Microsoft. Visual C++ is needed bor the build of the product.
 * [Windows SDK](https://developer.microsoft.com/ja-jp/windows/downloads/windows-8-1-sdk)<br>
 This is the library for the development of Windows applications that is installed separately to the above IDE.
 * [Microsoft DirectX SDK (June 2010)](https://www.microsoft.com/en-us/download/details.aspx?id=6812)<br>
 This is the API for fast rendering, sound playing and input processing.
 * [vecmath](http://objectclub.jp/download/vecmath1) (API 1.2 C++ port)<br>
   This is the library originated in Java(TM) 3D API 1.2.. The C++ implementation is published. The system library uses C++ implementation.

仕様 (Specifications)
----

 * 描画機能
   * 使用可能な画像ファイル: png, jpeg, gifなど
   * 画像データはライブラリ内部で管理
   * 自動で割り振られるIDによる画像データの管理
   * フォントの画像を使用したテキスト描画（大文字アルファベット、数字、記号）
 * 音声機能
   * 使用可能な音声ファイル: wav
   * 自動で割り振られるIDによる音声データの管理
   * ID指定による画像の再生などの処理が可能
   * ストリーミング再生が可能
 * 入力機能
   * キーボード入力の取得
   * ジョイパッド入力の取得
   * 上記2入力を統合した「仮想入力」の取得

Translation:<br>

 * Graphics
	* Available formats: png, jpeg, gif, etc.
	* The image data is buffered inside the library.
	* The ID is served to distinguish image resources inside the library.
	* The text is drawn using font table images.
 * Sound
	* Available formats: wav
	* The sound data is buffered inside the library.
	* The ID is served to distinguish sound resources inside the library.
	* The streaming of an wav data is supported.
 * Input
	* Keyboard input is acquired.
	* Joypad, or Gamepad, input is acquired.
	* The mixture of above two input is acquired as "the virtual input".

ビルド (Build)
----

このライブラリを使用したビルドを行う際には、以下のことが必要です。<br>

 * 要件の記述にあるライブラリやAPIの必要なヘッダファイルとリンクファイルのある場所に、環境変数`INCLUDE`、`LIB`、`LIBPATH`が通っていること
 * このライブラリのディレクトリに環境変数`INCLUDE`、`LIB`、`LIBPATH`が通っていること
 * リンク時に以下のスタティックリンクライブラリを指定すること

```
"kernel32.lib"
"user32.lib"
"gdi32.lib"
"winspool.lib"
"comdlg32.lib"
"advapi32.lib"
"shell32.lib"
"ole32.lib"
"oleaut32.lib"
"uuid.lib"
"odbc32.lib"
"odbccp32.lib"
"dsound.lib"
"winmm.lib"
"Shcore.lib"
"winmm.lib"
"d3d11.lib"
"d3dx11.lib"
"dinput8.lib"
"dxguid.lib"
"system.lib"
```

Translation:<br>
To build an application using the system library, you need following things:

 * The environmental variables, `INCLUDE`, `LIB` and `LIBPATH` are set to the libraries mentioned in the Requirements.
 * The environmental variables, `INCLUDE`, `LIB` and `LIBPATH` are set to the system library installation directory.
 * These static link libraries are must be specified when linking.<br>
 [The list above]

サンプルプロジェクトと使い方の説明 (Samples and instructions)
----
システムライブラリの使用例とデータ・関数の説明を行います。<br>
詳しくは、[samples/](samples)以下の各ファイルのREADMEを参照してください<br>
ここに目次となるリンクを張っておきます。

|サンプルフォルダ|説明|
|---|---|
|[samples/sample01_window](samples/sample01_window)|このサンプルでは、基本的なウィンドウの作り方と、システムライブラリを動かすために必ず呼び出しが必要な関数などの説明を行います。詳しくは[README.md](samples/sample01_window/README.md)を参照してください。|
|[samples/sample02_picture](samples/sample02_picture)|このサンプルでは、画像をウィンドウに描画するアプリケーションを通して、画像データの扱い方と、画像データを扱ううえで必要なデータや関数などの説明を行います。詳しくは[README.md](samples/sample02_picture/README.md)を参照してください。|
|[samples/sample03_font](samples/sample03_font)|このサンプルでは、テキストを画面に描画するプログラムを通して、フォント画像の扱い方と、テキストを扱ううえで必要なデータや関数などの説明を行います。詳しくは[README.md](samples/sample03_font/README.md)を参照してください。|
|[samples/sample04_input](samples/sample04_input)|このサンプルでは、入力デバイスの入力状況を画面に表示するアプリケーションを通して、必要なデータや関数などの説明を行います。詳しくは[README.md](samples/sample04_input/README.md)を参照してください。|
|[samples/sample05_sound](samples/sample05_sound)|このサンプルでは、音声を再生するアプリケーションを通して、音声データの扱い方と、音声を扱ううえで必要なデータや関数などの説明を行います。詳しくは[README.md](samples/sample05_sound/README.md)を参照してください。|
|[samples/sample06_streaming](samples/sample06_streaming)|このサンプルでは、大きめの音声ファイルをストリーミング再生するアプリケーションを通して、音声のストリーミング再生を行ううえで必要なデータや関数などの説明を行います。詳しくは[README.md](samples/sample06_streaming/README.md)を参照してください。|

Translation:<br>
Samples and descriptions about data and functions are provided.<br>
For details, read README in each sample directory.<br>
The contents are shown in the table below:

|Folder|Description|
|---|---|
|[samples/sample01_window](samples/sample01_window)|In this sample, you learn how to create a plain window and learn about functions that are required to run the application with the system library. For more details, see [README.md](samples/sample01_window/README.md).|
|[samples/sample02_picture](samples/sample02_picture)|In this sample, you learn how to manipulate an image data and learn about structures and functions that are needed to the handling of image data through the application that draws images on the screen. For more details, see [README.md](samples/sample02_picture/README.md).|
|[samples/sample03_font](samples/sample03_font)|In this sample, you learn how to manipulate an font data and learn about structures and functions that are needed to the handling of font data through the application that draws texts on the screen. For more details, see [README.md](samples/sample03_font/README.md)|
|[samples/sample04_input](samples/sample04_input)|In this sample, you learn how to process the input and learn structures and functions needed to the handling of input data through the application that draws input status on the screen. For more details, see [README.md](samples/sample04_input/README.md).|
|[samples/sample05_sound](samples/sample05_sound)|In this sample, you learn how to manipulate an sound data and learn about structures and functions that are needed to the handling of sound data through the application that plays sounds from speakers. For more details, see [README.md](samples/sample05_sound/README.md).|
|[samples/sample06_streaming](samples/sample06_streaming)|In this sample, you learn how to manipulate an streaming data and learn about structures and functions that are needed to the handling of streaming data through the application that plays streaming from speakers. For more details, see [README.md](samples/sample06_streaming/README.md).|

バグレポートや改善の提案など (Contributing)
----

バグレポートや改善の提案などは、issueを立てて伝えてください。<br>
詳しくは [CONTRIBUTING.md](CONTRIBUTING.md) を見てください。<br>


Translation:<br>
For bug reports and suggestions for improvements, make an issue.<br>
For more details, see [CONTRIBUTING.md](CONTRIBUTING.md).

ライセンス (License)
----

MIT ライセンスで公開します。<br>
詳しくは [LICENSE.md](LICENSE.md) を見てください。<br>


Translation:<br>
This software is released under the MIT License, see "LICENSE.md".
