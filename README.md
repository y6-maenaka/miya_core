# miya_core

miya_coreは、Bitcoin-likeなUTXOモデルを採用したオリジナルブロックチェーンクライアント実装です。C++20で記述され、独自のP2Pネットワーク層、データベースエンジン、コンセンサスメカニズムを備えています。

## 特徴

- **UTXOモデル**: Bitcoin型の未使用トランザクション出力（UTXO）管理
- **Proof-of-Work**: SHA-256ダブルハッシュによるコンセンサスアルゴリズム
- **P2PKHトランザクション**: Pay-to-Public-Key-Hash形式のトランザクション対応
- **NAT対応P2P**: STUN/ICEを使用したNATトラバーサル機能付きP2Pネットワーク
- **独自データベース**: MiyaDB（MMyISAM風ストレージエンジン、B-treeインデックス）
- **スクリプトシステム**: トランザクション検証用のOP_CODE実装
- **効率的なメモリプール**: Boost Multi-Indexコンテナによる多軸管理

## 技術仕様

- **言語**: C++20
- **ビルドシステム**: CMake
- **主要な依存関係**:
  - OpenSSL (ECDSA暗号化、SHA-256ハッシュ)
  - Boost (ASIO, program_options, multi_index)
- **コード規模**: 約410ファイル、チェーンモジュールだけで約10,500行

## アーキテクチャ

### ディレクトリ構成

```
miya_core/
├── .config/                    # 設定ファイル
│   ├── bootnodes.json         # ブートストラップノードアドレス
│   ├── stun_servers.json      # NATトラバーサル用STUNサーバー
│   ├── wallet.json            # ウォレット設定（ECDSA鍵パスワード）
│   └── settings.json          # 一般設定
├── chain/                      # ブロックチェーンコア実装 (~10,500行)
│   ├── block/                 # ブロック・ヘッダー構造
│   ├── transaction/           # トランザクション型（P2PKH、coinbase、スクリプト）
│   ├── transaction_pool/      # メモリプール実装
│   ├── utxo_set/             # UTXO管理
│   ├── chain_sync_manager/    # チェーン同期ロジック
│   ├── block_chain_manager/   # ブロックチェーン状態管理
│   ├── validation/            # ブロック・トランザクション検証
│   ├── mining/                # マイニングアルゴリズム
│   └── miya_coin/            # ローカルストレージ (blkXXXXX.dat, revXXXXX.dat)
├── ss_p2p_node_controller/    # カスタムP2Pネットワーク層
│   ├── include/ss_p2p/       # P2P API ヘッダー
│   └── src/ss_p2p/           # P2P実装（UDP、Kademlia DHT、ICE）
├── miya_db/                   # カスタムデータベースエンジン
│   ├── miya_db/              # クエリパーサー、コネクション管理
│   └── strage_manager/       # MMyISAMストレージエンジン、B-treeインデックス
├── node_gateway/              # メッセージプロトコル層
│   └── message/command/      # プロトコルコマンド（INV, GETBLOCKS, HEADERS等）
├── core/                      # メインアプリケーションコア
├── control_interface/         # トランザクション作成インターフェース
├── mining_manager/            # マイニング調整
├── share/                     # 共有ユーティリティ
│   ├── cipher/               # ECDSA暗号化（OpenSSLラッパー）
│   ├── hash/                 # SHAハッシュユーティリティ
│   ├── miya_db_client/       # データベースクライアントインターフェース
│   └── observer/             # オブザーバーパターン実装
└── test/                      # テストスイート
```

### コアコンポーネント

#### 1. ブロックチェーン実装 ([chain/](chain/))

**ブロック構造:**
- ヘッダー: version, prev_block_hash, merkle_root, timestamp, nBits (難易度), nonce
- トランザクションリスト（coinbase + 通常トランザクション）
- SHA-256ダブルハッシュによるPoW検証

**トランザクション型:**
- P2PKH (Pay-to-Public-Key-Hash): 標準的な支払いトランザクション
- Coinbase: マイニング報酬トランザクション
- スクリプトシステム: OP_DUP, OP_HASH_160, OP_EQUALVERIFY, OP_CHECKSIG等のOP_CODE

**チェーン同期:**
- IBD (Initial Block Download) サポート
- 並列/直列同期マネージャー
- フォーク検出と解決
- フルブロックダウンロード前のヘッダー検証

**ストレージ:**
- Bitcoin風のファイル構造: blkXXXXX.dat (ブロック), revXXXXX.dat (undo データ)
- メモリマップドファイルI/Oによる高性能化
- カスタムDBによるブロックインデックス
- UTXOセットの永続化

#### 2. トランザクションプール ([chain/transaction_pool/](chain/transaction_pool/))

Boost Multi-Indexコンテナを使用した複数インデックス管理:
- トランザクションID別（hashed_unique）
- 手数料別（ordered）
- エントリ時刻別（ordered）
- バイトサイズ別（ordered）

設定:
- 最大プールサイズ: 300MB
- 最大トランザクションサイズ: 100KB
- トランザクション有効期限: 2週間

#### 3. P2Pネットワーク ([ss_p2p_node_controller/](ss_p2p_node_controller/))

**機能:**
- UDPベースのピアツーピア通信
- ICE (Interactive Connectivity Establishment) によるNATトラバーサル
- STUNサーバーサポート
- Kademlia DHTによるピア発見とルーティング
- メッセージプール＆ハブによる受信メッセージ管理
- マルチキャストマネージャーによるブロードキャスト

**API例:**
```cpp
ss::node_controller n_controller(self_endpoint, io_context);
n_controller.start(boot_nodes);
auto &message_hub = n_controller.get_message_hub();
```

#### 4. メッセージプロトコル ([node_gateway/](node_gateway/))

Bitcoin風のメッセージプロトコル:
- **INV**: インベントリベクター（ブロック/TX通知）
- **GETBLOCKS**: ブロックハッシュリクエスト
- **GETDATA**: フルブロック/トランザクションリクエスト
- **GETHEADERS**: ブロックヘッダーリクエスト
- **HEADERS**: ブロックヘッダー応答
- **BLOCK**: フルブロックデータ
- **MEMPOOL**: メモリプール内容リクエスト
- **NOTFOUND**: アイテムが見つからない通知

メッセージ構造: 4バイトトークン + 12バイトコマンド + 4バイト長 + ペイロード

#### 5. UTXOセット ([chain/utxo_set/](chain/utxo_set/))

- MiyaDBによるバックエンド
- 仮想モードサポート（トランザクション的セマンティクス）
- Add/Get/Remove操作
- トランザクション検証との統合

#### 6. カスタムデータベース ([miya_db/](miya_db/))

**MMyISAMストレージエンジン:**
- B-treeインデックス
- LRUキャッシュ付きページテーブル
- 効率的なメモリマッピング用オーバーレイメモリマネージャー
- セーフモード（コミット/アボートサポート）

**クエリシステム:**
- カスタムクエリパーサー
- クエリコンテキスト管理
- ソケットベースのコネクションマネージャー
- BSON風バイナリフォーマット

#### 7. 暗号化 ([share/cipher/](share/cipher/), [share/hash/](share/hash/))

**ECDSA (楕円曲線デジタル署名アルゴリズム):**
- OpenSSLベース実装
- 鍵生成と保存（パスワード付きPEM形式）
- トランザクション署名/検証
- アドレス生成用公開鍵ハッシュ

**ハッシュ:**
- SHA-256, SHA-512, SHA-1サポート
- 用途: ブロックハッシュ、トランザクションID、アドレス生成、マークルルート

#### 8. マイニング ([chain/mining/](chain/mining/), [mining_manager/](mining_manager/))

**シンプルマイニング実装:**
- 設定可能な難易度（nBits）でのProof-of-Work
- 有効なハッシュが見つかるまでnonce反復
- マルチスレッドマイニングサポート
- メモリプールから候補ブロックを組み立てるブロックアセンブラー

**マイニングプロセス:**
1. ブロックアセンブラーがメモリプールからトランザクション選択
2. マイニング報酬付きcoinbaseトランザクション作成
3. マークルルート計算
4. ブロックハッシュが難易度ターゲットを満たすまでnonce反復
5. 有効なブロックをブロードキャスト

#### 9. 検証 ([chain/validation/](chain/validation/))

**ブロック検証:**
- PoW検証（ハッシュ < ターゲット）
- マークルルート検証
- タイムスタンプ検証

**トランザクション検証:**
- スクリプト実行と検証
- UTXO存在チェック
- 二重支払い防止
- 署名検証

## ビルド方法

### 前提条件

- CMake 3.10以上
- C++20対応コンパイラ（GCC 10+, Clang 10+等）
- OpenSSL開発ライブラリ
- Boost開発ライブラリ（ASIO, program_options, multi_index）

### ビルド手順

```bash
# リポジトリをクローン
git clone <repository-url>
cd miya_core

# ビルドディレクトリを作成
mkdir build
cd build

# CMake設定（デバッグビルド）
cmake -D_BUILD_CORE_DEBUG=True ..

# ビルド実行
cmake --build .

# 実行
./core_ver_0
```

### Dockerサポート

AlmaLinuxベースのDockerコンテナでもビルド可能です。

```bash
docker build -t miya_core .
docker run -it miya_core
```

## 設定

### 設定ファイル

#### [.config/wallet.json](.config/wallet.json)
ECDSA秘密鍵のPEMパスフレーズを設定します。

```json
{
  "ecdsa_pem_passphrase": "your-secure-password"
}
```

#### [.config/bootnodes.json](.config/bootnodes.json)
初期P2P接続用のブートストラップノードを指定します。

```json
[
  {
    "address": "192.168.1.100",
    "port": 8333
  }
]
```

#### [.config/stun_servers.json](.config/stun_servers.json)
NATトラバーサル用のSTUNサーバーを設定します。

```json
[
  {
    "address": "13.231.16.25",
    "port": 8080
  }
]
```

#### [.config/settings.json](.config/settings.json)
一般的な設定オプション。

```json
{
  "extensions": {
    "verifySignature": false
  }
}
```

## 使用方法

### ノードの起動

```bash
cd build
./core_ver_0
```

起動すると以下の処理が実行されます:
1. ウォレットからECDSA鍵を読み込み
2. P2Pノードコントローラー初期化
3. メッセージハブ起動
4. チェーンマネージャー初期化（ストレージとUTXOセット）
5. 必要に応じてIBD（Initial Block Download）開始
6. メインイベントループ実行

### トランザクションの作成

[control_interface/](control_interface/)を使用してトランザクションを作成します。`tx_origin/`ディレクトリにJSONファイルを配置:

```json
{
  "destination": [
    {
      "address": "CA66412A08F2A9B706D383CA3599A49BFC59F86B",
      "value": 23980
    }
  ]
}
```

## 開発状況

### テスト

テストスイートは[test/](test/)ディレクトリに配置:
- `miya_chain/`: ブロックチェーン機能テスト
- `miya_db/`: データベーステスト
- 各コンポーネントの単体テスト

### 既知の制限事項

**実装上の制限:**
- P2PKHトランザクションのみサポート（P2SH, SegWit等は未対応）
- マイニング難易度調整未実装
- シンプルなマイニングアルゴリズムのみ
- トランザクション手数料市場未実装

**既知の問題:**
- OpenSSL SHA/ECDSA関数でのメモリリークの可能性
- トランザクションプールの過剰なメモリ消費
- UTXOセット応答処理の潜在的な競合状態
- IBD中の二重支払い時系列整合性チェック欠如

## 最近の開発動向

最新のコミット:
- chain_sync_manager同期開始時処理の実装
- メッセージのシリアライゼーション修正
- base_id, union_id, block_id, transaction_idのリファクタリング
- chain_sync_managerのエラーハンドリング改善
- オブザーバーストレージポリシーの改善

