# ofxTimeline #
![Timeline](http://www.jamesgeorge.org/images/ofxtimeline/github/Header.png)

ofxTimelineは、openFrameworksでタイムラインエディタインターフェースを使用する事が出来るシンプルなアドオンです。このフレームワークを使用する事で、openFrameworksで作られたアプリケーションの中で時間軸に沿ったシーケンスを編集し、そのタイムシーケンスを使って変数のコントロールを行う事が可能になります。

もしもAfter EffectsやFinal Cut、Blenderなどのタイムライン編集に馴染みがあれば、ofxTimelineは快適に使用する事が出来るでしょう。ほんの数行のコードで、スムーズに補完されたカーブ、カラー、ビデオ、オシレーター、オーディオ、3D空間中のカメラなどのタイムラインシーケンスを作る、エディタを使用する事が可能です。

また、サポートされていないタイムベースのデータを編集する為にカスタムトラックを作成し、ofxTimelineを拡張する事も可能です。[カスタムトラックをデザインする](#%E3%82%AB%E3%82%B9%E3%82%BF%E3%83%A0%E3%83%88%E3%83%A9%E3%83%83%E3%82%AF%E3%82%92%E3%83%87%E3%82%B6%E3%82%A4%E3%83%B3%E3%81%99%E3%82%8B)をご参照ください。

Copyright 2011-2012 [James George](http://www.jamesgeorge.org) Co-developed by [YCAM InterLab](http://interlab.ycam.jp/)

Licensed under the Apache License

## 再利用可能なツールの哲学##

ofxTimelineの開発は、クリエイティブコーディングの環境の中で、どのようにGUIベースのツールを作り、メディアを使って創造しインタラクションする新しい方法を切り開く事が出来るか、に対するリサーチの一環として制作されています。

タイムラインのインターフェースは一般的には、After EffectsやCinema4Dといった、高価なアプリケーションの中で見る事が出来ます。そしてそれらは時に非常に複雑で、またオープンソースではありません。
オープンなタイムラインインターフェースの実装も存在しますが、それらは[Blender](http://blender.org), [VLMC](http://www.videolan.org/vlmc/) and [Audacity](http://audacity.sourceforge.net/)といった巨大なプロジェクトに組み込まれてしまっていて、非常に才能にめぐまれたプログラマー達にとってさえも、再利用して何かを作る為に、それらのアプリケーションから特定の部分を抽出するのは困難な作業です。
さらには、そういったプロジェクトのライセンス、[GPL licensing](http://www.gnu.org/copyleft/gpl.html)は、商業利用には限定的なライセンスで、openFrameworksや、Processing、Cinderといったクリエイティブコーディングのプラットフォームに組み込むには難があります。

そういったクリエイティブコーディングのコミュニティが、直感的なアプリケーションを作り出す為の数多くの小さなコンポーネントを開発しようとしたら、どのような新しい可能性を切り開く事が出来るでしょうか。
クリエイティブコーダーと、デザイナーやアーティストとの間にどのようなコラボレーションが産まれるでしょうか。

## 主な特徴 ##
- ほんの数行のコードでopenFrameworksのプロジェクトに組み込む事が可能
- ミリ秒単位のタイミング精度、時間のドリフトがない再生機能
- カーブ、カラー、サムネイル付きのビデオトラックといった、複数のタイプのトラックが初めから組み込まれています
- マルチセレクトや、コピー＆ペースト、ポイントへのスナップ、十字キーでの細かな操作など、効果的な編集を可能にする、使いやすい操作性
- 全体を見渡しながらの編集や、微細な時間を詳細に編集可能にするズーム機能
- XMLファイルへの保存と読み込み
- 複数ページをサポート
- BPMの表示機能と、それに連動したスナップ機能ップさせる事が可能
- 簡単に独自のタイプのトラックを追加する事が可能


## はじめに

まず初めに、ビデオチュートリアルを下記のURLからご覧になることが出来ます：
https://vimeo.com/59653952


## ofxTimelineの構造
![Anatomy](http://www.jamesgeorge.org/images/ofxtimeline/github/AnatomyOfTimeline.jpg)

### 1 ページ

タイムラインをページごとに分ける事が出来ます。それぞれのページには、それぞれ別々のトラックのセットが入っています。

### 2 目盛りと再生ヘッド

ヘッダーには、分、秒、ミリ秒の目盛りが表示されています。ヘッダーの中をクリックすると、再生ヘッドをスクラブする事が可能です。タイムラインを縦断している緑色のラインは現在の再生ヘッダの位置を、赤いラインはマウスカーソル上の位置を示しています。

### 3 イン点/アウト点の設定

この目盛りの下に配置されたバーでは、タイムライン上のイン点とアウト点をドラッグして設定する事が出来ます。このイン点とアウト点で設定されたエリアが、再生されるエリアになります。

### 4 トラック

このタイムラインの主要な部分である、複数の編集可能なトラックが表示された部分です。各トラックはサイズ変更する事も可能です。

### 5 ズーム

トラック最下部のコントロールを用いてズームインすることで、時間をより厳密にコントロールする事が可能です。

## Examples ##

### Simple Curves
Simple Curves shows how to use two keyframes and a switcher to animate and color a cube
Simple Curvesでは2つのキーフレームと1つのスイッチャーを使用し、キューブの色と動きをコントロールする例を紹介しています。
![SimpleTween](http://www.jamesgeorge.org/images/ofxtimeline/github/example-simpleTween.png)

### Audio Waveform
Audio Waveformでは、オーディオトラックの使い方を紹介しています。
![AudioWaveform](http://www.jamesgeorge.org/images/ofxtimeline/github/example-audioWaveform.png)

### Video Renderer
Video Rendererでは、タイムラインとシェーダーを使用した、シンプルなビデオエフェクトアプリケーションの作り方を紹介しています。映像ファイルをアプリケーションにドラッグアンドドロップし、加工、編集されたフレームをイメージのシーケンスとして
書き出してみてください。
![VideoRenderer](http://www.jamesgeorge.org/images/ofxtimeline/github/example-videoRenderer.png)

### Multi-timeline
Multi-timelineでは、フラグを使用して同じアプリケーション内の他のタイムラインの再生を制御する方法を紹介しています。
![MultiTimeline](http://www.jamesgeorge.org/images/ofxtimeline/github/example-multiTimeline.png)

## ホットキー

ofxTimelineではいくつかのホットキーが設定されています。

### OS Xでホットキーを有効にするには
OS Xでは、GLUTがCOMMAND+C、COMMAND+V、そしてCOMMAND+Sの操作を取得してしまいます。ofxTimelineには、このデフォルトの関連付けを解除し、ofxTimeline上で使えるようにする機能を備えています。下記のコードをsetupの中に加えてください。

      ofxTimeline::removeCocoaMenusFromGlut("myAppName");

### 全ホットキー

ノート：OS Xではコマンドキーを使用します。LinuxとWindowsではCTRLキーを使用します。

<table>
    <tr>
        <td style="font-weight:bold">機能</td><td style="font-weight:bold">ショートカット</td>
    </tr>
    <tr>
        <td>選択範囲をカット</td><td>command+x</td>
    </tr>
    <tr>
        <td>選択範囲をコピー</td><td>command+c</td>
    </tr>
    <tr>
        <td>選択範囲にペースト</td><td>command+v</td>
    </tr>
    <tr>
        <td>元に戻る</td><td>command+z</td>
    </tr>
    <tr>
        <td>やり直す</td><td>shift+command+z</td>
    </tr>
    <tr>
        <td>フォーカスされたトラック上の全てのキーフレームを選択</td><td>command+a</td>
    </tr>
    <tr>
        <td>フォーカスされたトラック上の選択範囲に全てのキーフレームを加える</td><td>command+shift+a</td>
    </tr>
    <tr>
        <td>選択された全てのキーフレームを削除</td><td>delete or backspace</td>
    </tr>    
    <tr>
        <td>選択されたキーフレームを少し移動</td><td>arrow keys</td>
    </tr>    
    <tr>
        <td>選択されたキーフレームをもう少し移動</td><td>shift+arrow keys</td>
    </tr>    
    <tr>
        <td>フォーカスされたトラックを開く</td><td>alt+e</td>
    </tr>    
    <tr>
        <td>全てのトラックを畳む</td><td>alt+c</td>
    </tr>    
    <tr>
        <td>トラックのサイズを均等にする</td><td>alt+shift+c</td>
    </tr>    
</table>




## ofxTimelineを自分のopenFrameworksプロジェクトに組み込む

### ofxTimelineと関連するファイルのクローンを行う
timelineをダウンロードするためには、まずターミナルを開き、下記の様に入力します。

    $cd of_0073_osx_release/addons
    $git clone https://github.com/YCAMInterlab/ofxTimeline.git
    $cd ofxTimeline/
    $./clone_addons.sh

これで、必要なアドオンをダウンロードする事が出来ます。もしもいくつかのアドオンが既にインストール済みであった場合は、そのアドオンが上書きされる事はありません。

### ofxTimelineを含むプロジェクトを作成する
of_0073_osx_release/projectGenerator/に入っているProjectGeneratorを開きます。

プロジェクトに名前をつけ、addonsタブから、下記のアドオンを選択します。
- ofxTimeline
- ofxTimecode
- ofxMSATimer
- ofxTextInputField
- ofxRange
- ofxTween

上記を選択し追えたら、BACKで一つ前の画面に戻り、Generate projectをクリックします。

Windows上のVC2010を使われている場合は、Properties -> Configuration Properties -> Build Events -> Post-Build events -> Command Lineの順に選択し、下記のbuild eventをコピー＆ペーストしてください。

xcopy /e /i /y "$(ProjectDir)..\..\..\export\vs2010\*.dll" "$(ProjectDir)bin" & xcopy /e /i /y "$(ProjectDir)..\..\..\addons\ofxTimeline\libs\sndfile\redist\*.dll" "$(ProjectDir)bin"

MacOSXでAudioTrackを使用する場合は、OpenAL.frameworkをプロジェクトに加える必要があります。

### タイムラインをコードに加える

testApp.hファイルで下記を加えます：

    #include "ofMain.h"
    #include "ofxTimeline.h" //add timeline include

    class testApp : public ofBaseApp{
    public:
      ofxTimeline timeline; //inside of the class add a timeline

testApp.cppのsetupの中で、タイムラインのセットアップを行います：

    //--------------------------------------------------------------
    void testApp::setup(){
      
      timeline.setup(); //registers events
      timeline.setDurationInSeconds(60); //sets time
      timeline.setLoopType(OF_LOOP_NORMAL); //turns the timeline to loop
      
      //add a track
      timeline.addCurves("MyCircleRadius", ofRange(0, 200));
  
drawもしくはupdateの中で値を読み出します。
  
    //--------------------------------------------------------------
    void testApp::draw(){
      //the value of changingRadius will be different depending on the timeline
      float changingRadius = timeline.getValue("MyCircleRadius"),
      //use the value for something amazing!
      ofCircle(mouseX, mouseY, changingRadius);
      //don't forget to draw your timeline so you can edit it.
      timeline.draw();
    }
    

## トラックのタイプ
ofxTimelineには、一般的なタイムラインで必要になる数種類のタイプのトラックが標準で組み込まれています。


### Bangs
![Bangs](http://www.jamesgeorge.org/images/ofxtimeline/github/BangTrack.png)

bangはシンプルな時間のマーカーで、再生ヘッドがマーカーを通過する時にイベントを送る事ができます。bangトラックを使用するには、下記の様にしてbangイベントを取得するクラスを使う必要があります。

MyClass.h

      ofxTimeline timeline;
      void receivedBang(ofxTLBangEventArgs& bang);
      
MyClass.cpp

    //--------------------------------------------------------------
    void MyClass::setup(){
      timeline.addBangs("trigger particles");
      ofAddListener(timeline.events().bangFired, this, &testApp::receivedBang);
    }
    
    //--------------------------------------------------------------
    void MyClass::receivedBang(ofxTLBangEventArgs& bang){
      ofLogNotice("Bang fired from track " + bang->track.getName());
      if(bang.track->getName() == "trigger particles"){
        particleGenerator.start(); //some example response
      }
    }
    
.

    継承関係：ofxTLTrack -> ofxTLKeyframes -> ofxTLBangs

### Curves ###
![Curves](http://www.jamesgeorge.org/images/ofxtimeline/github/CurvesTrack.png)

Curvesを使うと、補完機能の付いたキーフレームを使い、タイムラインに沿って値を最小値と最大値の間でスムーズに変化させる事が出来ます。

タイムライン上のキーフレームを右クリックすると選択ウィンドウが表示され、補完の方法を変更する事が出来ます。補完の方法は、[ofxCurves](https://github.com/arturoc/ofxTween)の、Penner方程式をベースにしています。

    継承関係: ofxTLTrack -> ofxTLKeyframes -> ofxTLCurves

### Flags ###
![Flags](http://www.jamesgeorge.org/images/ofxtimeline/github/FlagTrack.png)

Flagsはbangsに似ていますが、各フラグ（マーカー）に、テキストを入力する事が出来ます。値はイベントが発生するごとに読み出す事が可能です。

    //--------------------------------------------------------------
    void MyClass::receivedBang(ofxTLBangEventArgs& bang){      
      ofLogNotice("Bang fired from track " + bang.flag);
    }   

.

    継承関係：ofxTLTrack -> ofxTLKeyframes -> ofxTLBangs -> ofxTLFlags

### Switches ###
![Switches](http://www.jamesgeorge.org/images/ofxtimeline/github/SwitchTrack.png)

Switchesを使う事で、タイムライン上で、ON/OFFのシンプルなコントロールをすることが出来ます。

    継承関係：ofxTLTrack -> ofxTLBangs -> ofxTLSwitches

### VideoTrack ###
![VideoTrack](http://www.jamesgeorge.org/images/ofxtimeline/github/VideoTrack.png)

VideoTracksでは、映像とそのエフェクトを同時にスクラブ再生する事が出来ます。videoトラックが追加されると、映像再生が全体のタイムラインの再生を制御する形になります。

    ofxTLVideoTrack* videoTrack = timeline.addVideoTrack("Video", videoPath);
    if(videoTrack != NULL){ //in the case the video load failed check against null
        timeline.setFrameRate(videoTrack->getPlayer()->getTotalNumFrames()/videoTrack->getPlayer()->getDuration());
        timeline.setDurationInFrames(videoTrack->getPlayer()->getTotalNumFrames());
        timeline.setTimecontrolTrack(videoTrack); //video playback will control the time        
    }

.

    継承関係：ofxTLTrack -> ofxTLImageTrack -> ofxTLVideoTrack


### AudioTrack ###
![AudioTrack](http://www.jamesgeorge.org/images/ofxtimeline/github/AudioTrack.png)

AudioTracksを使用すると、オーディオトラックとその他のシーケンスを同時にスクラブ再生する事が出来ます。

.cppファイルの中でトラックを追加し、ファイルを読み込みます。

    //--------------------------------------------------------------
    void testApp::keyPressed(int key){
        //... setup stuff
        timeline.addAudioTrack("Audio", "myAudioFile.wav");
    }

    //--------------------------------------------------------------
    void testApp::update(){
        //check the FFT data
        if(key == ' '){
            //calling play on the waveform controls the timeline playback
            ofxTLAudioTrack* track = timeline.getAudioTrack("Audio");
            for(int i = 0; i < track->getFFTSize(); i++){
                  track->getFFT()[i]; //FFT data
            }
        }
    }

.

    継承関係：ofxTLTrack -> ofxTLImageTrack -> ofxTLAudioTrack


### ColorTrack
![ColorTrack](http://www.jamesgeorge.org/images/ofxtimeline/github/ColorTrack.png)

colorトラックを使えば、シーケンスに沿ってスムーズに色を変化させる事が出来ます。色のサンプルパレットとして画像を使用し、各キーフレームではその画像パレットの中から、色をサンプルする位置を指定します。キーフレーム間では画像パレットの中で色をサンプルする位置が移動していくことで、滑らかに色が変化します。

### LFO (experimental)
![LFO](http://www.jamesgeorge.org/images/ofxtimeline/github/LFOTrack.png)

LFOは、Low Frequency Oscillatorの略で、シンプルな繰り返しのパターンや、ノイズ関数を使用する事が出来ます。


## サードパーティーのカスタムトラック##

ofxTimelineは他のアドオンやタイムベースのメディアを組み込んで使用する事ができます。下記はいくつかの例です。

### 深度センサーからのイメージシーケンス ###

https://github.com/obviousjim/ofxRGBDepth // [RGBDToolkit](http://wwww.rgbdtoolkit.com)

### Midiトラック ###

https://github.com/momo-the-monster/ofxTimelineTracks // [ofxTLMidiNote](https://vimeo.com/46793363)

## カスタムトラックをデザインする ##

トラックは拡張しやすいようにデザインされています。ofxTimelineには下記の2つのトラックテンプレートがあり、オリジナルのトラックを作るには、ここから始めるのが良いでしょう。

### ofxTLEmptyTrack

タイムベースのメディアをビジュアライズしたり、オリジナルのインタラクティブトラックを作る場合は、このテンプレートを使用してください。

### ofxTLEmptyKeyframes
Use this template for creating a custom keyframe based track. It already has copy+paste, multiple selection, saving and loading.
キーフレームベースのトラックを作る場合はこのテンプレートを使用してください。このテンプレートには、コピー＆ペースト、複数選択、セーブとロードの機能が組み込まれています。


## Supported By ##

ofxTimeline is codeveloped by YCAM InterLab during the [Guest Research Projector v.2 at YCAM Interlab](http://interlab.ycam.jp/en/projects/guestresearch/vol2) by [James George](http://www.jamesgeorge.org) at the [YCAMInterLab](http://interlab.ycam.jp/)

![YCAM](http://www.jamesgeorge.org/images/ofxtimeline/github/logo_ycam.png)

### Additional support  ###

![Additional Support](http://www.jamesgeorge.org/images/ofxtimeline/github/SupportedBy.png)

[Playmodes](http://www.playmodes.com/) who graciously funded and shaped the project in it's early stages for [BlueBeams](https://vimeo.com/35931265) .

[The Frank-Ratchye STUDIO for Creative Inquiry](http://studioforcreativeinquiry.org/) who have supported the [RGBDToolkit](http://www.rgbdtoolkit.com) project.

[Eyebeam](http://http://eyebeam.org/) Who's community has prompted many of the thoughts provoking the development of ofxTimeline during my residency in Spring 2012.

Also a huge thank you to [Creative Applications](creativeapplications.net) for [publishing the initial experiment](http://www.creativeapplications.net/openframeworks/ofxtimeline-openframeworks/) which brought a lot of interested people together to get this to where it is now.
