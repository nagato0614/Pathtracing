# pathtracing

スペクトルを考慮したパストレーシング

## できていること

 - レンダリングメソッド
   - パストレーシング
 - マテリアル
   - スペクトルを考慮した拡散面 
   - 鏡面
   - 透過
 - BVH
   - 各軸ごとにソートし中央値で分割するだけの簡単なもの
 - directLighting(NextEventEstimation)
   - MISは未実装.光源の一点をサンプリング接続を行うのみ
    
## できてないこと
 - シーンファイルの読み込み
 - Cameraクラスの実装
   - Thin Lens Camera の実装
 - BVHの最適化
   - よりよいアルゴリズムの実装
 - IBLの実装
   - 画像の読み込みライブラリの実装または追加
   - RGBからスペクトラルに変換
 - スペクトルを考慮した屈折
    - デバックのため現在未使用

## 将来的にやりたいこと
 - フォトンマッピングの系の実装
    - プログレッシブフォトンマッピングの実装
 - BSDFの追加
   
## 必要なもの
- libpng
- boost-python
- zlib
- cmake


cmakeでのビルドを想定しています