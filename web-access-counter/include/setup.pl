#!/usr/bin/perl

# サーバ環境に合わせた設定（グローバル変数の定義）

use strict;
use warnings;
use utf8;

# 同じアドレスからのアクセスをカウントしない (1:ON, 0:OFF)
our $flag_ip_sameaddr_check = 1;

# プログラムに引数がなかった場合の標準動作モード
# SSIのテキストモードで使用する場合は、$flag_mode_param = "text";
our $flag_mode_param = "text";

# 'text'モードの時の、表示桁数 （0:不要なゼロをつけない）
our $sw_cnt_colmn = 6;


# ログに記録するときに、削除（置換）する文字列
# [0]要素に置換前、[1]要素に置換後の文字列を指定する。上から順に検索し、一致した時点で終了する
our @arrHideReferer = (
		['http://www.example.com/mw/index.php\?title=', 'mw:'],
		['http://www.example.com', ''],
		['http://example.com' => '']
		);

# 「Apache Webサーバのログからエラー行を抽出してHTML化するツール」のDBファイルの指定
# parse-apache-log.sqlite を stat_monthly.plで利用するための設定
# この文字列を ''（空白）にすると、Apache Logを使わないモードとなる。
#our $strApachelogSqlDsn = 'DBI:SQLite:dbname=/home/user/data/parse-apache-log.sqlite';
our $strApachelogSqlDsn = '';

1
