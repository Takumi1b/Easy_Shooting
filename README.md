# Easy_Shooting

提供されたライブラリを使用した簡単なシューティングゲーム


# DEMO

https://user-images.githubusercontent.com/103201314/173221234-01cb416f-e57c-438e-943b-c8349e381c7f.mp4


# Requirement

* intel CPU 搭載 Mac
* c言語のコンパイル環境
* Handy Graphic (version 0.8.3)

# Installation

リポジトリからダウンロード
```bash
git clone https://github.com/Takumi1b/Easy_Shooting.git
```

Handy Graphic (version 0.8.3)は以下よりダウンロード

[Handy Graphic](http://www.cc.kyoto-su.ac.jp/~ogihara/Hg/)

# Usage
コンパイル
```bash
hgcc -o main easy_shooting.c
```
実行
```bash
./main
```
操作方法

左右移動：a/d または　←/→

弾の発射：l または　↑を押してゲージを動かし、離して発射

リロード：k または　↓

ポーズ　：p

# Note

Handy Graphicの仕様上、Windows, Linux環境下では動作しません

M1 Macでは動作が不安定になります

# Author

* TanabeTakumi
* 京都産業大学大学院　先端情報学研究科
* i2286095@cc.kyoto-su.ac.jp
