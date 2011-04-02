#!/usr/bin/perl

# save this file in << UTF-8  >> encode !
# ******************************************************
# Software name : Webページ用 カウンタCGI （ダミーGIF画像利用）
#
# Copyright (C) INOUE Hirokazu, All Rights Reserved
#     http://oasis.halfmoon.jp/
#
# client.cgi
#
# version 3.0, 2011/03/24  utf8化
#
# GNU GPL Free Software
#
# このプログラムはフリーソフトウェアです。あなたはこれを、フリーソフトウェア財
# 団によって発行された GNU 一般公衆利用許諾契約書(バージョン2か、希望によっては
# それ以降のバージョンのうちどれか)の定める条件の下で再頒布または改変することが
# できます。
#
# このプログラムは有用であることを願って頒布されますが、*全くの無保証* です。
# 商業可能性の保証や特定の目的への適合性は、言外に示されたものも含め全く存在し
# ません。詳しくはGNU 一般公衆利用許諾契約書をご覧ください。
#
# あなたはこのプログラムと共に、GNU 一般公衆利用許諾契約書の複製物を一部受け取
# ったはずです。もし受け取っていなければ、フリーソフトウェア財団まで請求してく
# ださい(宛先は the Free Software Foundation, Inc., 59 Temple Place, Suite 330
# , Boston, MA 02111-1307 USA)。
#
# http://www.opensource.jp/gpl/gpl.ja.html
# ******************************************************
# *************
# 使用法
#   client.cgi?test		状況レポート
#   client.cgi?text		カウンタをテキストで表示
#   client.cgi?gif		$str_filepath_dummygifで指定されたGIF画像を表示（1x1のダミー画像を想定）
#   client.cgi			$flag_mode_paramで指定された textまたはgifの動作を行う
# *************

use strict;
use warnings;
use utf8;
use CGI;
use DBI qw(:sql_types);
use Time::HiRes;	# ミリ秒単位で、処理時間を求める
use Encode::Guess qw/euc-jp shiftjis iso-2022-jp/;	  # 必要ないエンコードは削除すること
use FindBin qw/$Bin/;	# サーバ上でのフルパス名を得るため

# 文字コード変換 共通サブルーチン用定義
my $flag_os = 'linux';  # linux/windows
my $flag_charcode = 'utf8';             # utf8/shiftjis
# IOの文字コードを規定
if($flag_charcode eq 'utf8'){
	binmode(STDIN, ":utf8");
	binmode(STDOUT, ":utf8");
	binmode(STDERR, ":utf8");
}
if($flag_charcode eq 'shiftjis'){
	binmode(STDIN, "encoding(sjis)");
	binmode(STDOUT, "encoding(sjis)");
	binmode(STDERR, "encoding(sjis)");
}

# 正常時もエラーログを記録する
my $flag_debugcsv_force_on = 0;

# SQLサーバのDSN定義
my $str_dsn_accesslog = 'DBI:SQLite:dbname='.$FindBin::Bin.'/data/accdb.sqlite';
my $str_dsn_counter = 'DBI:SQLite:dbname='.$FindBin::Bin.'/data/counter.sqlite';

# デバッグ用エラーログファイル
my $str_filepath_debugcsv = $FindBin::Bin.'/debuglog.csv';

# ダミーGIF画像ファイル
my $str_filepath_dummygif = 'dummy_img.gif';

# *************
# 動作モードの設定

# SSIのテキストモードで使用する場合は、$flag_mode_param = "text";
my $flag_mode_param = "text";

# 表示桁数 （0:不要なゼロをつけない）
my $sw_cnt_colmn = 6;

# ロックに使う作業用変数
my $flag_is_locked = 0;			# ロックされている場合 =1
my $tm_lock = 0;			# ロック時刻 （UNIX秒）
my $tm_lock_timeout = 15;		# ロックのタイムアウト秒数

# 同じアドレスからのアクセスをカウントしない (1:ON, 0:OFF)
my $flag_ip_sameaddr_check = 1;
# 作業用変数（同一アドレスからのアクセスの場合1になる）
my $flag_is_ip_sameaddr = 0;

# IPアドレスのホスト名への変換を行う (1:ON, 0:OFF)
my $flag_ip_gethost = 1;

# ログに記録するときに、削除（置換）する文字列
# [0]要素に置換前、[1]要素に置換後の文字列を指定する。上から順に検索し、一致した時点で終了する
my @arrHideReferer = (
		['http://www.example.com/mw/index.php\?title=', 'mw:'],
		['http://www.example.com', ''],
		['http://example.com' => '']
		);

# 環境変数TZを日本時間に設定する
$ENV{'TZ'} = "JST-9";

# 処理時間計測（処理開始時間）
my $tmmStart = Time::HiRes::time();

# 環境変数から解析する変数
my $ip = "";			# アクセス元のIPアドレス
my $hostname = "";		# アクセス元のホスト名
my $browser = "";		# ブラウザ変数
my $referer = "";		# 参照元URL
my $lang = "";			# 言語 "us ja ko ..." の集計形式
my $lang_raw = "";		# 言語（生データ）

# カウンタ
my $nCounter = 0;

# *************
# 初期値の決定（これ以後は、操作禁止）

# 異常終了の場合のトラップ
sub sigexit {  sub_write_to_debug_log("===SIGEXIT==="); exit(0); }
$SIG{'PIPE'} = $SIG{'INT'} = $SIG{'HUP'} = $SIG{'QUIT'} = $SIG{'TERM'} = "sigexit";


# アクセスログ用のデータを得る（IPアドレス、リファラー、ブラウザ文字列）
($ip,$hostname,$browser,$referer,$lang) = sub_make_accesslod_data();


# *************
# プログラムの引数により処理を切り替える
if (!defined($ARGV[0]) || $ARGV[0] eq ''){
	# 何も指定しない場合は、初期値の動作を行う
} elsif ($ARGV[0] eq "text") {
	$flag_mode_param = "text";		# SSIモード（テキストでカウンタ値を返す）
} elsif ($ARGV[0] eq "gif") {
	$flag_mode_param = "gif";		# ブランクのGIF画像を返す（ダミー画像）
} elsif ($ARGV[0] eq 'test') {
	$flag_mode_param = "test";		# カウンタのステータス表示（テキスト）
} else {
	# その他のオプションを指定した場合、エラー終了
	sub_write_to_debug_log("===ARGV_ERROR===");
	exit;
}


# カウンタデータベースを読み込む（ロック設定、IP重複判定）
if($flag_mode_param eq "test") {
	($flag_is_locked, $nCounter, $flag_is_ip_sameaddr, $tm_lock) = sub_read_counter_db($ip, 0);
} else {
	($flag_is_locked, $nCounter, $flag_is_ip_sameaddr, $tm_lock) = sub_read_counter_db($ip, 1);
}

# *************
# CGIが使用できるかテストを行う。
if ($flag_mode_param eq "test") {
	sub_test_cgi($ip,$hostname,$browser,$referer,$lang, $tm_lock, $flag_is_locked, $flag_is_ip_sameaddr, $nCounter);
	exit;
}


# *************
# ロックが掛かっていない場合、アクセスログ記録を行う
if($flag_is_locked == 0)
{
	# 次の2つの条件のどちらかの時にログを記録する
	# 1. $flag_ip_sameaddr_check==0のとき
	# 2. $flag_ip_sameaddr_check==1かつ$nSameIp==0のとき
	if($flag_is_ip_sameaddr == 0 || $flag_ip_sameaddr_check == 0)
	{
		# アクセスログDBに追記
		sub_write_accesslog_db($ip,$hostname,$browser,$referer,$lang);
		
		$nCounter++;	# カウンターを1増やす
		
		sub_write_counter_db($ip, $nCounter);		# カウンタ書き込み、ロック解除
	}
}
else
{	# ロックが掛かっていた場合はデバッグログに書き込む
	sub_write_to_debug_log("===LOCK===");
}


# ミリ秒単位での処理開始からの経過時間
my $tmmLapse = Time::HiRes::time() - $tmmStart;

# 処理時間が1秒以上かかった場合は、デバッグログに書き込む
if($tmmLapse > 1) {
	sub_write_to_debug_log("===LONGTIME===");
}
elsif($flag_debugcsv_force_on == 1){
	# デバッグ用に、全てのアクセスでデバッグログに書き込む
	sub_write_to_debug_log("===DEBUG===");
}


# GIFまたはテキストでカウンターを描画する
sub_display_counter($nCounter, $flag_mode_param, $sw_cnt_colmn);


exit;
# CGI カウンター 処理終了
# *************


# *************
# HTML でメッセージを出力する
# HTML_Output($str)
sub HTML_Output {
	my $str = shift;	# 引数
	my $q = new CGI;

	print $q->header(-type=>'text/html', -charset=>'shift_jis');
	print $q->start_html(-title=>"Access log query",-lang=>'ja-JP');
	print($str);
	print $q->end_html;
}


# *************
# 画面表示（テキストカウンタ or GIFダミー画像）
# sub_display_counter($n_counter, $flag_mode, $n_counter_digit)
sub sub_display_counter{
	# 引数
	my $n_counter = shift;
	my $flag_mode = shift;
	my $n_counter_digit = shift;
	# 引数チェック
	if(!defined($n_counter) || $n_counter eq '' || $n_counter<1){ $n_counter = 0; }
	if(!defined($flag_mode) || $flag_mode eq ''){ $flag_mode = 'null'; }
	if(!defined($n_counter_digit) || $n_counter_digit eq '' || $n_counter_digit<1){ $n_counter_digit = 4; }

	if ($flag_mode eq "text") {
		# テキストでカウンターを描画
		# 表示桁数に合わせて、現在のカウンタ値を文字列にする
		my $str = sprintf(sprintf("%%0%dld", $n_counter_digit), $n_counter);
		print("Content-type: text/html\n\n");
		print($str."\n");
		return;
	} elsif ($flag_mode eq "gif") {
		# GIF のダミー画像を表示する
		unless( -f $str_filepath_dummygif ){ return; }
		print("Content-type: image/gif\n\n");
		my $n_filesize = -s $str_filepath_dummygif;
		open(IN, "<$str_filepath_dummygif");
		binmode(IN);
		binmode(STDOUT);
		my $buffer = undef;
		read(IN, $buffer, $n_filesize);
		close(IN);
		print $buffer;
		return;
	}
}


# *************
# SQL格納に有害になる文字を除去し、文字コードをSJISに変更する
# $str_clean = sub_conv_to_safe_str($str, $n_max_length);
sub sub_conv_to_safe_str
{
	# 引数
	my $str = shift;
	my $n_max_length = shift;
	# 引数チェック
	if(!defined($str) || length($str)<=0){ $str = ''; }
	if(!defined($n_max_length) || $n_max_length<=0){ $n_max_length = 255; }

	chomp($str);	# 末尾改行を削除
	if($str eq ''){ return ''; }

	# 文字エンコードをshiftjisに変換
	$str = sub_conv_to_flagged_utf8($str);	# 任意encode → utf8
	$str = Encode::encode('shiftjis', $str);	# utf-8 → sjis

	# バイナリや特殊記号を削除（SQLインジェクション防止）
	# （なお、0x5cのバックスラッシュはSJISの文字コードを侵食するため削除しない
	$str =~ tr/(\x00-\x1f|\x21-\x27)//;
	$str =~ s/(\x2c|\x3b|<|>|\x3f|\x60)//g;

	# 文字列を最大長さで切り捨てる
	$str = substr($str, 0, $n_max_length);
	
	return $str;
}


# *************
# アクセスログ用のデータを生成する（文字コードを強制的にSJISに変換）
# array($ip,$hostname,$browser,$referer,$lang) = sub_make_accesslod_data()
sub sub_make_accesslod_data
{
	# アクセス元のIPアドレス
	my $ip = $ENV{'REMOTE_ADDR'};
	if(!defined($ip) || length($ip)<=0){ $ip = '0.0.0.0'; }
	
	$ip = sub_conv_to_safe_str($ip, 255);

	# アクセス元のホスト名
	my $hostname = $ENV{'REMOTE_HOST'};
	if(!defined($hostname) || length($hostname)<=0){ $hostname = ''; }
	if ($flag_ip_gethost && (($hostname eq "") || ($hostname eq $ip))) {
		if($ip eq '127.0.0.1' || $ip eq ''){ $hostname = 'localhost'; }
		elsif($ip eq '0.0.0.0'){ $hostname = ''; }
		else{ $hostname = gethostbyaddr(pack("C4", split(/\./, $ip)), 2); }
	}
	
	$hostname = sub_conv_to_safe_str($hostname, 255);

	# ブラウザ名（ユーザエージェント名）
	my $browser = $ENV{'HTTP_USER_AGENT'};
	if(!defined($browser) || length($browser)<=0){ $browser = ''; }

	$browser = sub_conv_to_safe_str($browser, 255);

	# 呼び出された元のホームページ名（リファラー）
	my $referer = $ENV{'HTTP_REFERER'};
	if(!defined($referer) || length($referer)<=0){ $referer = ''; }
	if($referer eq "" && defined($ENV{'REQUEST_URI'}) &&  $ENV{'REQUEST_URI'} ne "") { $referer = $ENV{'REQUEST_URI'};}
	# 格納できない文字を処理する
	$referer =~ s/%([0-9a-fA-F][0-9a-fA-F])/pack("C", hex($1))/eg;
	$referer =~ s/\\//g;
	$referer =~ s/\xa//eg;
	$referer =~ s/,//eg;

	# URLのドメイン部分を除去（または短縮形に置換）して、文字列長を短くする
	foreach(@arrHideReferer){
		if($referer =~ $_->[0]){
			$referer =~ s/$_->[0]/$_->[1]/;
			last;
		}
	}

	$referer = sub_conv_to_safe_str($referer, 255);

	# インストール言語
	my $lang = '';
	my $lang_raw = $ENV{'HTTP_ACCEPT_LANGUAGE'};
	if(!defined($lang_raw) || length($lang_raw)<=0){ $lang_raw = ''; }
	if($lang_raw =~ "en") { $lang = $lang . "en ";}
	if($lang_raw =~ "ja") { $lang = $lang . "ja ";}
	if($lang_raw =~ "zh") { $lang = $lang . "zh ";}
	if($lang_raw =~ "ko") { $lang = $lang . "ko ";}
	if($lang_raw =~ "fr") { $lang = $lang . "fr ";}
	if($lang_raw =~ "de") { $lang = $lang . "de ";}
	if($lang_raw =~ "es") { $lang = $lang . "es ";}
	if($lang_raw =~ "pt") { $lang = $lang . "pt ";}		# ポルトガル語
	if($lang_raw =~ "el") { $lang = $lang . "el ";}		# ギリシア語
	if($lang_raw =~ "it") { $lang = $lang . "it ";}
	if($lang_raw =~ "ru") { $lang = $lang . "ru ";}
	if($lang_raw =~ "ar") { $lang = $lang . "ar ";}		# アラビア語
	if($lang_raw =~ "nl") { $lang = $lang . "nl ";}
	if($lang_raw =~ "ro") { $lang = $lang . "ro ";}
	if($lang_raw =~ "hu") { $lang = $lang . "hu ";}
	if($lang_raw =~ "hr") { $lang = $lang . "hr ";}
	if($lang_raw =~ "cs") { $lang = $lang . "cs ";}
	if($lang_raw =~ "pl") { $lang = $lang . "pl ";}
	if($lang_raw =~ "cz") { $lang = $lang . "cz ";}
	if($lang_raw =~ "bg") { $lang = $lang . "bg ";}
	if($lang_raw =~ "ms") { $lang = $lang . "ms ";}
	if($lang_raw =~ "ro") { $lang = $lang . "ro ";}

	return($ip,$hostname,$browser,$referer,$lang);
}


# *************
# カウンターDB読み込み（カウンター値、ロック掛ける、同一IPチェック）
# ($nLocked, $nCounter, $nSameIp, $tm_lock) = 
#          sub_read_counter_db($ip, $flag_lock_mode)
sub sub_read_counter_db
{
	# 引数
	my $ip = shift;	# IPアドレス
	my $flag_lock_mode = shift;	# ロック機能を使う場合、1
	# 引数チェック
	if(!defined($ip) || $ip eq ''){ $ip = '0.0.0.0'; }
	if(!defined($flag_lock_mode) || $flag_lock_mode != 1 || $flag_lock_mode != 0){ $flag_lock_mode = 0; }

	my $tm_now = time();	# 現在日時
	my $nLocked = 1;	# 1:ロック、0:正常
	my $nCounter = 0;
	my $nSameIp = 0;	# 1:DBに登録されているIP、 0:DBに登録されていない
	my $tm_lock = 0;		# 読み出したロック時間
	
	my $dbh = undef;
	eval{
		# SQLサーバに接続
		$dbh = DBI->connect($str_dsn_counter, "", "", {PrintError => 0, AutoCommit => 1}) or die(DBI::errstr);

		# ロック状況を得る
		my $str_query = "SELECT tm_epock FROM locktbl WHERE idx = 0";
		my $sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute() or die(DBI::errstr);
		my @row = $sth->fetchrow_array();
		$sth->finish() or die(DBI::errstr);
		$sth = undef;	# 再利用するためundef

		if(@row){ $tm_lock = $row[0]; }
		else{ $tm_lock = 0; }		# 値がない場合は UNIX時刻 0 とする
		@row = ();	# 再利用するため配列初期化

		if($tm_now - $tm_lock > $tm_lock_timeout){ $nLocked = 0; }
		else{ $nLocked = 1;}

		# 現在のカウンター値を得る
		$str_query = "SELECT counter FROM countertbl WHERE idx = 0";
		$sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute() or die(DBI::errstr);
		@row = $sth->fetchrow_array();
		$sth->finish() or die(DBI::errstr);
		$sth = undef;	# 再利用するためundef
		
		if(@row){ $nCounter = $row[0]; }
		else{ $nCounter = 0; }
		@row = ();	# 再利用するため配列初期化

		# 指定されたIPアドレスが既に登録されているかチェックする
		$str_query = "SELECT COUNT (*) FROM iptbl WHERE ip = ?";
		$sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->bind_param(1, $ip, SQL_VARCHAR) or die(DBI::errstr);
		$sth->execute() or die(DBI::errstr);
		@row = $sth->fetchrow_array();
		$sth->finish() or die(DBI::errstr);
		$sth = undef;	# 再利用するためundef
		
		if(@row && $row[0] > 0){ $nSameIp = 1; }
		else{ $nSameIp = 0; }

		if($flag_lock_mode == 1 && $nLocked == 0)	# ロックが掛かっていない場合
		{
			# 次の2つの条件のどちらかの時にログを記録する
			# 1. ロックされておらず、$flag_ip_sameaddr_check==0のとき
			# 2. ロックされておらず、$flag_ip_sameaddr_check==1かつ$nSameIp==0のとき
			if($flag_ip_sameaddr_check == 0 || $nSameIp == 0)
			{
				$str_query = "UPDATE locktbl SET tm_epock = ? WHERE idx = 0";
				$sth = $dbh->prepare($str_query) or die(DBI::errstr);
				$sth->bind_param(1, $tm_now, SQL_INTEGER) or die(DBI::errstr);
				$sth->execute() or die(DBI::errstr);
				
				$tm_lock = $tm_now;
			}
		}

		# DBを閉じる
		$dbh->disconnect() or die(DBI::errstr);
	};
	if($@){
		# evalによるDBエラートラップ：エラー時の処理
		if(defined($dbh)){ $dbh->disconnect(); }
		my $str = $@;
		chomp($str);
		sub_write_to_debug_log("===SQLERROR_COUNTERDB_READ (".$str.")===");
	}

	return($nLocked, $nCounter, $nSameIp, $tm_lock);
}


# *************
# カウンターDBアップデート（カウンター書き込み、ロック解除、IP書き込み）
# sub_write_counter_db($ip, $n_counter)
sub sub_write_counter_db
{
	# 引数
	my $ip = shift;
	my $n_counter = shift;
	# 引数チェック
	if(!defined($ip) || $ip eq ''){ $ip = '0.0.0.0'; }
	if(!defined($n_counter) || $n_counter eq '' || $n_counter <= 0){ $n_counter = 0; }

	my $tm_now = time();	# 現在日時

	my $n_index = 0;	# アクセス元ipアドレスリストのインデックス

	my $dbh = undef;
	eval{
		# SQLサーバに接続
		$dbh = DBI->connect($str_dsn_counter, "", "", {PrintError => 0, AutoCommit => 1}) or die(DBI::errstr);

		# 現在のカウンター値を書き込む
		my $str_query = "UPDATE countertbl SET counter = ? WHERE idx = 0";
		my $sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->bind_param(1, $n_counter, SQL_INTEGER) or die(DBI::errstr);
		$sth->execute() or die(DBI::errstr);
		$sth->finish() or die(DBI::errstr);
		$sth = undef;	# 再利用するためundef

		# 最も古い時間の「指定IPアドレス」行のインデックスを取り出す
		$str_query = "SELECT idx FROM iptbl WHERE ip = ? ORDER BY tm_epock LIMIT 1";
		$sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->bind_param(1, $ip, SQL_VARCHAR) or die(DBI::errstr);
		$sth->execute($ip) or die(DBI::errstr);
		my @row = $sth->fetchrow_array();
		$sth->finish() or die(DBI::errstr);
		$sth = undef;	# 再利用するためundef
		
		if(@row)
		{	# IPアドレスが既に登録されていた場合
			$n_index = $row[0];
		}
		else
		{
			@row = ();	# 再利用するため配列初期化
			# 最も古い時間のインデックスを取り出す
			$str_query = "SELECT idx FROM iptbl ORDER BY tm_epock LIMIT 1";
			$sth = $dbh->prepare($str_query) or die(DBI::errstr);
			$sth->execute() or die(DBI::errstr);
			@row = $sth->fetchrow_array();
			$sth->finish() or die(DBI::errstr);
			$sth = undef;	# 再利用するためundef
			
			if(@row) { $n_index = $row[0]; }
			else { $n_index = 0; }
		}

		# IPアドレス、現在時刻を書き込む
		$str_query = "UPDATE iptbl SET ip = ?, tm_epock = ? WHERE idx = ?";

		$sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->bind_param(1, $ip, SQL_VARCHAR) or die(DBI::errstr);
		$sth->bind_param(2, $tm_now, SQL_INTEGER) or die(DBI::errstr);
		$sth->bind_param(3, $n_index, SQL_INTEGER) or die(DBI::errstr);
		$sth->execute() or die(DBI::errstr);
		$sth->finish() or die(DBI::errstr);
		$sth = undef;	# 再利用するためundef

		# ロック解除
		$str_query = "UPDATE locktbl SET tm_epock = 0 WHERE idx = 0";
		$sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute() or die(DBI::errstr);
		$sth->finish() or die(DBI::errstr);
		

		# DBを閉じる
		$dbh->disconnect() or die(DBI::errstr);
	};
	if($@){
		# evalによるDBエラートラップ：エラー時の処理
		if(defined($dbh)){ $dbh->disconnect(); }
		my $str = $@;
		chomp($str);
		sub_write_to_debug_log("===SQLERROR_COUNTERDB_UPDATE (".$str.")===");
	}

	return;
}


# *************
# 重複判定用IPアドレスリストを表示用に読み出す（テストモード用）
# $str_web = sub_textout_iplist();
sub sub_textout_iplist
{
	my $str = "<p>info : IP address list in counter DB<br />\n";
	
	my $dbh = undef;
	eval{
		# SQLサーバに接続
		$dbh = DBI->connect($str_dsn_counter, "", "", {PrintError => 0, AutoCommit => 1}) or die(DBI::errstr);

		# IPアドレスリストを全て読み出す
		my $str_query = "SELECT * FROM iptbl ORDER BY tm_epock";
		my $sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute() or die(DBI::errstr);

		my @row;
		while(@row = $sth->fetchrow_array())
		{
			$str .= sprintf("&nbsp;&nbsp;idx=%02d, ip=%-15s, time=%d/%02d/%02d %02d:%02d:%02d<br />\n", $row[0], $row[1], (localtime($row[2]))[5]+1900, (localtime($row[2]))[4]+1, (localtime($row[2]))[3], (localtime($row[2]))[2], (localtime($row[2]))[1], (localtime($row[2]))[0]);
		}

		$sth->finish() or die(DBI::errstr);
		# DBを閉じる
		$dbh->disconnect() or die(DBI::errstr);
	};
	if($@){
		# evalによるDBエラートラップ：エラー時の処理
		if(defined($dbh)){ $dbh->disconnect(); }
		my $str = $@;
		chomp($str);
		sub_write_to_debug_log("===SQLERROR_COUNTERDB_READ (".$str.")===");
	}

	$str .= "</p>\n";
	
	return($str);
}

# *************
# アクセスログデータベースに、ログを１行書き込む
# sub_write_accesslog_db($ip,$hostname,$browser,$referer,$lang)
sub sub_write_accesslog_db
{
	# 引数
	my $ip = shift;
	my $hostname = shift;
	my $browser = shift;
	my $referer = shift;
	my $lang = shift;
	# 引数チエック
	if(!defined($ip) || $ip eq ''){ $ip = ''; }
	if(!defined($hostname) || $hostname eq ''){ $hostname = ''; }
	if(!defined($browser) || $browser eq ''){ $browser = ''; }
	if(!defined($referer) || $referer eq ''){ $referer = ''; }
	if(!defined($lang) || $lang eq ''){ $lang = ''; }


	my $tm_now = time();	# 現在日時
	# 日時
	my ($year,$month,$day,$hour,$min,$sec,$n_weekday,$n_day_second) = sub_make_datetime_str();

	my $dbh = undef;
	eval{
		# DBに接続
		$dbh = DBI->connect($str_dsn_accesslog, "", "", {PrintError => 0, AutoCommit => 1}) or die(DBI::errstr);
		# クエリ作成
		my $str_query = "insert into acctbl values(null,?,?,?,?,?,?,?,?)";
		my $sth = $dbh->prepare($str_query) or die(DBI::errstr);

		# クエリ命令発行
		$sth->execute($tm_now, $n_weekday, $n_day_second, $ip, $hostname, $browser, $referer, $lang) or die(DBI::errstr);
		$sth->finish() or die(DBI::errstr);

		# DBを閉じる
		$dbh->disconnect() or die(DBI::errstr);
		
	};
	if($@){
		# evalによるDBエラートラップ：エラー時の処理
		if(defined($dbh)){ $dbh->disconnect(); }
		my $str = $@;
		chomp($str);
		sub_write_to_debug_log("===SQLERROR_ACCDB (".$str.")===");
	}

	return;
}


# *************
# テストモード（情報表示）
# sub_test_cgi($ip,$hostname,$browser,$referer,$lang, $tm_lock, $flag_lock, $flag_sameip, $n_counter);
sub sub_test_cgi
{
	# 引数
	my $ip = shift;
	my $hostname = shift;
	my $browser = shift;
	my $referer = shift;
	my $lang = shift;
	my $tm_lock = shift;
	my $flag_lock = shift;
	my $flag_sameip = shift;
	my $n_counter = shift;
	
	# 現在日時
	my $tm_now = time();

	my $str = "<p>=== counter and access log program diagnostic ===</p>\n";

	if($flag_lock > 0) { $str .= "<p>error : process locked</p>\n";}
	else { $str .= "<p>info : lock status clear, OK</p>\n";}
	$str .= sprintf("<p>info : lock time %d/%02d/%02d %02d:%02d:%02d</p>\n", (localtime($tm_lock))[5]+1900, (localtime($tm_lock))[4]+1, (localtime($tm_lock))[3], (localtime($tm_lock))[2], (localtime($tm_lock))[1], (localtime($tm_lock))[0]);
	$str .= sprintf("<p>info : current time %d/%02d/%02d %02d:%02d:%02d</p>\n", (localtime($tm_now))[5]+1900, (localtime($tm_now))[4]+1, (localtime($tm_now))[3], (localtime($tm_now))[2], (localtime($tm_now))[1], (localtime($tm_now))[0]);
	if($flag_sameip > 0) { $str .= "<p>info : IP address ".$ip." is listed in DB, OK</p>\n";}
	else { $str .= "<p>info : IP address ".$ip." is NOT listed in DB, OK</p>\n";}
	$str .= "<p>info : current counter = ".$n_counter.", OK</p>\n";

	my $dbh = undef;
	eval{
		# SQLサーバに接続
		$dbh = DBI->connect($str_dsn_accesslog, "", "", {PrintError => 1, AutoCommit => 0}) or die(DBI::errstr);

		# アクセスデータベース全体のデータ数
		my $str_query = "SELECT COUNT (*) FROM acctbl";
		my $sth = $dbh->prepare($str_query) or die(DBI::errstr);
		$sth->execute() or die(DBI::errstr);
		my @row = $sth->fetchrow_array();
		$sth->finish() or die(DBI::errstr);

		$str .= "<p>info : Access Log DB ".$row[0]." lines exist, OK</p>\n";
		$dbh->disconnect() or die(DBI::errstr);

	};
	if($@){
		# evalによるDBエラートラップ：エラー時の処理
		if(defined($dbh)){ $dbh->disconnect(); }
		my $str = $@;
		chomp($str);
		sub_write_to_debug_log("===SQLERROR_ACCDB (".$str.")===");
		$str .= "<p>DBI access error<br />\n";
	}
	
	
	$str .= "<p>ip = ".$ip."<br />\n";
	$str .= "hostname = ".$hostname."<br />\n";
	$str .= "browser = ".$browser."<br />\n";
	$str .= "referer = ".$referer."</p>\n";

	$str .= sub_textout_iplist();

	HTML_Output($str);
	
	return;
}


# *************
# エラー時のデバッグログCSVファイルへの書き込み
# sub_write_to_debug_log($str)
sub sub_write_to_debug_log
{
	# 引数
	my $str = shift;

	my $tm_now = time();	# 現在日時

	# ミリ秒単位での処理開始からの経過時間
	my $str_millitime_lapse = Time::HiRes::time() - $tmmStart;
	# 現在日時
	my $str_time_now .= sprintf("%d/%02d/%02d,%02d:%02d:%02d", (localtime($tm_now))[5]+1900, (localtime($tm_now))[4]+1, (localtime($tm_now))[3], (localtime($tm_now))[2], (localtime($tm_now))[1], (localtime($tm_now))[0]);
	# ロック日時
	my $str_time_lock .= sprintf("%d/%02d/%02d,%02d:%02d:%02d", (localtime($tm_lock))[5]+1900, (localtime($tm_lock))[4]+1, (localtime($tm_lock))[3], (localtime($tm_lock))[2], (localtime($tm_lock))[1], (localtime($tm_lock))[0]);

	# デバッグ用ファイルに書き込む
	open(OUT, ">> $str_filepath_debugcsv");	# 追記モード
	
	print(OUT $str_time_now.",lock=".$str_time_lock.",laps=".$str_millitime_lapse.",argv=".(defined($ARGV[0])?$ARGV[0]:'').",ip=".$ip.", hostname=".$hostname.",browser=".$browser.",referer=".$referer.",lock=".$flag_is_locked.",sameip=".$flag_is_ip_sameaddr);
	if(defined($str) && $str ne '') { print(OUT ",".$str);}
	print(OUT "\n");
	close(OUT);

	return;
}


# *************
# 日時データの文字列を作成する
# ($year,$month,$day,$hour,$min,$sec,$n_weekday,$n_day_second) = 
#   sub_make_datetime_str()
sub sub_make_datetime_str{

	# 現在日時
	my $tm_now = time();
	# 曜日を求める
	my $n_weekday = (localtime($tm_now))[6];
	my ($sec, $min, $hour, $day, $month, $year) = localtime($tm_now);
	# その日の0時からの秒数
	my $n_day_second = $hour*60*60 + $min*60 + $sec;

	return($year,$month,$day,$hour,$min,$sec,$n_weekday,$n_day_second);

}


# *************
# 文字コード相互変換サブルーチン

# 任意の文字コードの文字列を、UTF-8フラグ付きのUTF-8に変換する
sub sub_conv_to_flagged_utf8{
		my $str = shift;
		my $enc_force = undef;
		if(@_ >= 1){ $enc_force = shift; }			  # デコーダの強制指定
		
		# デコーダが強制的に指定された場合
		if(defined($enc_force)){
				if(ref($enc_force)){
						$str = $enc_force->decode($str);
						return($str);
				}
				elsif($enc_force ne '')
				{
						$str = Encode::decode($enc_force, $str);
				}
		}

		my $enc = Encode::Guess->guess($str);   # 文字列のエンコードの判定

		unless(ref($enc)){
				# エンコード形式が2個以上帰ってきた場合 （shiftjis or utf8）
				my @arr_encodes = split(/ /, $enc);
				if(grep(/^$flag_charcode/, @arr_encodes) >= 1){
						# $flag_charcode と同じエンコードが検出されたら、それを優先する
						$str = Encode::decode($flag_charcode, $str);
				}
				elsif(lc($arr_encodes[0]) eq 'shiftjis' || lc($arr_encodes[0]) eq 'euc-jp' || 
						lc($arr_encodes[0]) eq 'utf8' || lc($arr_encodes[0]) eq 'us-ascii'){
						# 最初の候補でデコードする
						$str = Encode::decode($arr_encodes[0], $str);
				}
		}
		else{
				# UTF-8でUTF-8フラグが立っている時以外は、変換を行う
				unless(ref($enc) eq 'Encode::utf8' && utf8::is_utf8($str) == 1){
						$str = $enc->decode($str);
				}
		}

		return($str);
}


# 任意の文字コードの文字列を、UTF-8フラグ無しのUTF-8に変換する
sub sub_conv_to_unflagged_utf8{
		my $str = shift;

		# いったん、フラグ付きのUTF-8に変換
		$str = sub_conv_to_flagged_utf8($str);

		return(Encode::encode('utf8', $str));
}


# UTF8から現在のOSの文字コードに変換する
sub sub_conv_to_local_charset{
		my $str = shift;

		# UTF8から、指定された（OSの）文字コードに変換する
		$str = Encode::encode($flag_charcode, $str);
		
		return($str);
}


# 引数で与えられたファイルの エンコードオブジェクト Encode::encode を返す
sub sub_get_encode_of_file{
		my $fname = shift;			  # 解析するファイル名

		# ファイルを一気に読み込む
		open(FH, "<".sub_conv_to_local_charset($fname));
		my @arr = <FH>;
		close(FH);
		my $str = join('', @arr);			   # 配列を結合して、一つの文字列に

		my $enc = Encode::Guess->guess($str);   # 文字列のエンコードの判定

		# エンコード形式の表示（デバッグ用）
		print("Automatick encode ");
		if(ref($enc) eq 'Encode::utf8'){ print("detect : utf8\n"); }
		elsif(ref($enc) eq 'Encode::Unicode'){
				print("detect : ".$$enc{'Name'}."\n");
		}
		elsif(ref($enc) eq 'Encode::XS'){
				print("detect : ".$enc->mime_name()."\n");
		}
		elsif(ref($enc) eq 'Encode::JP::JIS7'){
				print("detect : ".$$enc{'Name'}."\n");
		}
		else{
				# 二つ以上のエンコードが推定される場合は、$encに文字列が返る
				print("unknown (".$enc.")\n");
		}

		# エンコード形式が2個以上帰ってきた場合 （例：shiftjis or utf8）でテクと失敗と扱う
		unless(ref($enc)){
				$enc = '';
		}

		# ファイルがHTMLの場合 Content-Type から判定する
		if(lc($fname) =~ m/html$/ || lc($fname) =~ m/htm$/){
				my $parser = HTML::HeadParser->new();
				unless($parser->parse($str)){
						my $content_enc = $parser->header('content-type');
						if(defined($content_enc) && $content_enc ne '' && lc($content_enc) =~ m/text\/html/ ){
								if(lc($content_enc) =~ m/utf-8/){ $enc = 'utf8'; }
								elsif(lc($content_enc) =~ m/shift_jis/){ $enc = 'shiftjis'; }
								elsif(lc($content_enc) =~ m/euc-jp/){ $enc = 'euc-jp'; }
								
								print("HTML Content-Type detect : ". $content_enc ." (is overrided)\n");
								$enc = $content_enc;
						}
				}
		}

		return($enc);
}


# ************* (EOF)
